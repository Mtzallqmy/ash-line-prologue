#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
"$PROJECT_ROOT/Scripts/Build/ValidateBeforeBuild.sh"
UE_ROOT="${UE_ROOT:-}"
if [[ -z "$UE_ROOT" || ! -x "$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool" ]]; then
  echo "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT to a UE 5.4 installation." >&2
  exit 2
fi
UBT="$UE_ROOT/Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool"
"$UBT" ASH_LINE Linux Development -Project="$PROJECT_ROOT/ASH_LINE.uproject" -Progress
