[CmdletBinding()]
param(
    [string]$RepositoryUrl = 'https://github.com/Mtzallqmy/ash-line-prologue',
    [string]$RunnerRoot = 'C:\actions-runner',
    [string]$RunnerName = '',
    [string]$UnrealRoot = '',
    [string]$AndroidHome = '',
    [string]$AndroidNdkHome = '',
    [string]$JavaHome = '',
    [switch]$PersistEnvironment,
    [switch]$InstallAsService
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$RequiredLabels = @('self-hosted', 'Windows', 'X64', 'unreal-5.4', 'android')
if ([string]::IsNullOrWhiteSpace($RunnerName)) {
    $RunnerName = "ashline-unreal-$env:COMPUTERNAME"
}

function Write-Step([string]$Message) {
    Write-Host "[ASH LINE] $Message" -ForegroundColor Cyan
}

function Fail([string]$Message) {
    throw "[ASH LINE] $Message"
}

function Assert-Administrator {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    $adminRole = [Security.Principal.WindowsBuiltInRole]::Administrator
    if (-not $principal.IsInRole($adminRole)) {
        Fail 'Run PowerShell or VS Code as Administrator, then try again.'
    }
}

function Resolve-RequiredPath([string]$Candidate, [string]$Name) {
    if ([string]::IsNullOrWhiteSpace($Candidate)) {
        Fail "$Name is empty. Pass it to the script or define the environment variable."
    }
    $expanded = [Environment]::ExpandEnvironmentVariables($Candidate)
    if (-not (Test-Path -LiteralPath $expanded -PathType Container)) {
        Fail "$Name was not found: $expanded"
    }
    return (Resolve-Path -LiteralPath $expanded).Path
}

function Assert-File([string]$Path, [string]$Description) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        Fail "$Description was not found: $Path"
    }
}

function Set-EnvironmentValue([string]$Name, [string]$Value) {
    [Environment]::SetEnvironmentVariable($Name, $Value, 'Machine')
    Set-Item -Path "Env:$Name" -Value $Value
    Write-Step "Set machine environment variable: $Name"
}

Assert-Administrator

if ([string]::IsNullOrWhiteSpace($UnrealRoot)) {
    $UnrealRoot = $env:UE_ROOT
}
if ([string]::IsNullOrWhiteSpace($AndroidHome)) {
    $AndroidHome = $env:ANDROID_HOME
}
if ([string]::IsNullOrWhiteSpace($AndroidNdkHome)) {
    $AndroidNdkHome = $env:ANDROID_NDK_HOME
}
if ([string]::IsNullOrWhiteSpace($JavaHome)) {
    $JavaHome = $env:JAVA_HOME
}

Write-Step 'Checking Unreal Engine, Android SDK/NDK, and Java.'
$UnrealRoot = Resolve-RequiredPath $UnrealRoot 'UE_ROOT'
$AndroidHome = Resolve-RequiredPath $AndroidHome 'ANDROID_HOME'
$AndroidNdkHome = Resolve-RequiredPath $AndroidNdkHome 'ANDROID_NDK_HOME'
$JavaHome = Resolve-RequiredPath $JavaHome 'JAVA_HOME'

$ubtCandidates = @(
    (Join-Path $UnrealRoot 'Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe'),
    (Join-Path $UnrealRoot 'Engine\Binaries\DotNET\UnrealBuildTool.exe')
)
$ubtFound = $ubtCandidates | Where-Object {
    Test-Path -LiteralPath $_ -PathType Leaf
}
if (-not $ubtFound) {
    Fail "UnrealBuildTool.exe was not found below UE_ROOT: $UnrealRoot"
}

Assert-File (Join-Path $UnrealRoot 'Engine\Build\BatchFiles\RunUAT.bat') 'RunUAT.bat'
Assert-File (Join-Path $UnrealRoot 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe') 'UnrealEditor-Cmd.exe'
Assert-File (Join-Path $AndroidHome 'platform-tools\adb.exe') 'adb.exe'
Assert-File (Join-Path $AndroidHome 'platforms\android-34\android.jar') 'Android API 34'
Assert-File (Join-Path $AndroidHome 'build-tools\34.0.0\aapt2.exe') 'Android Build Tools 34.0.0'
Assert-File (Join-Path $AndroidNdkHome 'source.properties') 'Android NDK source.properties'
Assert-File (Join-Path $JavaHome 'bin\java.exe') 'Java executable'

if ($PersistEnvironment) {
    Set-EnvironmentValue 'UE_ROOT' $UnrealRoot
    Set-EnvironmentValue 'ANDROID_HOME' $AndroidHome
    Set-EnvironmentValue 'ANDROID_SDK_ROOT' $AndroidHome
    Set-EnvironmentValue 'ANDROID_NDK_HOME' $AndroidNdkHome
    Set-EnvironmentValue 'JAVA_HOME' $JavaHome
}

Write-Step 'Checking tool versions.'
& (Join-Path $JavaHome 'bin\java.exe') -version 2>&1 | Select-Object -First 1 | Write-Host
& (Join-Path $AndroidHome 'platform-tools\adb.exe') version 2>&1 | Select-Object -First 1 | Write-Host

if (-not (Test-Path -LiteralPath $RunnerRoot -PathType Container)) {
    New-Item -ItemType Directory -Path $RunnerRoot -Force | Out-Null
}
$RunnerRoot = (Resolve-Path -LiteralPath $RunnerRoot).Path
Set-Location -LiteralPath $RunnerRoot

if (Test-Path -LiteralPath (Join-Path $RunnerRoot '.runner')) {
    Fail "This folder already contains a registered Runner: $RunnerRoot"
}

Write-Step 'Finding the latest official GitHub Actions Runner.'
$release = Invoke-RestMethod -Uri 'https://api.github.com/repos/actions/runner/releases/latest' -Headers @{
    Accept = 'application/vnd.github+json'
    'User-Agent' = 'ASH-LINE-Runner-Setup'
}
$asset = $release.assets | Where-Object {
    $_.name -match '^actions-runner-win-x64-.*\.zip$'
} | Select-Object -First 1
if ($null -eq $asset) {
    Fail 'Could not find the Windows x64 Runner package.'
}

$zipPath = Join-Path $RunnerRoot $asset.name
Write-Step "Downloading $($asset.name)."
Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $zipPath -UseBasicParsing
if ((Get-Item -LiteralPath $zipPath).Length -lt 1MB) {
    Fail 'The downloaded Runner archive is missing or invalid.'
}

Write-Step 'Extracting the Runner.'
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::ExtractToDirectory($zipPath, $RunnerRoot)
Remove-Item -LiteralPath $zipPath -Force
Assert-File (Join-Path $RunnerRoot 'config.cmd') 'config.cmd'
Assert-File (Join-Path $RunnerRoot 'run.cmd') 'run.cmd'

$registrationToken = $env:GITHUB_RUNNER_REGISTRATION_TOKEN
$secureToken = $null
if ([string]::IsNullOrWhiteSpace($registrationToken)) {
    $secureToken = Read-Host 'Enter the temporary GitHub Registration Token' -AsSecureString
    $tokenPtr = [Runtime.InteropServices.Marshal]::SecureStringToBSTR($secureToken)
    try {
        $registrationToken = [Runtime.InteropServices.Marshal]::PtrToStringBSTR($tokenPtr)
    }
    finally {
        [Runtime.InteropServices.Marshal]::ZeroFreeBSTR($tokenPtr)
    }
}
if ([string]::IsNullOrWhiteSpace($registrationToken)) {
    Fail 'Registration Token is empty.'
}

$labels = $RequiredLabels -join ','
Write-Step "Registering Runner $RunnerName with labels: $labels"
& (Join-Path $RunnerRoot 'config.cmd') --unattended --url $RepositoryUrl --token $registrationToken --name $RunnerName --labels $labels --work '_work' --replace
if ($LASTEXITCODE -ne 0) {
    Fail "Runner registration failed. Exit code: $LASTEXITCODE"
}

$registrationToken = $null
if ($null -ne $secureToken) {
    $secureToken.Dispose()
}
Remove-Item Env:GITHUB_RUNNER_REGISTRATION_TOKEN -ErrorAction SilentlyContinue

if ($InstallAsService) {
    Write-Step 'Installing the Runner Windows service.'
    & (Join-Path $RunnerRoot 'svc.cmd') install
    if ($LASTEXITCODE -ne 0) {
        Fail "Windows service installation failed. Exit code: $LASTEXITCODE"
    }
    & (Join-Path $RunnerRoot 'svc.cmd') start
    if ($LASTEXITCODE -ne 0) {
        Fail "Windows service start failed. Exit code: $LASTEXITCODE"
    }
    Write-Step 'Runner service is running.'
}
else {
    Write-Step 'Runner registered. Keep this terminal open and run run.cmd.'
    Write-Host "Set-Location '$RunnerRoot'; .\run.cmd" -ForegroundColor Yellow
}

Write-Step 'Runner setup completed. Verify it is online in GitHub Actions.'
