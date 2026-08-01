@echo off
setlocal

REM Root dispatcher: handles flags and passes them to scripts

if "%~1"=="help" (
    call scripts\build\internal_help.bat
    exit /b 0
)

if "%~1"=="version" (
    call scripts\build\internal_version.bat
    exit /b 0
)

REM Everything else goes to the real build system
call scripts\build\build_all.bat %*

endlocal
