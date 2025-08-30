@echo off
REM build.default.bat
REM Edit VCPKG_PATH to match your path
set VCPKG_PATH=C:/path/to/vcpkg

REM Only run this if you add/change dependencies or set up vcpkg for the first time:
REM "%VCPKG_PATH%/vcpkg.exe" install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf --triplet=x64-windows

cmake -B build -S . -G "Visual Studio 17 2022" -A x64 -DCMAKE_TOOLCHAIN_FILE="%VCPKG_PATH%/scripts/buildsystems/vcpkg.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build --config Debug