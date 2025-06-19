@echo off
set PROGRAM_DIR=build/main.exe
echo Building file %PROGRAM_DIR%

g++ main.cpp -o %PROGRAM_DIR%

echo Build finished!
set /p should_run="Would you like to run the program? [Y/N]: "
if /I "%should_run%"=="Y" (
    echo Running program now!
    start %PROGRAM_DIR%
) else (
    echo okay fine then.
)