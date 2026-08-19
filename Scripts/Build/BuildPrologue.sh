#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
UE_ROOT="${UE_ROOT:-}"
if [[ -z "$UE_ROOT" ]]; then echo "Set UE_ROOT to your Unreal Engine installation before building." >&2; exit 2; fi
UBT="$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool"
if [[ ! -x "$UBT" ]]; then echo "UnrealBuildTool not found at $UBT" >&2; exit 2; fi
python3 "$PROJECT_ROOT/Scripts/Validation/validate_project.py" "$PROJECT_ROOT"
"$UBT" ASH_LINEEditor Linux Development -Project="$PROJECT_ROOT/ASH_LINE.uproject" -Progress
# Cook/package commands are intentionally explicit and must be adapted to the installed UE version and Android SDK.
echo "Compile validation completed. Run Unreal AutomationTool for Android Cook/Package, then Scripts/Validation/size_report.py."
