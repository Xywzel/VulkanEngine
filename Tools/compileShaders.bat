@echo off

Rem for /f %%f in ('dir /b /a:-d ..\Resources\Shaders\') do .\glslangValidator.exe "..\Resources\Shaders\%%f"

for /f %%f in ('dir /b /a:-d ..\Resources\Shaders\') do (
    echo Compiling %%f to Compled\%%f.spv
    .\glslc.exe "..\Resources\Shaders\%%f" -o "..\Resources\Shaders\Compiled\%%f.spv"
)

pause