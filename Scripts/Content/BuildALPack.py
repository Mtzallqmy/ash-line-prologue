#!/usr/bin/env python3
"""Build a content-only ASH LINE .alpack without embedding native code."""
from __future__ import annotations
import argparse, base64, hashlib, json, os, shutil, subprocess, tempfile, zipfile
from pathlib import Path
from datetime import datetime, timezone

SCHEMA_VERSION = 1
FORBIDDEN_SUFFIXES = {".so", ".dll", ".dylib", ".exe", ".bat", ".cmd", ".sh", ".py", ".js"}

def sha256_file(path: Path) -> str:
    digest = hashlib.sha256()
    with path.open("rb") as handle:
        for block in iter(lambda: handle.read(1024 * 1024), b""):
            digest.update(block)
    return digest.hexdigest()

def validate_relative_path(relative: str) -> None:
    path = Path(relative)
    if path.is_absolute() or ".." in path.parts:
        raise ValueError(f"unsafe package path: {relative}")

def collect_content(source: Path):
    files = []
    for path in sorted(source.rglob("*")):
        if not path.is_file():
            continue
        relative = path.relative_to(source).as_posix()
        validate_relative_path(relative)
        if path.suffix.lower() in FORBIDDEN_SUFFIXES:
            raise ValueError(f"content-only package cannot contain executable/native file: {relative}")
        files.append((relative, path))
    return files

def content_hash(files) -> str:
    digest = hashlib.sha256()
    for relative, path in files:
        digest.update(relative.encode("utf-8"))
        digest.update(b"\0")
        with path.open("rb") as handle:
            for block in iter(lambda: handle.read(1024 * 1024), b""):
                digest.update(block)
    return digest.hexdigest()

def sign_manifest(manifest_path: Path, private_key: Path) -> bytes:
    result = subprocess.run(["openssl", "dgst", "-sha256", "-sign", str(private_key), "-in", str(manifest_path)], check=True, capture_output=True)
    return result.stdout

def build(args) -> Path:
    source = Path(args.source).resolve()
    output = Path(args.output).resolve()
    if not source.is_dir():
        raise FileNotFoundError(source)
    files = collect_content(source)
    total_size = sum(path.stat().st_size for _, path in files)
    if args.hard_size_bytes and total_size > args.hard_size_bytes:
        raise ValueError(f"content size {total_size} exceeds hard limit {args.hard_size_bytes}")
    content_digest = content_hash(files)
    manifest = {
        "schemaVersion": SCHEMA_VERSION,
        "packageId": args.package_id,
        "packageType": args.package_type,
        "version": args.version,
        "minGameVersion": args.min_game_version,
        "maxGameVersion": args.max_game_version,
        "downloadSize": 0,
        "installedSize": total_size,
        "dependencies": args.dependencies,
        "optionalDependencies": args.optional_dependencies,
        "mountPriority": args.mount_priority,
        "sha256": content_digest,
        "signature": "",
        "publicKeyId": args.public_key_id,
        "contentRoot": args.content_root,
        "entryAssets": args.entry_asset,
        "patchFromVersion": args.patch_from_version,
        "patchSize": args.patch_size,
        "builtAtUtc": datetime.now(timezone.utc).isoformat(),
    }
    with tempfile.TemporaryDirectory(prefix="ashline_alpack_") as temp_dir:
        staging = Path(temp_dir)
        content_dir = staging / "content"
        content_dir.mkdir()
        for relative, path in files:
            destination = content_dir / relative
            destination.parent.mkdir(parents=True, exist_ok=True)
            shutil.copy2(path, destination)
        manifest_path = staging / "manifest.json"
        manifest_path.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        key_path = Path(args.signing_key) if args.signing_key else None
        if key_path:
            signature = sign_manifest(manifest_path, key_path)
            (staging / "signature.dat").write_bytes(signature)
            manifest["signature"] = base64.b64encode(signature).decode("ascii")
            manifest_path.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
        elif args.require_signature:
            raise ValueError("--require-signature requires --signing-key; private keys must remain outside the repository")
        metadata = {"tool": "BuildALPack", "schemaVersion": SCHEMA_VERSION, "fileCount": len(files), "contentSize": total_size, "signed": bool(key_path)}
        (staging / "optional_metadata").mkdir()
        (staging / "optional_metadata" / "build.json").write_text(json.dumps(metadata, indent=2) + "\n", encoding="utf-8")
        output.parent.mkdir(parents=True, exist_ok=True)
        for _ in range(2):
            manifest_path.write_text(json.dumps(manifest, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
            with zipfile.ZipFile(output, "w", compression=zipfile.ZIP_DEFLATED, compresslevel=6) as archive:
                for path in sorted(staging.rglob("*")):
                    if path.is_file(): archive.write(path, path.relative_to(staging).as_posix())
            manifest["downloadSize"] = output.stat().st_size
    report = output.with_suffix(output.suffix + ".report.json")
    report.write_text(json.dumps({"package": manifest, "topFiles": [{"path": r, "bytes": p.stat().st_size} for r, p in sorted(files, key=lambda item: item[1].stat().st_size, reverse=True)[:30]]}, indent=2, ensure_ascii=False) + "\n", encoding="utf-8")
    return output

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("source")
    parser.add_argument("output")
    parser.add_argument("--package-id", required=True)
    parser.add_argument("--package-type", default="Mission")
    parser.add_argument("--version", type=int, default=1)
    parser.add_argument("--min-game-version", default="0.1.0")
    parser.add_argument("--max-game-version", default="")
    parser.add_argument("--mount-priority", type=int, default=100)
    parser.add_argument("--content-root", default="/Game/AshLine/Packages/")
    parser.add_argument("--dependency", dest="dependencies", action="append", default=[])
    parser.add_argument("--optional-dependency", dest="optional_dependencies", action="append", default=[])
    parser.add_argument("--entry-asset", action="append", default=[])
    parser.add_argument("--patch-from-version", type=int, default=0)
    parser.add_argument("--patch-size", type=int, default=0)
    parser.add_argument("--public-key-id", default="")
    parser.add_argument("--signing-key", default="")
    parser.add_argument("--require-signature", action="store_true")
    parser.add_argument("--hard-size-bytes", type=int, default=0)
    args = parser.parse_args()
    result = build(args)
    print(result)

if __name__ == "__main__":
    main()
