[CmdletBinding()]
param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path,
    [switch]$RequireUnreal
)

$ErrorActionPreference = "Stop"
$pythonExe = $env:PYTHON_EXE
if ([string]::IsNullOrWhiteSpace($pythonExe) -and -not [string]::IsNullOrWhiteSpace($env:UE_ROOT)) {
    $embeddedPython = Join-Path $env:UE_ROOT 'Engine\Binaries\ThirdParty\Python3\Win64\python.exe'
    if (Test-Path -LiteralPath $embeddedPython -PathType Leaf) { $pythonExe = $embeddedPython }
}
if ([string]::IsNullOrWhiteSpace($pythonExe)) {
    $pythonCommand = Get-Command python.exe -ErrorAction SilentlyContinue
    if ($pythonCommand) { $pythonExe = $pythonCommand.Source }
}
if ([string]::IsNullOrWhiteSpace($pythonExe) -or -not (Test-Path -LiteralPath $pythonExe -PathType Leaf)) {
    throw 'Python is required for repository validators. Unreal embedded Python is sufficient after Unreal is installed.'
}
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
    & $pythonExe (Join-Path $ProjectRoot "Scripts/Validation/$validator") $ProjectRoot
    if ($LASTEXITCODE -ne 0) { throw "Validation failed: $validator" }
}

if (Test-Path -LiteralPath (Join-Path $ProjectRoot '.git') -PathType Container) {
    git -C $ProjectRoot diff --check
    if ($LASTEXITCODE -ne 0) { throw "git diff --check failed" }
} else {
    Write-Warning "No .git directory found; skipping git diff --check (ZIP/source archive build mode)."
}

if ($RequireUnreal) {
    if ([string]::IsNullOrWhiteSpace($env:UE_ROOT)) { throw "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: set UE_ROOT." }
    $ubt = Join-Path $env:UE_ROOT "Engine/Binaries/DotNET/UnrealBuildTool/UnrealBuildTool.exe"
    if (-not (Test-Path $ubt)) { throw "UNREAL BUILD ENVIRONMENT NOT AVAILABLE: UnrealBuildTool missing at $ubt." }
}

Write-Host "Pre-build validation completed."
