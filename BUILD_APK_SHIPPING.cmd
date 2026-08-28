@echo off
setlocal
cd /d "%~dp0"
echo [ASH LINE] Building Shipping Android ARM64 APK...
echo [ASH LINE] ANDROID_KEYSTORE, ANDROID_KEY_ALIAS, ANDROID_KEYSTORE_PASSWORD and ANDROID_KEY_PASSWORD must already be set.
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\Build\BuildAndroidMinimal.ps1" -Configuration Shipping
if errorlevel 1 (
  echo.
  echo [ASH LINE] Shipping APK build FAILED. Review the error above and Saved\Logs.
  exit /b 1
)
echo.
echo [ASH LINE] Shipping APK build completed. Check Releases\Android\0.0.1\APK
endlocal
