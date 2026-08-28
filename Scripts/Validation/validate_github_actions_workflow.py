#!/usr/bin/env python3
"""Validate the Android workflow without third-party Python dependencies."""
from pathlib import Path

root = Path(__file__).resolve().parents[2]
workflow_path = root / ".github/workflows/build-android-apk.yml"
text = workflow_path.read_text(encoding="utf-8")

required_strings = [
    "workflow_dispatch",
    "Development",
    "Shipping",
    "self-hosted",
    "Windows",
    "X64",
    "unreal-5.4",
    "android",
    "BuildAndroidMinimal.ps1",
    "UE_ROOT",
    "ANDROID_HOME",
    "ANDROID_NDK_HOME",
    "JAVA_HOME",
    "ANDROID_KEYSTORE_B64",
    "ash-line-android-apk",
    "ash-line-android-reports",
    "timeout-minutes: 180",
]
missing = [token for token in required_strings if token not in text]
if missing:
    raise SystemExit("Missing workflow requirements: " + ", ".join(missing))

for label in ("self-hosted", "Windows", "X64", "unreal-5.4", "android"):
    if label not in text:
        raise SystemExit(f"Runner label is missing: {label}")
if "- Development" not in text or "- Shipping" not in text:
    raise SystemExit("Development and Shipping workflow options are required")

build_script = (root / "Scripts/Build/BuildAndroidMinimal.ps1").read_text(encoding="utf-8")
if "'-package'" not in build_script and "-package" not in build_script:
    raise SystemExit("BuildAndroidMinimal.ps1 must pass -package to BuildCookRun")
if "SetupMinimalAndroidBuild.ps1" not in build_script:
    raise SystemExit("BuildAndroidMinimal.ps1 must resolve the minimal Unreal/Android environment")

print("GitHub Actions workflow validation: PASS")
