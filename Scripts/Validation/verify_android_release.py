#!/usr/bin/env python3
from __future__ import annotations
import hashlib, json, shutil, subprocess, sys, zipfile
from pathlib import Path

apk = Path(sys.argv[1]) if len(sys.argv) > 1 else None
report_path = Path(sys.argv[2]) if len(sys.argv) > 2 else (apk.with_suffix('.json') if apk else Path('android_release_report.json'))
if not apk or not apk.is_file():
    print('APK verification blocked: APK file does not exist', file=sys.stderr)
    raise SystemExit(2)

errors = []
warnings = []
with zipfile.ZipFile(apk) as archive:
    names = archive.namelist()
    abi_dirs = sorted({name.split('/')[1] for name in names if name.startswith('lib/') and name.count('/') >= 2})
    if 'arm64-v8a' not in abi_dirs: errors.append('arm64-v8a native libraries are missing')
    for forbidden in {'armeabi-v7a', 'x86', 'x86_64'}:
        if forbidden in abi_dirs: errors.append(f'forbidden ABI present: {forbidden}')
    native_entries = [name for name in names if name.startswith('lib/') and name.endswith('.so')]
    manifest = next((name for name in names if name == 'AndroidManifest.xml'), None)
    if not manifest: errors.append('AndroidManifest.xml is missing')

apksigner = shutil.which('apksigner')
aapt = shutil.which('aapt') or shutil.which('aapt2')
signature = 'NOT_CHECKED'
manifest_text = ''
if apksigner:
    result = subprocess.run([apksigner, 'verify', '--verbose', str(apk)], capture_output=True, text=True)
    signature = 'PASS' if result.returncode == 0 else f'FAIL: {result.stderr.strip()[:500]}'
    if result.returncode != 0: errors.append('apksigner verification failed')
else:
    warnings.append('apksigner unavailable; APK signature was not verified')
if aapt:
    result = subprocess.run([aapt, 'dump', 'badging', str(apk)], capture_output=True, text=True)
    manifest_text = result.stdout
    if result.returncode != 0: warnings.append('aapt could not dump Android manifest')
else:
    warnings.append('aapt/aapt2 unavailable; minSdk/targetSdk/permissions were not decoded')

sha = hashlib.sha256(apk.read_bytes()).hexdigest()
size = apk.stat().st_size
report = {
    'apk': str(apk),
    'filename': apk.name,
    'apkBytes': size,
    'apkMiB': round(size / (1024 * 1024), 3),
    'sha256': sha,
    'abiDirs': abi_dirs,
    'nativeEntries': native_entries,
    'signature': signature,
    'manifestDecoded': bool(manifest_text),
    'manifestText': manifest_text,
    'minSdkExpected': 26,
    'packageExpected': 'com.ashline.game',
    'versionNameExpected': '0.0.1',
    'versionCodeExpected': 1,
    'errors': errors,
    'warnings': warnings,
}
report_path.parent.mkdir(parents=True, exist_ok=True)
report_path.write_text(json.dumps(report, indent=2, ensure_ascii=False) + '\n', encoding='utf-8')
print(json.dumps({k: report[k] for k in ['filename', 'apkBytes', 'sha256', 'abiDirs', 'signature', 'errors', 'warnings']}, indent=2, ensure_ascii=False))
raise SystemExit(1 if errors else 0)
