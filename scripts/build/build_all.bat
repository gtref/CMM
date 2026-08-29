@echo off
setlocal

REM Load flags
call scripts\build\config.bat %1

REM Set local tool overrides if present
set "BISON_BIN=bison"
set "FLEX_BIN=flex"
set "ZIG_BIN=zig"

if exist tools\bison.exe set "BISON_BIN=tools\bison.exe"
if exist bin\bison.exe set "BISON_BIN=bin\bison.exe"

if exist tools\flex.exe set "FLEX_BIN=tools\flex.exe"
if exist bin\flex.exe set "FLEX_BIN=bin\flex.exe"

if exist tools\zig.exe set "ZIG_BIN=tools\zig.exe"
if exist bin\zig.exe set "ZIG_BIN=bin\zig.exe"

if "%~1"=="clean" (
    echo Cleaning build...
    if exist build\* del /q build\*
    if exist src\parser.tab.c del /q src\parser.tab.c
    if exist src\parser.tab.h del /q src\parser.tab.h
    if exist src\lexer.yy.c del /q src\lexer.yy.c
    exit /b 0
)

if "%~1"=="install" (
    echo Installing CMMC Compiler...
    if not exist build (
        echo Build directory does not exist. Please build first.
        exit /b 1
    )
    .\pathadd ..\..\build
    exit /b 0
)

echo === Building CMMC Compiler (%MODE% mode) ===

if not exist build (
    mkdir build
)

echo.
echo --- Copying TinyCC runtime files ---
if not exist build\libtcc.dll (
    copy lib\libtcc.dll build\libtcc.dll
    if errorlevel 1 exit /b 1
)

if exist lib\libtcc1-64.a (
    if not exist build\libtcc1-64.a (
        copy lib\libtcc1-64.a build\libtcc1-64.a
        if errorlevel 1 exit /b 1
    )
)

echo.
echo --- Copying args.dll ---
if not exist build\args.dll (
    copy lib\args.dll build\args.dll
    if errorlevel 1 exit /b 1
)

echo.
echo --- Running Bison ---
%BISON_BIN% -d -o src\parser.tab.c src\parser.y
if errorlevel 1 exit /b 1

echo.
echo --- Running Flex ---
%FLEX_BIN% -o src\lexer.yy.c src\lexer.l
if errorlevel 1 exit /b 1

echo.
echo --- Compiling with zig cc ---

if "%MODE%"=="native" (
    %ZIG_BIN% cc %CFLAGS% ^
        src\main.c ^
        src\ast.c ^
        src\codegen.c ^
        src\semantic.c ^
        src\parser.tab.c ^
        src\lexer.yy.c ^
        src\executable_formats\exec.c ^
        -o build\cmmc.exe
)

if "%MODE%"=="vm" (
    %ZIG_BIN% cc %CFLAGS% ^
        src\main.c ^
        src\ast.c ^
        src\vm.c ^
        src\parser.tab.c ^
        src\lexer.yy.c ^
        src\executable_formats\exec.c ^
        -o build\cmmc_vm.exe
)

if errorlevel 1 exit /b 1

echo.
echo Build complete.
endlocal
