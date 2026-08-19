#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_project.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_content_system.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt02.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt03.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt04.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_prompt05.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/validate_build_references.py" "$PROJECT_ROOT"
python3 "$PROJECT_ROOT/Scripts/Validation/static_surface_check.py" "$PROJECT_ROOT"
git -C "$PROJECT_ROOT" diff --check
if [[ "${1:-}" == "--require-ue" ]]; then
  [[ -n "${UE_ROOT:-}" ]] || { echo "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT." >&2; exit 2; }
  [[ -x "$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool" ]] || { echo "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: UnrealBuildTool missing." >&2; exit 2; }
fi
echo "Pre-build validation completed."
