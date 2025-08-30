<# ===== build.default.ps1 ===== #>
# Edit VCPKG_PATH to match your path
$VCPKG_PATH = "C:/path/to/vcpkg"
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build