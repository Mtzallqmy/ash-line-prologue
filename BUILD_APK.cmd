@echo off
setlocal
cd /d "%~dp0"
echo [ASH LINE] Building Development Android ARM64 APK...
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0Scripts\Build\BuildFirstAPK.ps1" -Configuration Development
if errorlevel 1 (
  echo.
  echo [ASH LINE] APK build FAILED. Review the error above and Saved\Logs.
  exit /b 1
)
echo.
echo [ASH LINE] APK build completed. Check Releases\Android\0.0.1\APK
endlocal
