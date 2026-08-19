[CmdletBinding()]
param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path,
    [switch]$RequireUnreal
)

$ErrorActionPreference = "Stop"
$validators = @(
    "validate_project.py",
    "validate_content_system.py",
    "validate_prompt02.py",
    "validate_prompt03.py",
    "validate_prompt04.py",
    "validate_prompt05.py",
    "validate_build_references.py",
    "static_surface_check.py"
)
foreach ($validator in $validators) {
    & python (Join-Path $ProjectRoot "Scripts/Validation/$validator") $ProjectRoot
    if ($LASTEXITCODE -ne 0) { throw "Validation failed: $validator" }
}

git -C $ProjectRoot diff --check
if ($LASTEXITCODE -ne 0) { throw "git diff --check failed" }

if ($RequireUnreal) {
    if ([string]::IsNullOrWhiteSpace($env:UE_ROOT)) { throw "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT." }
    $ubt = Join-Path $env:UE_ROOT "Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe"
    if (-not (Test-Path $ubt)) { throw "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: UnrealBuildTool missing at $ubt." }
}

Write-Host "Pre-build validation completed."
