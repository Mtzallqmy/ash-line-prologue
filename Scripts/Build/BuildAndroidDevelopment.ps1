[CmdletBinding()]
param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
)

$ErrorActionPreference = "Stop"
& (Join-Path $PSScriptRoot "ValidateBeforeBuild.ps1") -ProjectRoot $ProjectRoot -RequireUnreal
if ([string]::IsNullOrWhiteSpace($env:ANDROID_HOME) -and [string]::IsNullOrWhiteSpace($env:ANDROID_SDK_ROOT)) { throw "ANDROID_HOME or ANDROID_SDK_ROOT is required." }
if ([string]::IsNullOrWhiteSpace($env:ANDROID_NDK_HOME) -and [string]::IsNullOrWhiteSpace($env:NDK_HOME)) { throw "ANDROID_NDK_HOME or NDK_HOME is required." }

$ueRoot = $env:UE_ROOT
$uat = Join-Path $ueRoot "Engine/Build/BatchFiles/RunUAT.bat"
if (-not (Test-Path $uat)) { throw "RunUAT.bat not found at $uat" }
$archive = Join-Path $ProjectRoot "Releases/Android/0.0.1/.ue_archive_Development"
& $uat BuildCookRun "-project=$(Join-Path $ProjectRoot 'ASH_LINE.uproject')" -nop4 -utf8output -platform=Android -clientconfig=Development -build -cook -stage -pak -archive "-archivedirectory=$archive" -prereqs
if ($LASTEXITCODE -ne 0) { throw "Unreal Android Development BuildCookRun failed." }
