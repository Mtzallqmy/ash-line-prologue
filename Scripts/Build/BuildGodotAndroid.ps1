[CmdletBinding()]
param(
    [ValidateSet('Development', 'Release')]
    [string]$Configuration = 'Development',
    [string]$GodotExecutable = '',
    [string]$ExpectedPackage = 'com.ashline.game',
    [int]$ExpectedMinSdk = 26
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$repositoryRoot = (Resolve-Path (Join-Path $PSScriptRoot '..\..')).Path
$projectRoot = Join-Path $repositoryRoot 'Godot'
$defaultGodot = Join-Path $env:USERPROFILE 'Tools\Godot\4.7.2\Godot_v4.7.2-stable_win64_console.exe'
$godot = if ($GodotExecutable) { $GodotExecutable } elseif ($env:GODOT_EXE) { $env:GODOT_EXE } else { $defaultGodot }
$sdkRoot = Join-Path $env:LOCALAPPDATA 'Android\Sdk'
$buildToolsRoot = Join-Path $sdkRoot 'build-tools'
$aapt = Get-ChildItem -LiteralPath $buildToolsRoot -Recurse -File -Filter 'aapt.exe' | Sort-Object FullName -Descending | Select-Object -First 1 -ExpandProperty FullName
$outputDir = Join-Path $projectRoot 'Releases\Android\0.0.1\APK'
$statusFile = Join-Path $projectRoot 'BuildStatus.json'
$baseName = 'ASH_LINE_Prologue_Android_ARM64'
$suffix = if ($Configuration -eq 'Development') { '.debug.apk' } else { '.release.apk' }
$apkPath = Join-Path $outputDir ($baseName + $suffix)
$hashPath = $apkPath + '.sha256'

function Set-BuildStatus {
    param([string]$Phase, [int]$Percent, [string]$Message)
    [pscustomobject]@{
        phase = $Phase
        percent = $Percent
        message = $Message
        configuration = $Configuration
        updatedUtc = (Get-Date).ToUniversalTime().ToString('o')
    } | ConvertTo-Json | Set-Content -LiteralPath $statusFile -Encoding utf8
}

try {
    Set-BuildStatus -Phase 'validating' -Percent 5 -Message 'Checking Godot and Android build prerequisites.'
    if (-not (Test-Path -LiteralPath $godot -PathType Leaf)) { throw "Godot executable not found: $godot" }
    if (-not (Test-Path -LiteralPath $aapt -PathType Leaf)) { throw "Android aapt.exe not found under: $buildToolsRoot" }
    $requiredFiles = @(
        (Join-Path $projectRoot 'project.godot'),
        (Join-Path $projectRoot 'export_presets.cfg'),
        (Join-Path $projectRoot 'scenes\MainMenu.tscn'),
        (Join-Path $projectRoot 'scenes\CombatArena.tscn'),
        (Join-Path $projectRoot 'scenes\Player.tscn'),
        (Join-Path $projectRoot 'scenes\Enemy.tscn'),
        (Join-Path $projectRoot 'scripts\core\GameSession.gd'),
        (Join-Path $projectRoot 'scripts\platform\CombatArena.gd'),
        (Join-Path $projectRoot 'tests\vertical_slice_smoke.gd')
    )
    $missingFiles = $requiredFiles | Where-Object { -not (Test-Path -LiteralPath $_ -PathType Leaf) }
    if ($missingFiles) { throw "Godot launch-critical files are missing: $($missingFiles -join '; ')" }

    Set-BuildStatus -Phase 'smoke-test' -Percent 18 -Message 'Running the Godot vertical-slice smoke test (no phone or adb required).'
    & $godot --headless --language en --audio-driver Dummy --path $projectRoot --script 'res://tests/vertical_slice_smoke.gd'
    if ($LASTEXITCODE -ne 0) { throw 'Godot vertical-slice smoke test failed.' }

    New-Item -ItemType Directory -Force -Path $outputDir | Out-Null
    Remove-Item -LiteralPath $apkPath,$hashPath -Force -ErrorAction SilentlyContinue
    Set-BuildStatus -Phase 'exporting' -Percent 35 -Message "Exporting Godot $Configuration APK for arm64-v8a."
    $exportArgument = if ($Configuration -eq 'Development') { '--export-debug' } else { '--export-release' }
    & $godot --headless --language en --quit --path $projectRoot $exportArgument 'Android ARM64' $apkPath
    if ($LASTEXITCODE -ne 0) { throw "Godot $Configuration export failed." }
    if (-not (Test-Path -LiteralPath $apkPath -PathType Leaf) -or (Get-Item -LiteralPath $apkPath).Length -lt 1MB) { throw "APK was not produced or is too small: $apkPath" }

    Set-BuildStatus -Phase 'validating-apk' -Percent 82 -Message 'Inspecting package metadata, arm64 ABI, minimum SDK, and signature.'
    $badging = & $aapt dump badging $apkPath
    if ($LASTEXITCODE -ne 0) { throw 'aapt could not inspect the generated APK.' }
    $badgingText = $badging -join "`n"
    if ($badgingText -notmatch ("package: name='" + [regex]::Escape($ExpectedPackage) + "'")) { throw "Unexpected package name. Expected $ExpectedPackage." }
    if ($badgingText -notmatch "native-code:.*'arm64-v8a'") { throw 'Generated APK does not contain arm64-v8a native code.' }
    if ($badgingText -notmatch ("sdkVersion:'" + $ExpectedMinSdk + "'")) { throw "Generated APK does not use minSdk $ExpectedMinSdk." }
    $apksigner = Join-Path (Split-Path $aapt -Parent) 'apksigner.bat'
    & $apksigner verify --verbose $apkPath
    if ($LASTEXITCODE -ne 0) { throw 'APK signature verification failed.' }

    $sha256 = (Get-FileHash -LiteralPath $apkPath -Algorithm SHA256).Hash.ToLowerInvariant()
    "$sha256 *$(Split-Path $apkPath -Leaf)" | Set-Content -LiteralPath $hashPath -NoNewline -Encoding ascii
    Set-BuildStatus -Phase 'complete' -Percent 100 -Message "APK validated: $(Split-Path $apkPath -Leaf)"
    Write-Host "APK_READY=$apkPath"
    Write-Host "APK_SHA256=$sha256"
    Write-Host "APK_CHECKSUM_FILE=$hashPath"
} catch {
    Set-BuildStatus -Phase 'failed' -Percent 100 -Message $_.Exception.Message
    throw
}
