#!/usr/bin/env python3
"""Validate Unreal build references that can be checked without UnrealBuildTool."""
from __future__ import annotations

import configparser
import json
import re
import sys
from pathlib import Path

ROOT = Path(sys.argv[1] if len(sys.argv) > 1 else ".").resolve()
errors: list[str] = []
warnings: list[str] = []


def read_text(path: Path) -> str:
    return path.read_text(encoding="utf-8") if path.exists() else ""


def parse_build_dependencies(path: Path) -> set[str]:
    text = read_text(path)
    return set(re.findall(r'"([A-Za-z0-9_]+)"', text))


project_path = ROOT / "ASH_LINE.uproject"
if not project_path.exists():
    errors.append("ASH_LINE.uproject is missing")
    project = {}
else:
    project = json.loads(project_path.read_text(encoding="utf-8"))

module_names = {module["Name"] for module in project.get("Modules", [])}
expected_modules = {
    "AshLineCore",
    "AshLineCharacters",
    "AshLineCombat",
    "AshLineWeapons",
    "AshLineAI",
    "AshLineDrones",
    "AshLineMissions",
    "AshLineContent",
    "AshLineUI",
    "AshLinePlatform",
}
missing_modules = expected_modules - module_names
if missing_modules:
    errors.append("Missing project modules: " + ", ".join(sorted(missing_modules)))

real_plugins = {"EnhancedInput", "PythonScriptPlugin", "EditorScriptingUtilities"}
for plugin in project.get("Plugins", []):
    if plugin.get("Enabled") and plugin.get("Name") not in real_plugins:
        errors.append(f"Unknown or non-plugin entry in .uproject Plugins: {plugin.get('Name')}")

build_files = {path.parent.name: path for path in (ROOT / "Source").glob("*/*.Build.cs")}
module_graph: dict[str, set[str]] = {name: set() for name in build_files}
for module, path in build_files.items():
    dependencies = parse_build_dependencies(path)
    unknown_project_deps = {dep for dep in dependencies if dep.startswith("AshLine") and dep not in module_names}
    if unknown_project_deps:
        errors.append(f"{module} references missing module(s): {', '.join(sorted(unknown_project_deps))}")
    module_graph[module] = {dep for dep in dependencies if dep in module_graph}

visiting: set[str] = set()
visited: set[str] = set()

def visit(module: str, trail: list[str]) -> None:
    if module in visiting:
        cycle = " -> ".join(trail + [module])
        errors.append("Circular module dependency: " + cycle)
        return
    if module in visited:
        return
    visiting.add(module)
    for dependency in sorted(module_graph.get(module, ())):
        visit(dependency, trail + [module])
    visiting.remove(module)
    visited.add(module)

for module in sorted(module_graph):
    visit(module, [])

engine_ini = read_text(ROOT / "Config/DefaultEngine.ini")
required_config_tokens = [
    "PackageName=com.ashline.game",
    "MinSDKVersion=26",
    "TargetSDKVersion=34",
    "bBuildForArm64=True",
    "bBuildForArmV7=False",
    "bBuildForX86=False",
    "bBuildForX8664=False",
    "Orientation=Landscape",
]
for token in required_config_tokens:
    if token not in engine_ini:
        errors.append(f"Missing mandatory engine config: {token}")
if not ("GlobalDefaultGameMode=/Script/AshLineMissions.ALCombatPrototypeGameMode" in engine_ini or "GlobalDefaultGameMode=/Script/AshLineCharacters.ALPrologueGameMode" in engine_ini):
    errors.append("Missing a valid ASH LINE GameMode config")

map_matches = re.findall(r"^(?:GameDefaultMap|EditorStartupMap)=(.+)$", engine_ini, flags=re.MULTILINE)
for map_path in map_matches:
    map_path = map_path.strip()
    if not map_path:
        errors.append("Default map setting is empty")
    elif map_path.startswith("/Game/"):
        relative = map_path.removeprefix("/Game/") + ".umap"
        if not (ROOT / "Content" / relative).exists():
            errors.append(f"Missing project map asset: {map_path}")
    elif map_path.startswith("/Engine/"):
        warnings.append(f"Engine map deferred to installed Unreal content: {map_path}")
    else:
        errors.append(f"Invalid map reference: {map_path}")

manifest_path = ROOT / "Content/AshLine/Data/content_manifest.json"
weapon_catalog_path = ROOT / "Content/AshLine/Data/weapon_catalog.json"
if manifest_path.exists():
    manifest = json.loads(manifest_path.read_text(encoding="utf-8"))
    package_ids = [package.get("id") for package in manifest.get("packages", [])]
    if len(package_ids) != len(set(package_ids)):
        errors.append("Duplicate PackageID found")
    for package in manifest.get("packages", []):
        for asset in package.get("entryAssets", []):
            if asset.startswith("/Game/"):
                relative = asset.removeprefix("/Game/")
                if not ((ROOT / "Content" / (relative + ".uasset")).exists() or (ROOT / "Content" / (relative + ".umap")).exists()):
                    errors.append(f"Missing manifest entry asset: {asset}")
if weapon_catalog_path.exists():
    weapons = json.loads(weapon_catalog_path.read_text(encoding="utf-8")).get("weapons", [])
    weapon_ids = [weapon.get("id") for weapon in weapons]
    if len(weapon_ids) != len(set(weapon_ids)):
        errors.append("Duplicate weapon ID found")

required_scripts = [
    "Scripts/Build/ValidateBeforeBuild.sh",
    "Scripts/Build/BuildEditor.sh",
    "Scripts/Build/BuildDevelopment.sh",
    "Scripts/Build/BuildAndroidDevelopment.sh",
    "Scripts/Build/BuildAndroidShipping.sh",
    "Scripts/Build/BuildAndroidRelease.sh",
    "Scripts/Build/ValidateBeforeBuild.ps1",
    "Scripts/Build/BuildAndroidDevelopment.ps1",
    "Scripts/Build/BuildAndroidShipping.ps1",
    "Scripts/Build/BuildEditor.ps1",
    "Scripts/Build/BuildAndroidPrototype.ps1",
    "Scripts/Build/BuildAndroidPrototype.sh",
    "Scripts/Editor/CreatePrototypeAssets.py",
    "Scripts/Validation/verify_android_release.py",
    "Releases/Android/0.0.1/Reports/ReleaseReport.md",
    "Releases/Android/0.0.1/Reports/SizeReport.md",
    "Releases/Android/0.0.1/Reports/TestReport.md",
    "Releases/Android/0.0.1/Checksums/SHA256.txt",
    "Docs/Build/CombatPrototype_v0.0.1.md",
    "Docs/Build/ReleasePrompt08Report.md",
    "Docs/Build/ReleasePrompt09Report.md",
]
for required_script in required_scripts:
    if not (ROOT / required_script).exists():
        errors.append(f"Missing required build/editor script: {required_script}")
if not (ROOT / "Docs/Build/PreFixAudit.md").exists():
    errors.append("Missing pre-fix audit report")

print("ASH LINE build-reference validation")
print("===================================")
print(f"Errors: {len(errors)}")
for error in errors:
    print("ERROR:", error)
print(f"Warnings: {len(warnings)}")
for warning in warnings:
    print("WARNING:", warning)
if not errors:
    print("PASS: build references, module graph, IDs, config, and project asset references are coherent")
sys.exit(1 if errors else 0)
