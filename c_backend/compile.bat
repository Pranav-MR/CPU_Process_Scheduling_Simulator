@echo off
echo Compiling scheduler.c...
gcc -Wall -O2 scheduler.c -o scheduler.exe
if %ERRORLEVEL% EQU 0 (
    echo Compilation successful! Created scheduler.exe
) else (
    echo Compilation failed!
)
