@echo off

set MODE=native
set CFLAGS=

if "%~1"=="debug" (
    set MODE=native
    set CFLAGS=-g -O0
)

if "%~1"=="release" (
    set MODE=native
    set CFLAGS=-O3
)

if "%~1"=="vm" (
    set MODE=vm
    set CFLAGS=-O2
)

if "%~1"=="native" (
    set MODE=native
    set CFLAGS=-O2
)
