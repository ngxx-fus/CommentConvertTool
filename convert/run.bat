@echo off
if exist convert.exe del convert.exe

echo Compiling...
g++ -std=c++17 .\convert\convert.cpp -o convert.exe

if %errorlevel% neq 0 (
    echo [ERROR] Compilation failed!
    pause
    exit /b %errorlevel%
)

echo Running...
.\convert.exe
echo.

@REM pause