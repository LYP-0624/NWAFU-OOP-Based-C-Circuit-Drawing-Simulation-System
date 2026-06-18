@echo off
setlocal
set "SCRIPT_DIR=%~dp0"
set "EXE=%SCRIPT_DIR%build_deploycheck\CircuitSim.exe"

if not exist "%EXE%" (
    echo Cannot find "%EXE%".
    echo Please build the project first.
    pause
    exit /b 1
)

start "" "%EXE%"
endlocal
