@echo off
setlocal

REM Load flags
call scripts\build\config.bat %1

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
    pathadd C:\Users\ronan\L1_DIGITAL_TECH\C\work\better_C--\build
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
bison -d -o src\parser.tab.c src\parser.y
if errorlevel 1 exit /b 1

echo.
echo --- Running Flex ---
flex -o src\lexer.yy.c src\lexer.l
if errorlevel 1 exit /b 1

echo.
echo --- Compiling with zig cc ---

if "%MODE%"=="native" (
    zig cc %CFLAGS% ^
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
    zig cc %CFLAGS% ^
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
