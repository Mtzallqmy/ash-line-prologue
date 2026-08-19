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
$RunnerName = if ([string]::IsNullOrWhiteSpace($RunnerName)) { "ashline-unreal-$env:COMPUTERNAME" } else { $RunnerName }

function Write-Step([string]$Message) {
    Write-Host "[ASH LINE] $Message" -ForegroundColor Cyan
}

function Fail([string]$Message) {
    throw "[ASH LINE] $Message"
}

function Assert-Administrator {
    $identity = [Security.Principal.WindowsIdentity]::GetCurrent()
    $principal = New-Object Security.Principal.WindowsPrincipal($identity)
    if (-not $principal.IsInRole([Security.Principal.WindowsBuiltInRole]::Administrator)) {
        Fail 'شغّل PowerShell بصفة Administrator ثم أعد التنفيذ.'
    }
}

function Resolve-RequiredPath([string]$Candidate, [string]$Name) {
    if ([string]::IsNullOrWhiteSpace($Candidate)) {
        Fail "المتغير $Name غير محدد. مرره للسكربت أو عرّفه في Environment Variables."
    }
    $resolved = [Environment]::ExpandEnvironmentVariables($Candidate)
    if (-not (Test-Path -LiteralPath $resolved -PathType Container)) {
        Fail "المسار $Name غير موجود: $resolved"
    }
    return (Resolve-Path -LiteralPath $resolved).Path
}

function Assert-File([string]$Path, [string]$Description) {
    if (-not (Test-Path -LiteralPath $Path -PathType Leaf)) {
        Fail "لم يتم العثور على $Description في: $Path"
    }
}

function Set-EnvironmentValue([string]$Name, [string]$Value) {
    [Environment]::SetEnvironmentVariable($Name, $Value, 'Machine')
    Set-Item -Path "Env:$Name" -Value $Value
    Write-Step "تم ضبط Machine Environment Variable: $Name=$Value"
}

Assert-Administrator

if ([string]::IsNullOrWhiteSpace($UnrealRoot)) { $UnrealRoot = $env:UE_ROOT }
if ([string]::IsNullOrWhiteSpace($AndroidHome)) { $AndroidHome = $env:ANDROID_HOME }
if ([string]::IsNullOrWhiteSpace($AndroidNdkHome)) { $AndroidNdkHome = $env:ANDROID_NDK_HOME }
if ([string]::IsNullOrWhiteSpace($JavaHome)) { $JavaHome = $env:JAVA_HOME }

Write-Step 'التحقق من Unreal Engine 5.4 وAndroid SDK/NDK وJava.'
$UnrealRoot = Resolve-RequiredPath $UnrealRoot 'UE_ROOT'
$AndroidHome = Resolve-RequiredPath $AndroidHome 'ANDROID_HOME'
$AndroidNdkHome = Resolve-RequiredPath $AndroidNdkHome 'ANDROID_NDK_HOME'
$JavaHome = Resolve-RequiredPath $JavaHome 'JAVA_HOME'

$ubtCandidates = @(
    (Join-Path $UnrealRoot 'Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe'),
    (Join-Path $UnrealRoot 'Engine\Binaries\DotNET\UnrealBuildTool.exe')
)
if (-not ($ubtCandidates | Where-Object { Test-Path -LiteralPath $_ -PathType Leaf })) {
    Fail "لم يتم العثور على UnrealBuildTool.exe داخل UE_ROOT: $UnrealRoot"
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

Write-Step 'التحقق من إصدارات الأدوات.'
& (Join-Path $JavaHome 'bin\java.exe') -version 2>&1 | Select-Object -First 1 | Write-Host
& (Join-Path $AndroidHome 'platform-tools\adb.exe') version 2>&1 | Select-Object -First 1 | Write-Host

if (-not (Test-Path -LiteralPath $RunnerRoot -PathType Container)) {
    New-Item -ItemType Directory -Path $RunnerRoot -Force | Out-Null
}
$RunnerRoot = (Resolve-Path -LiteralPath $RunnerRoot).Path
Set-Location -LiteralPath $RunnerRoot

if (Test-Path -LiteralPath (Join-Path $RunnerRoot '.runner')) {
    Fail "هذا المجلد يحتوي Runner مسجلاً مسبقًا: $RunnerRoot. استخدم مجلدًا جديدًا أو أزل التسجيل بالطريقة الرسمية أولًا."
}

Write-Step 'اكتشاف أحدث إصدار رسمي من GitHub Actions Runner.'
$release = Invoke-RestMethod -Uri 'https://api.github.com/repos/actions/runner/releases/latest' -Headers @{
    Accept = 'application/vnd.github+json'
    'User-Agent' = 'ASH-LINE-Runner-Setup'
}
$asset = $release.assets | Where-Object { $_.name -match '^actions-runner-win-x64-.*\.zip$' } | Select-Object -First 1
if ($null -eq $asset) { Fail 'تعذر العثور على Windows x64 Runner package في أحدث إصدار.' }

$zipPath = Join-Path $RunnerRoot $asset.name
Write-Step "تنزيل $($asset.name)."
Invoke-WebRequest -Uri $asset.browser_download_url -OutFile $zipPath -UseBasicParsing
if ((Get-Item -LiteralPath $zipPath).Length -lt 1MB) { Fail 'ملف Runner الذي تم تنزيله غير صالح أو ناقص.' }

Write-Step 'فك ضغط Runner.'
Add-Type -AssemblyName System.IO.Compression.FileSystem
[System.IO.Compression.ZipFile]::ExtractToDirectory($zipPath, $RunnerRoot)
Remove-Item -LiteralPath $zipPath -Force
Assert-File (Join-Path $RunnerRoot 'config.cmd') 'config.cmd'
Assert-File (Join-Path $RunnerRoot 'run.cmd') 'run.cmd'

$registrationToken = $env:GITHUB_RUNNER_REGISTRATION_TOKEN
$secureToken = $null
if ([string]::IsNullOrWhiteSpace($registrationToken)) {
    $secureToken = Read-Host 'أدخل Registration Token المؤقت من GitHub' -AsSecureString
    $registrationToken = [Runtime.InteropServices.Marshal]::PtrToStringBSTR(
        [Runtime.InteropServices.Marshal]::SecureStringToBSTR($secureToken)
    )
}
if ([string]::IsNullOrWhiteSpace($registrationToken)) { Fail 'Registration Token فارغ.' }

$labels = $RequiredLabels -join ','
Write-Step "تسجيل Runner باسم $RunnerName وبLabels: $labels"
& (Join-Path $RunnerRoot 'config.cmd') --unattended --url $RepositoryUrl --token $registrationToken --name $RunnerName --labels $labels --work '_work' --replace
if ($LASTEXITCODE -ne 0) { Fail "فشل تسجيل Runner، exit code=$LASTEXITCODE" }

$registrationToken = $null
if ($secureToken) { $secureToken.Dispose() }
Remove-Item Env:GITHUB_RUNNER_REGISTRATION_TOKEN -ErrorAction SilentlyContinue

if ($InstallAsService) {
    Write-Step 'تثبيت Runner كخدمة Windows.'
    & (Join-Path $RunnerRoot 'svc.cmd') install
    if ($LASTEXITCODE -ne 0) { Fail "فشل تثبيت Windows Service، exit code=$LASTEXITCODE" }
    & (Join-Path $RunnerRoot 'svc.cmd') start
    if ($LASTEXITCODE -ne 0) { Fail "فشل تشغيل Windows Service، exit code=$LASTEXITCODE" }
    Write-Step 'تم تشغيل Runner كخدمة. لا تغلق الجهاز أو توقف الخدمة أثناء البناء.'
} else {
    Write-Step 'تم التسجيل بنجاح. شغّل run.cmd في هذه النافذة لإبقاء Runner متصلًا.'
    Write-Host "cd `"$RunnerRoot`"; .\run.cmd" -ForegroundColor Yellow
}

Write-Step 'اكتمل إعداد Runner. تحقق من ظهوره في GitHub قبل انتظار الـWorkflow.'
