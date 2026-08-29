@echo off
setlocal

echo Checking required build tools...
echo.

set "missing=0"

if exist tools\bison.exe (
    echo [OK] bison found locally at tools\bison.exe.
) else if exist bin\bison.exe (
    echo [OK] bison found locally at bin\bison.exe.
) else (
    where bison.exe >nul 2>nul
    if errorlevel 1 (
        echo [FAIL] bison.exe not found locally or in PATH.
        set "missing=1"
    ) else (
        echo [OK] bison.exe found in PATH.
    )
)

if exist tools\flex.exe (
    echo [OK] flex found locally at tools\flex.exe.
) else if exist bin\flex.exe (
    echo [OK] flex found locally at bin\flex.exe.
) else (
    where flex.exe >nul 2>nul
    if errorlevel 1 (
        echo [FAIL] flex.exe not found locally or in PATH.
        set "missing=1"
    ) else (
        echo [OK] flex.exe found in PATH.
    )
)

if exist tools\zig.exe (
    echo [OK] zig found locally at tools\zig.exe.
) else if exist bin\zig.exe (
    echo [OK] zig found locally at bin\zig.exe.
) else (
    where zig.exe >nul 2>nul
    if errorlevel 1 (
        where zig >nul 2>nul
        if errorlevel 1 (
            echo [FAIL] zig not found locally or in PATH.
            set "missing=1"
        ) else (
            echo [OK] zig found in PATH.
        )
    ) else (
        echo [OK] zig found in PATH.
    )
)

echo.
if "%missing%"=="1" (
    echo One or more required tools are missing.
    exit /b 1
)

echo All required tools were found.
exit /b 0
