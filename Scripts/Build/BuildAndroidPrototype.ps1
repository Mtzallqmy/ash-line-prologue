[CmdletBinding()]
param(
    [ValidateSet("Development", "Shipping")]
    [string]$Configuration = "Development",
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot "../..")).Path
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ([string]::IsNullOrWhiteSpace($env:UE_ROOT) -or -not (Test-Path -LiteralPath $env:UE_ROOT -PathType Container)) {
    & (Join-Path $PSScriptRoot 'ResolveBuildEnvironment.ps1')
}
& (Join-Path $PSScriptRoot "ValidateBeforeBuild.ps1") -ProjectRoot $ProjectRoot -RequireUnreal

if ([string]::IsNullOrWhiteSpace($env:ANDROID_HOME) -and [string]::IsNullOrWhiteSpace($env:ANDROID_SDK_ROOT)) { throw "ANDROID_HOME or ANDROID_SDK_ROOT is required." }
if ([string]::IsNullOrWhiteSpace($env:ANDROID_NDK_HOME) -and [string]::IsNullOrWhiteSpace($env:NDK_HOME)) { throw "ANDROID_NDK_HOME or NDK_HOME is required." }
$javaExe = Join-Path $env:JAVA_HOME 'bin\java.exe'
if (-not (Test-Path -LiteralPath $javaExe -PathType Leaf)) { throw "JDK executable is required at $javaExe" }

$ueRoot = $env:UE_ROOT
$uat = Join-Path $ueRoot "Engine\Build\BatchFiles\RunUAT.bat"
if (-not (Test-Path -LiteralPath $uat -PathType Leaf)) { throw "RunUAT.bat not found at $uat" }
$projectFile = Join-Path $ProjectRoot "ASH_LINE.uproject"
$releaseRoot = Join-Path $ProjectRoot "Releases\Android\0.0.1"
$archiveRoot = Join-Path $releaseRoot ".ue_archive_$Configuration"
$apkRoot = Join-Path $releaseRoot "APK"
$reportsRoot = Join-Path $releaseRoot "Reports"
$checksumsRoot = Join-Path $releaseRoot "Checksums"
New-Item -ItemType Directory -Force -Path $apkRoot, $reportsRoot, $checksumsRoot, $archiveRoot | Out-Null

$engineIni = Join-Path $ProjectRoot 'Config\DefaultEngine.ini'
$engineIniBackup = Get-Content -LiteralPath $engineIni -Raw
$temporaryKeystore = $null

function Set-AndroidSigningValue([string]$Content, [string]$Key, [string]$Value) {
    $escaped = $Value.Replace('"', '\"')
    $line = $Key + '="' + $escaped + '"'
    $pattern = "(?m)^$([regex]::Escape($Key))=.*$"
    if ($Content -match $pattern) { return [regex]::Replace($Content, $pattern, $line) }
    $sectionPattern = '(?ms)(\[/Script/AndroidRuntimeSettings\.AndroidRuntimeSettings\]\s*)'
    if ($Content -notmatch $sectionPattern) { $Content += "`r`n[/Script/AndroidRuntimeSettings.AndroidRuntimeSettings]`r`n" }
    return [regex]::Replace($Content, $sectionPattern, "`$1$line`r`n", 1)
}

try {
    $distributionArgs = @()
    if ($Configuration -eq "Shipping") {
        foreach ($name in @("ANDROID_KEYSTORE", "ANDROID_KEY_ALIAS", "ANDROID_KEYSTORE_PASSWORD", "ANDROID_KEY_PASSWORD")) {
            $value = (Get-Item "Env:$name" -ErrorAction SilentlyContinue).Value
            if ([string]::IsNullOrWhiteSpace($value)) { throw "Shipping requires $name from a secure environment." }
        }
        if (-not (Test-Path -LiteralPath $env:ANDROID_KEYSTORE -PathType Leaf)) { throw "ANDROID_KEYSTORE does not point to a file." }

        $buildAndroid = Join-Path $ProjectRoot 'Build\Android'
        New-Item -ItemType Directory -Force -Path $buildAndroid | Out-Null
        $keystoreName = [IO.Path]::GetFileName($env:ANDROID_KEYSTORE)
        $temporaryKeystore = Join-Path $buildAndroid $keystoreName
        Copy-Item -LiteralPath $env:ANDROID_KEYSTORE -Destination $temporaryKeystore -Force

        $signingIni = $engineIniBackup
        $signingIni = Set-AndroidSigningValue $signingIni 'KeyStore' $keystoreName
        $signingIni = Set-AndroidSigningValue $signingIni 'KeyAlias' $env:ANDROID_KEY_ALIAS
        $signingIni = Set-AndroidSigningValue $signingIni 'KeyStorePassword' $env:ANDROID_KEYSTORE_PASSWORD
        $signingIni = Set-AndroidSigningValue $signingIni 'KeyPassword' $env:ANDROID_KEY_PASSWORD
        Set-Content -LiteralPath $engineIni -Value $signingIni -Encoding UTF8
        $distributionArgs = @('-distribution')
    }

    # -package is mandatory here: stage/archive alone may complete without emitting an APK.
    $args = @(
        'BuildCookRun',
        "-project=$projectFile",
        '-nop4',
        '-unattended',
        '-utf8output',
        '-platform=Android',
        "-clientconfig=$Configuration",
        '-build',
        '-cook',
        '-stage',
        '-package',
        '-pak',
        '-compressed',
        '-archive',
        "-archivedirectory=$archiveRoot"
    ) + $distributionArgs

    & $uat @args
    if ($LASTEXITCODE -ne 0) { throw "Unreal Android $Configuration BuildCookRun failed with exit code $LASTEXITCODE." }
}
finally {
    Set-Content -LiteralPath $engineIni -Value $engineIniBackup -Encoding UTF8
    if ($temporaryKeystore -and (Test-Path -LiteralPath $temporaryKeystore -PathType Leaf)) {
        Remove-Item -LiteralPath $temporaryKeystore -Force
    }
}

$apkCandidates = @(Get-ChildItem -Path $archiveRoot -Recurse -Filter '*.apk' -File -ErrorAction SilentlyContinue)
if ($apkCandidates.Count -eq 0) { throw "Unreal completed without producing an APK under $archiveRoot." }
$apk = $apkCandidates | Sort-Object Length -Descending | Select-Object -First 1
$targetName = if ($Configuration -eq "Shipping") { "AshLine_CombatPrototype_v0.0.1_android_arm64.apk" } else { "AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk" }
$target = Join-Path $apkRoot $targetName
Copy-Item -LiteralPath $apk.FullName -Destination $target -Force
$hash = (Get-FileHash -Algorithm SHA256 -LiteralPath $target).Hash.ToLowerInvariant()
"$hash  $targetName" | Set-Content -LiteralPath (Join-Path $checksumsRoot "SHA256.txt") -Encoding ASCII

@"
# ASH LINE Combat Prototype Release Report

Configuration: $Configuration
APK: $targetName
APK path: $target
APK size bytes: $((Get-Item -LiteralPath $target).Length)
SHA-256: $hash
ABI target: arm64-v8a
Package target: com.ashline.game
Version target: 0.0.1 (1)

This report was generated only after Unreal BuildCookRun used -package and produced a real APK.
"@ | Set-Content -LiteralPath (Join-Path $reportsRoot "ReleaseReport.md") -Encoding UTF8

@"
# ASH LINE Combat Prototype Size Report

APK: $targetName
Bytes: $((Get-Item -LiteralPath $target).Length)
MiB: $([math]::Round((Get-Item -LiteralPath $target).Length / 1MB, 2))
Target: <= 500 MB
Preferred prototype target: <= 300 MB where practical
"@ | Set-Content -LiteralPath (Join-Path $reportsRoot "SizeReport.md") -Encoding UTF8

Write-Host "APK_READY=$target"
