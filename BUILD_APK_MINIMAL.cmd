@echo off
setlocal
cd /d "%~dp0"
echo [ASH LINE] Detecting the minimal Android build toolchain and building Development ARM64...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\Build\BuildAndroidMinimal.ps1" -Configuration Development
if errorlevel 1 (
  echo [ASH LINE] Development APK build failed. Review Saved\Logs and Saved\BuildSetup\MinimalToolchain.json.
  exit /b 1
)
echo [ASH LINE] APK complete: Releases\Android\0.0.1\APK\AshLine_CombatPrototype_v0.0.1_android_arm64_development.apk
endlocal
