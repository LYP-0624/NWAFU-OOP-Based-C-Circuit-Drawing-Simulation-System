@echo off
setlocal

set QT_DIR=D:\Qt\6.11.1\mingw_64
set MINGW_DIR=D:\Qt\Tools\mingw1310_64
set BUILD_DIR=build_mingw1310

if not exist "%BUILD_DIR%" mkdir "%BUILD_DIR%"

cmake -S . -B "%BUILD_DIR%" -G "MinGW Makefiles" ^
    -DCMAKE_PREFIX_PATH=%QT_DIR% ^
    -DCMAKE_C_COMPILER=%MINGW_DIR%\bin\gcc.exe ^
    -DCMAKE_CXX_COMPILER=%MINGW_DIR%\bin\g++.exe
if errorlevel 1 exit /b 1

cmake --build "%BUILD_DIR%" -j 4
if errorlevel 1 exit /b 1

echo Build complete: %BUILD_DIR%\CircuitSim.exe
endlocal
