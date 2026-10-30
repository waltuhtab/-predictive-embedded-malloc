@echo off
echo --- Building Adaptive Memory Project ---

REM Create the 'bin' directory for released binaries if it doesn't exist
if not exist bin mkdir bin

REM Compile Experiment 1 (Fragmentation)
REM -I. tells the compiler to look in the root folder for adaptive_mem.h
echo Compiling Experiment 1 (Fragmentation)...
gcc -O3 -I. adaptive_mem.c exp\exp1_fragmentation.c -o bin\run_exp1.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp1.exe
) else (
    echo  -^> [!] Failed to build Experiment 1
)

REM Compile Experiment 2 (Zero-Copy)
echo Compiling Experiment 2 (Zero-Copy)...
gcc -O3 -I. adaptive_mem.c exp\exp2_zerocopy.c -o bin\run_exp2.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp2.exe
) else (
    echo  -^> [!] Failed to build Experiment 2
)

REM Compile Experiment 3 (Safety)
echo Compiling Experiment 3 (Safety)...
gcc -O3 -I. adaptive_mem.c exp\exp3_safety.c -o bin\run_exp3.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp3.exe
) else (
    echo  -^> [!] Failed to build Experiment 3
)

REM Compile Experiment 4 (Predictive)
echo Compiling Experiment 4 (Predictive)...
gcc -O3 -I. adaptive_mem.c exp\exp4_predictive.c -o bin\run_exp4.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp4.exe
) else (
    echo  -^> [!] Failed to build Experiment 4
)

REM Compile Experiment 5 (Dynamic Simulator)
echo Compiling Experiment 5 (Dynamic Simulator)...
gcc -O3 -I. adaptive_mem.c exp\exp5_dynamic.c -o bin\run_exp5.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp5.exe
) else (
    echo  -^> [!] Failed to build Experiment 5
)

REM Compile Experiment 6 (Real-World IoT)
echo Compiling Experiment 6 (Real World)...
gcc -O3 -I. adaptive_mem.c exp\exp6_realworld.c -o bin\run_exp6.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp6.exe
) else (
    echo  -^> [!] Failed to build Experiment 6
)

REM Compile Experiment 7 (Benchmark)
echo Compiling Experiment 7 (Benchmark)...
gcc -O3 -I. adaptive_mem.c exp\exp7_benchmark.c -o bin\run_exp7.exe
if %errorlevel% equ 0 (
    echo  -^> Success: bin\run_exp7.exe
) else (
    echo  -^> [!] Failed to build Experiment 7
)

echo --- Build Complete ---