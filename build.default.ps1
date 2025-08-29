# Edit VCPKG_PATH to match your path
$VCPKG_PATH = "C:/path/to/vcpkg"
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake"
cmake --build build