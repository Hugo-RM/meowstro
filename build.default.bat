@echo off
REM build.default.bat
REM Edit VCPKG_PATH to match your path
set VCPKG_PATH=C:\Users\Hugo\vcpkg
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=%VCPKG_PATH%\scripts\buildsystems\vcpkg.cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build