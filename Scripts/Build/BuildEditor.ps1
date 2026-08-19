[CmdletBinding()]
param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
)

$ErrorActionPreference = "Stop"
& (Join-Path $PSScriptRoot "ValidateBeforeBuild.ps1") -ProjectRoot $ProjectRoot -RequireUnreal
$ubt = Join-Path $env:UE_ROOT "Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe"
if (-not (Test-Path $ubt)) { throw "UnrealBuildTool.exe not found at $ubt" }
$projectFile = Join-Path $ProjectRoot "ASH_LINE.uproject"
& $ubt "ASH_LINEEditor" "Win64" "Development" "-Project=$projectFile" "-WaitMutex"
if ($LASTEXITCODE -ne 0) { throw "ASH_LINEEditor Win64 Development build failed with exit code $LASTEXITCODE." }
Write-Host "EDITOR BUILD: PASS"
