#!/usr/bin/env bash
set -euo pipefail
PROJECT_ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/../.." && pwd)"
CONFIGURATION="${1:-Shipping}"
exec "$PROJECT_ROOT/Scripts/Build/BuildAndroidRelease.sh" "$CONFIGURATION"
