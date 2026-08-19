[CmdletBinding()]
param(
    [ValidateSet("Development", "Shipping")]
    [string]$Configuration = "Shipping",
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
)

$ErrorActionPreference = "Stop"
& (Join-Path $PSScriptRoot "ValidateBeforeBuild.ps1") -ProjectRoot $ProjectRoot -RequireUnreal

if ([string]::IsNullOrWhiteSpace($env:ANDROID_HOME) -and [string]::IsNullOrWhiteSpace($env:ANDROID_SDK_ROOT)) { throw "ANDROID_HOME or ANDROID_SDK_ROOT is required." }
if ([string]::IsNullOrWhiteSpace($env:ANDROID_NDK_HOME) -and [string]::IsNullOrWhiteSpace($env:NDK_HOME)) { throw "ANDROID_NDK_HOME or NDK_HOME is required." }
if ([string]::IsNullOrWhiteSpace((Get-Command java -ErrorAction SilentlyContinue))) { throw "JDK is required." }
if ($Configuration -eq "Shipping") {
    foreach ($name in @("ANDROID_KEYSTORE", "ANDROID_KEY_ALIAS", "ANDROID_KEYSTORE_PASSWORD", "ANDROID_KEY_PASSWORD")) {
        if ([string]::IsNullOrWhiteSpace((Get-Item "Env:$name" -ErrorAction SilentlyContinue).Value)) { throw "Shipping requires $name from a secure environment." }
    }
    if (-not (Test-Path $env:ANDROID_KEYSTORE)) { throw "ANDROID_KEYSTORE does not point to a file." }
}

$ueRoot = $env:UE_ROOT
$uat = Join-Path $ueRoot "Engine/Build/BatchFiles/RunUAT.bat"
if (-not (Test-Path $uat)) { throw "RunUAT.bat not found at $uat" }
$projectFile = Join-Path $ProjectRoot "ASH_LINE.uproject"
$releaseRoot = Join-Path $ProjectRoot "Releases/Android/0.0.1"
$archiveRoot = Join-Path $releaseRoot ".ue_archive_$Configuration"
$apkRoot = Join-Path $releaseRoot "APK"
$reportsRoot = Join-Path $releaseRoot "Reports"
$checksumsRoot = Join-Path $releaseRoot "Checksums"
New-Item -ItemType Directory -Force -Path $apkRoot, $reportsRoot, $checksumsRoot, $archiveRoot | Out-Null

& $uat BuildCookRun "-project=$projectFile" -nop4 -utf8output -platform=Android "-clientconfig=$Configuration" -build -cook -stage -pak -archive "-archivedirectory=$archiveRoot" -prereqs $(if ($Configuration -eq "Shipping") { "-distribution" })
if ($LASTEXITCODE -ne 0) { throw "Unreal Android $Configuration BuildCookRun failed." }

$apk = Get-ChildItem -Path $archiveRoot -Recurse -Filter *.apk | Select-Object -First 1
if (-not $apk) { throw "Unreal completed without producing an APK." }
$targetName = if ($Configuration -eq "Shipping") { "AshLine_CombatPrototype_v0.0.1_android_arm64.apk" } else { "AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk" }
$target = Join-Path $apkRoot $targetName
Copy-Item $apk.FullName $target -Force
$hash = (Get-FileHash -Algorithm SHA256 $target).Hash.ToLowerInvariant()
"$hash  $targetName" | Set-Content (Join-Path $checksumsRoot "SHA256.txt")

@"
# ASH LINE Combat Prototype Release Report

Configuration: $Configuration
APK: $targetName
APK path: $target
APK size bytes: $((Get-Item $target).Length)
SHA-256: $hash
ABI: arm64-v8a
Package: com.ashline.game
Version: 0.0.1 (1)

This report was generated only after Unreal BuildCookRun produced an APK.
"@ | Set-Content (Join-Path $reportsRoot "ReleaseReport.md")

@"
# ASH LINE Combat Prototype Size Report

APK: $targetName
Bytes: $((Get-Item $target).Length)
Target: <= 500 MB
Preferred prototype target: <= 300 MB where practical
"@ | Set-Content (Join-Path $reportsRoot "SizeReport.md")

Write-Host "APK_READY=$target"
