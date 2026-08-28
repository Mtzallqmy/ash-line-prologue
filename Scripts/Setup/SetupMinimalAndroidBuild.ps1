param(
    [string]$ProjectRoot = (Resolve-Path (Join-Path $PSScriptRoot '../..')).Path,
    [string]$PreferredUnrealRoot = '',
    [string]$PreferredAndroidHome = '',
    [string]$PreferredAndroidNdkHome = '',
    [string]$PreferredJavaHome = '',
    [switch]$InstallAndroidPackages,
    [switch]$PersistUserEnvironment,
    [switch]$RequireAll
)

Set-StrictMode -Version Latest
$ErrorActionPreference = 'Stop'

$requiredNdk = '25.1.8937393'
$requiredSdk = 'android-34'
$requiredBuildTools = '34.0.0'
$result = [ordered]@{ Unreal = $null; MSVC = $null; Java = $null; AndroidSdk = $null; AndroidNdk = $null; Python = $null; Missing = @() }

function Add-UniqueCandidate([System.Collections.Generic.List[string]]$List, [string]$Candidate) {
    if (-not [string]::IsNullOrWhiteSpace($Candidate) -and -not $List.Contains($Candidate)) { [void]$List.Add($Candidate) }
}

function Find-UnrealRoot {
    $candidates = [System.Collections.Generic.List[string]]::new()
    Add-UniqueCandidate $candidates $PreferredUnrealRoot
    Add-UniqueCandidate $candidates $env:UE_ROOT
    Add-UniqueCandidate $candidates 'C:\Program Files\Epic Games\UE_5.4'
    if (Test-Path 'C:\Program Files\Epic Games') {
        Get-ChildItem 'C:\Program Files\Epic Games' -Directory -Filter 'UE_5.4*' -ErrorAction SilentlyContinue | ForEach-Object { Add-UniqueCandidate $candidates $_.FullName }
    }
    foreach ($candidate in $candidates) {
        $editor = Join-Path $candidate 'Engine\Binaries\Win64\UnrealEditor-Cmd.exe'
        $uat = Join-Path $candidate 'Engine\Build\BatchFiles\RunUAT.bat'
        if ((Test-Path $editor -PathType Leaf) -and (Test-Path $uat -PathType Leaf)) {
            $versionFile = Join-Path $candidate 'Engine\Build\Build.version'
            if (Test-Path $versionFile) {
                $version = Get-Content $versionFile -Raw | ConvertFrom-Json
                if ($version.MajorVersion -ne 5 -or $version.MinorVersion -ne 4 -or $version.PatchVersion -lt 4) { continue }
            }
            return $candidate
        }
    }
    return $null
}

function Find-MsvcBuildTools {
    $vswhere = 'C:\Program Files (x86)\Microsoft Visual Studio\Installer\vswhere.exe'
    if (-not (Test-Path $vswhere -PathType Leaf)) { return $null }
    $install = ((& $vswhere -latest -products * -requires Microsoft.VisualStudio.Component.VC.Tools.x86.x64 -property installationPath | Select-Object -First 1 | Out-String).Trim())
    if (-not [string]::IsNullOrWhiteSpace($install)) {
        $compiler = Get-ChildItem (Join-Path $install 'VC\Tools\MSVC') -Recurse -Filter 'cl.exe' -File -ErrorAction SilentlyContinue | Select-Object -First 1
        if ($compiler) { return $install }
    }
    $fallback = 'C:\Program Files (x86)\Microsoft Visual Studio\2022\BuildTools'
    $compiler = Get-ChildItem (Join-Path $fallback 'VC\Tools\MSVC') -Recurse -Filter 'cl.exe' -File -ErrorAction SilentlyContinue | Select-Object -First 1
    return $(if ($compiler) { $fallback } else { $null })
}

function Find-JavaHome {
    $candidates = [System.Collections.Generic.List[string]]::new()
    Add-UniqueCandidate $candidates $PreferredJavaHome
    Add-UniqueCandidate $candidates $env:JAVA_HOME
    Get-ChildItem 'C:\Program Files\Eclipse Adoptium' -Directory -ErrorAction SilentlyContinue | ForEach-Object { Add-UniqueCandidate $candidates $_.FullName }
    foreach ($candidate in $candidates) {
        $java = Join-Path $candidate 'bin\java.exe'
        if (-not (Test-Path $java -PathType Leaf)) { continue }
        $releaseFile = Join-Path $candidate 'release'
        $version = if (Test-Path $releaseFile -PathType Leaf) { (Get-Content -LiteralPath $releaseFile | Where-Object { $_ -match '^JAVA_VERSION=' } | Select-Object -First 1) } else { '' }
        if ($version -match 'JAVA_VERSION="17\.') { return $candidate }
    }
    return $null
}

function Find-AndroidHome {
    $candidates = [System.Collections.Generic.List[string]]::new()
    Add-UniqueCandidate $candidates $PreferredAndroidHome
    Add-UniqueCandidate $candidates $env:ANDROID_HOME
    Add-UniqueCandidate $candidates $env:ANDROID_SDK_ROOT
    Add-UniqueCandidate $candidates (Join-Path $env:LOCALAPPDATA 'Android\Sdk')
    foreach ($candidate in $candidates) {
        if (Test-Path $candidate -PathType Container) { return $candidate }
    }
    return (Join-Path $env:LOCALAPPDATA 'Android\Sdk')
}

function Find-SdkManager([string]$SdkHome) {
    if (-not (Test-Path $SdkHome -PathType Container)) { return $null }
    $latest = Join-Path $SdkHome 'cmdline-tools\latest\bin\sdkmanager.bat'
    if (Test-Path $latest -PathType Leaf) { return $latest }
    return Get-ChildItem (Join-Path $SdkHome 'cmdline-tools') -Recurse -Filter 'sdkmanager.bat' -File -ErrorAction SilentlyContinue | Select-Object -First 1 -ExpandProperty FullName
}

function Install-AndroidCommandLineTools([string]$SdkHome) {
    $existing = Find-SdkManager $SdkHome
    if ($existing) { return $existing }
    New-Item -ItemType Directory -Force -Path $SdkHome | Out-Null
    $repository = (Invoke-WebRequest -UseBasicParsing 'https://dl.google.com/android/repository/repository2-1.xml').Content
    $matches = [regex]::Matches($repository, 'commandlinetools-win-([0-9]+)_latest\.zip')
    if ($matches.Count -eq 0) { throw 'Unable to discover the official Android Command-line Tools download.' }
    $packageName = ($matches | ForEach-Object { $_.Value } | Sort-Object -Unique | Sort-Object { [int]([regex]::Match($_, '[0-9]+').Value) } -Descending | Select-Object -First 1)
    $archive = Join-Path $env:TEMP $packageName
    $extractRoot = Join-Path $env:TEMP ('ash-line-android-cli-' + [guid]::NewGuid().ToString('N'))
    try {
        Invoke-WebRequest -UseBasicParsing ("https://dl.google.com/android/repository/$packageName") -OutFile $archive
        Expand-Archive -LiteralPath $archive -DestinationPath $extractRoot -Force
        $source = Join-Path $extractRoot 'cmdline-tools'
        $destination = Join-Path $SdkHome 'cmdline-tools\latest'
        if (-not (Test-Path $source -PathType Container)) { throw 'The Android Command-line Tools archive has an unexpected structure.' }
        if (Test-Path $destination) { Remove-Item -LiteralPath $destination -Recurse -Force }
        New-Item -ItemType Directory -Force -Path (Split-Path $destination) | Out-Null
        Move-Item -LiteralPath $source -Destination $destination
    }
    finally {
        if (Test-Path $extractRoot) { Remove-Item -LiteralPath $extractRoot -Recurse -Force }
        if (Test-Path $archive) { Remove-Item -LiteralPath $archive -Force }
    }
    return Find-SdkManager $SdkHome
}

function Set-ToolEnvironment([string]$Name, [string]$Value) {
    if ([string]::IsNullOrWhiteSpace($Value)) { return }
    Set-Item -Path "Env:$Name" -Value $Value
    if ($PersistUserEnvironment) { [Environment]::SetEnvironmentVariable($Name, $Value, 'User') }
}

$result.Unreal = Find-UnrealRoot
$result.MSVC = Find-MsvcBuildTools
$result.Java = Find-JavaHome
$result.AndroidSdk = Find-AndroidHome
$sdkManager = Find-SdkManager $result.AndroidSdk
$ndkCandidate = if ($PreferredAndroidNdkHome) { $PreferredAndroidNdkHome } else { Join-Path $result.AndroidSdk "ndk\$requiredNdk" }
$result.AndroidNdk = if (Test-Path (Join-Path $ndkCandidate 'source.properties') -PathType Leaf) { $ndkCandidate } else { $null }

if ($InstallAndroidPackages) {
    if (-not $sdkManager) { $sdkManager = Install-AndroidCommandLineTools $result.AndroidSdk }
    if (-not $sdkManager) { throw 'Android SDK Command-line Tools installation did not provide sdkmanager.bat.' }
    (1..200 | ForEach-Object { 'y' }) | & $sdkManager --sdk_root=$result.AndroidSdk --licenses | Out-Null
    & $sdkManager --sdk_root=$result.AndroidSdk 'platform-tools' "platforms;$requiredSdk" "build-tools;$requiredBuildTools" "ndk;$requiredNdk"
    if ($LASTEXITCODE -ne 0) { throw 'Android SDK package installation failed.' }
    $sdkManager = Find-SdkManager $result.AndroidSdk
    $result.AndroidNdk = Join-Path $result.AndroidSdk "ndk\$requiredNdk"
}

if (-not $result.Unreal) { $result.Missing += 'Unreal Engine 5.4.4+ (Windows installation from Epic Games Launcher)' }
if (-not $result.MSVC) { $result.Missing += 'Visual Studio 2022 Build Tools with Microsoft.VisualStudio.Workload.VCTools' }
if (-not $result.Java) { $result.Missing += 'JDK 17' }
if (-not $sdkManager) { $result.Missing += 'Android SDK Command-line Tools' }
if (-not (Test-Path (Join-Path $result.AndroidSdk "platforms\$requiredSdk") -PathType Container)) { $result.Missing += "Android platform $requiredSdk" }
if (-not (Test-Path (Join-Path $result.AndroidSdk "build-tools\$requiredBuildTools") -PathType Container)) { $result.Missing += "Android Build Tools $requiredBuildTools" }
if (-not (Test-Path (Join-Path $result.AndroidSdk 'platform-tools\adb.exe') -PathType Leaf)) { $result.Missing += 'Android Platform Tools' }
if (-not $result.AndroidNdk) { $result.Missing += "Android NDK $requiredNdk" }

if ($result.Unreal) {
    $python = Join-Path $result.Unreal 'Engine\Binaries\ThirdParty\Python3\Win64\python.exe'
    if (Test-Path $python -PathType Leaf) { $result.Python = $python }
}
if (-not $result.Python) {
    $pythonCommand = Get-Command python.exe -ErrorAction SilentlyContinue
    if ($pythonCommand) { $result.Python = $pythonCommand.Source }
}
if (-not $result.Python) { $result.Missing += 'Python (Unreal embedded Python is sufficient after Unreal is installed)' }

Set-ToolEnvironment 'UE_ROOT' $result.Unreal
Set-ToolEnvironment 'JAVA_HOME' $result.Java
Set-ToolEnvironment 'ANDROID_HOME' $result.AndroidSdk
Set-ToolEnvironment 'ANDROID_SDK_ROOT' $result.AndroidSdk
Set-ToolEnvironment 'ANDROID_NDK_HOME' $result.AndroidNdk
Set-ToolEnvironment 'NDK_HOME' $result.AndroidNdk
Set-ToolEnvironment 'PYTHON_EXE' $result.Python

$reportPath = Join-Path $ProjectRoot 'Saved\BuildSetup\MinimalToolchain.json'
New-Item -ItemType Directory -Force -Path (Split-Path $reportPath) | Out-Null
$result | ConvertTo-Json -Depth 3 | Set-Content -LiteralPath $reportPath -Encoding UTF8
$result | ConvertTo-Json -Depth 3
if ($RequireAll -and $result.Missing.Count -gt 0) { throw ('Missing minimal toolchain components: ' + ($result.Missing -join '; ')) }
