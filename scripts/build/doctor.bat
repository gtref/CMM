@echo off
setlocal

echo Checking required build tools...
echo.

set "missing=0"

where bison.exe >nul 2>nul
if errorlevel 1 (
    echo [FAIL] bison.exe not found in PATH.
    set "missing=1"
) else (
    echo [OK] bison.exe found.
)

where flex.exe >nul 2>nul
if errorlevel 1 (
    echo [FAIL] flex.exe not found in PATH.
    set "missing=1"
) else (
    echo [OK] flex.exe found.
)

where zig.exe >nul 2>nul
if errorlevel 1 (
    where zig >nul 2>nul
    if errorlevel 1 (
        echo [FAIL] zig not found in PATH.
        set "missing=1"
    ) else (
        echo [OK] zig found.
    )
) else (
    echo [OK] zig found.
)

echo.
if "%missing%"=="1" (
    echo One or more required tools are missing.
    exit /b 1
)

echo All required tools were found.
exit /b 0
