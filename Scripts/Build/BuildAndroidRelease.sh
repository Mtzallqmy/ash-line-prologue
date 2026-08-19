#!/usr/bin/env bash
set -euo pipefail

PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
PROJECT_FILE="$PROJECT_ROOT/ASH_LINE.uproject"
CONFIGURATION="${1:-Shipping}"
UE_ROOT="${UE_ROOT:-}"
RELEASE_ROOT="$PROJECT_ROOT/Releases/Android/0.0.1"
ARCHIVE_ROOT="$RELEASE_ROOT/.ue_archive_$CONFIGURATION"

fail() { echo "ANDROID BUILD BLOCKED: $*" >&2; exit 2; }
[[ -n "$UE_ROOT" ]] || fail "Set UE_ROOT to the installed Unreal Engine 5.4 directory."
[[ -f "$PROJECT_FILE" ]] || fail "ASH_LINE.uproject is missing."
UBT="$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool"
UAT="$UE_ROOT/Engine/Build/BatchFiles/RunUAT.sh"
[[ -x "$UBT" ]] || fail "UnrealBuildTool not found at $UBT."
[[ -x "$UAT" ]] || fail "RunUAT.sh not found at $UAT."
[[ "$CONFIGURATION" == "Development" || "$CONFIGURATION" == "Shipping" ]] || fail "Configuration must be Development or Shipping."

if [[ "$CONFIGURATION" == "Shipping" ]]; then
  [[ -n "${ANDROID_KEYSTORE:-}" ]] || fail "Shipping requires ANDROID_KEYSTORE outside the repository."
  [[ -f "${ANDROID_KEYSTORE}" ]] || fail "ANDROID_KEYSTORE does not point to a file."
  [[ -n "${ANDROID_KEY_ALIAS:-}" ]] || fail "Shipping requires ANDROID_KEY_ALIAS."
  [[ -n "${ANDROID_KEYSTORE_PASSWORD:-}" ]] || fail "Shipping requires ANDROID_KEYSTORE_PASSWORD from a secure environment."
  [[ -n "${ANDROID_KEY_PASSWORD:-}" ]] || fail "Shipping requires ANDROID_KEY_PASSWORD from a secure environment."
fi

python3 "$PROJECT_ROOT/Scripts/Validation/validate_project.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_content_system.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt05.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt04.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt03.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt02.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_build_references.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/static_surface_check.py" "$PROJECT_ROOT"
mkdir -p "$RELEASE_ROOT/APK" "$RELEASE_ROOT/Symbols" "$RELEASE_ROOT/Reports" "$RELEASE_ROOT/Checksums" "$ARCHIVE_ROOT"

if [[ -z "${ANDROID_HOME:-${ANDROID_SDK_ROOT:-}}" && -d "/usr/lib/android-sdk" ]]; then
  export ANDROID_HOME="/usr/lib/android-sdk"
  export ANDROID_SDK_ROOT="$ANDROID_HOME"
fi
if [[ -z "${ANDROID_NDK_HOME:-${NDK_HOME:-}}" && -d "$ANDROID_HOME/ndk" ]]; then
  export ANDROID_NDK_HOME="$(find "$ANDROID_HOME/ndk" -mindepth 1 -maxdepth 1 -type d | sort -V | tail -1)"
  export NDK_HOME="$ANDROID_NDK_HOME"
fi
export ANDROID_HOME="${ANDROID_HOME:-${ANDROID_SDK_ROOT:-}}"
[[ -n "$ANDROID_HOME" && -d "$ANDROID_HOME" ]] || fail "ANDROID_HOME/ANDROID_SDK_ROOT is not configured."
[[ -n "${ANDROID_NDK_HOME:-${NDK_HOME:-}}" && -d "${ANDROID_NDK_HOME:-${NDK_HOME:-}}" ]] || fail "ANDROID_NDK_HOME/NDK_HOME is not configured."
command -v java >/dev/null 2>&1 || fail "JDK is not available."
command -v adb >/dev/null 2>&1 || echo "Warning: adb is not available; install verification will be skipped."

TOOLCHAIN_REPORT="$RELEASE_ROOT/Reports/toolchain_$CONFIGURATION.txt"
{
  echo "Configuration=$CONFIGURATION"
  echo "Project=$PROJECT_FILE"
  echo "UE_ROOT=$UE_ROOT"
  echo "UBT_VERSION=$($UBT -Version 2>&1 | head -1 || true)"
  echo "JAVA_VERSION=$(java -version 2>&1 | head -1)"
  echo "ANDROID_HOME=$ANDROID_HOME"
  echo "ANDROID_NDK_HOME=${ANDROID_NDK_HOME:-${NDK_HOME:-}}"
  echo "SDKMANAGER=$ANDROID_HOME/cmdline-tools/latest/bin/sdkmanager"
  echo "BUILD_TOOLS=$(find "$ANDROID_HOME/build-tools" -maxdepth 1 -mindepth 1 -type d 2>/dev/null | sort | tail -1 || true)"
  echo "NDK=$(find "${ANDROID_NDK_HOME:-${NDK_HOME:-}}" -maxdepth 1 -mindepth 1 -type d 2>/dev/null | sort | tail -1 || true)"
  echo "ABI=arm64-v8a"
  echo "MinSDK=26"
  echo "TargetSDK=from UE 5.4 AndroidRuntimeSettings; configured 34"
  echo "Renderer=OpenGL ES compatibility path; Vulkan optional"
} > "$TOOLCHAIN_REPORT"

UAT_ARGS=(BuildCookRun "-project=$PROJECT_FILE" -nop4 -utf8output -platform=Android "-clientconfig=$CONFIGURATION" -build -cook -stage -pak -archive "-archivedirectory=$ARCHIVE_ROOT" -prereqs)
if [[ "$CONFIGURATION" == "Shipping" ]]; then UAT_ARGS+=( -distribution ); fi
"$UAT" "${UAT_ARGS[@]}"

APK_SOURCE="$(find "$ARCHIVE_ROOT" -type f -iname '*.apk' | head -1)"
[[ -n "$APK_SOURCE" ]] || fail "Unreal completed without producing an APK."
if [[ "$CONFIGURATION" == "Shipping" ]]; then
  APK_TARGET="$RELEASE_ROOT/APK/AshLine_CombatPrototype_v0.0.1_android_arm64.apk"
else
  APK_TARGET="$RELEASE_ROOT/APK/AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk"
fi
cp -f "$APK_SOURCE" "$APK_TARGET"
python3 "$PROJECT_ROOT/Scripts/Validation/verify_android_release.py" "$APK_TARGET" "$RELEASE_ROOT/Reports/release_$CONFIGURATION.json"
sha256sum "$APK_TARGET" > "$RELEASE_ROOT/Checksums/$(basename "$APK_TARGET").sha256"
sha256sum "$APK_TARGET" > "$RELEASE_ROOT/Checksums/SHA256.txt"
APK_BYTES="$(stat -c '%s' "$APK_TARGET")"
cat > "$RELEASE_ROOT/Reports/ReleaseReport.md" <<EOF
# ASH LINE Combat Prototype v0.0.1 — Release Report

Configuration: $CONFIGURATION
APK: $(basename "$APK_TARGET")
APK path: $APK_TARGET
APK size bytes: $APK_BYTES
SHA-256: $(cut -d' ' -f1 "$RELEASE_ROOT/Checksums/SHA256.txt")
ABI: arm64-v8a
Package: com.ashline.game
Version: 0.0.1 (1)

This report was generated after Unreal BuildCookRun produced and verification accepted the APK.
EOF
cat > "$RELEASE_ROOT/Reports/SizeReport.md" <<EOF
# ASH LINE Combat Prototype v0.0.1 — Size Report

APK: $(basename "$APK_TARGET")
Bytes: $APK_BYTES
Target: <= 500 MB
Preferred prototype target: <= 300 MB where practical
EOF
echo "APK_READY=$APK_TARGET"
