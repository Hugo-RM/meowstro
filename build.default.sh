#!/bin/bash
# build.default.sh - Cross-platform build script

# Detect platform and use appropriate method
if [[ "$OSTYPE" == "linux-gnu"* ]]; then
    echo "Linux detected - using system packages (recommended)"
    echo "Make sure SDL2 development packages are installed:"
    echo "sudo apt-get install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev libsdl2-ttf-dev"
    echo ""
    
    # Build without vcpkg - CMake will find system packages
    cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
    
elif [[ "$OSTYPE" == "darwin"* ]]; then
    echo "macOS detected - checking for Homebrew SDL2..."
    
    if brew list sdl2 &>/dev/null; then
        echo "Using Homebrew SDL2 packages"
        cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
    else
        echo "SDL2 not found via Homebrew. Install with:"
        echo "brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf"
        echo ""
        echo "Or use vcpkg by setting VCPKG_PATH below and uncommenting vcpkg section"
        # Uncomment and set path if using vcpkg:
        # VCPKG_PATH="$HOME/vcpkg"
        # cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
        exit 1
    fi
    
else
    echo "Unknown platform, trying vcpkg method..."
    echo "Edit VCPKG_PATH below to match your vcpkg installation:"
    VCPKG_PATH="$HOME/vcpkg"
    cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE="$VCPKG_PATH/scripts/buildsystems/vcpkg.cmake" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DCMAKE_BUILD_TYPE=Debug
fi

# Build the project
if [ $? -eq 0 ]; then
    echo "Configuration successful, building..."
    cmake --build build
    
    if [ $? -eq 0 ]; then
        echo "Build completed successfully!"
        echo "Executable: build/bin/Debug/meowstro"
        echo "Compile commands: build/compile_commands.json"
    else
        echo "Build failed!"
        exit 1
    fi
else
    echo "CMake configuration failed!"
    exit 1
fi