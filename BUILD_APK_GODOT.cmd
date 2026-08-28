@echo off
setlocal
set "ROOT=%~dp0"
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%ROOT%Scripts\Build\BuildGodotAndroid.ps1" -Configuration Development
set "CODE=%ERRORLEVEL%"
exit /b %CODE%
