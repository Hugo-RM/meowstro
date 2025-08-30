# Meowstro - SDL2 Game Project

## Overview
Meowstro is a C++ game project using SDL2 and related libraries. The project uses CMake for building with platform-specific dependency management to optimize setup and development across different systems.

## Prerequisites

- **C++17 compiler** (Visual Studio 2022, GCC, Clang)
- **[CMake](https://cmake.org/download/)** (version 3.10 or higher)
- **Platform-specific dependencies** (see below and in DEPENDENCIES.md)

## Platform-Specific Setup

All of my group uses Windows so that's the recommended OS but I will try to make Linux and MacOS work. I made it so those OS get tested in the GitHub Actions too so hopefully everything works with that.

### Windows Setup

Install [vcpkg](https://github.com/microsoft/vcpkg) and integrate it:

```powershell
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
.\bootstrap-vcpkg.bat
.\vcpkg integrate install
```

Copy `build.default.bat` to `build.bat` (Command Prompt) or `build.default.ps1` to `build.ps1` (PowerShell), update the vcpkg path, then:

```cmd
.\build.bat
```
or
```ps1
.\build.ps1
```

Both scripts will:
- Configure the project using the Visual Studio generator (`-G "Visual Studio 17 2022" -A x64`)
- Use the vcpkg toolchain file
- Build the Debug configuration

> The output directory is always `build/bin/Debug` or `build/bin/Release` depending on the build type, on all platforms.

### Ubuntu Linux

Install SDL2 and development tools from the system package manager:

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    cmake \
    pkg-config \
    libsdl2-dev \
    libsdl2-image-dev \
    libsdl2-mixer-dev \
    libsdl2-ttf-dev
    
# Note: Requires SDL2 2.28.2+, SDL2_image 2.8.8+, SDL2_mixer 2.8.1+, SDL2_ttf 2.24.0+
```

Then build directly:
```bash
cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

### macOS

Homebrew:
```bash
brew install sdl2 sdl2_image sdl2_mixer sdl2_ttf
cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

## Build Scripts

For convenience, platform-specific build scripts are provided:

- **Windows**: `build.bat` or `build.ps1` (copy from `build.default.bat` and `build.default.ps1`)
- **Linux/macOS**: `build.sh` (copy from `build.default.sh`)

Personal build scripts contain system-specific paths and are ignored in `.gitignore`.

## Running the Game

After building, run the executable:

- **Debug build (default):** `./build/bin/Debug/meowstro` (add `.exe` on Windows)
- **Release build:** `./build/bin/Release/meowstro` (add `.exe` on Windows)

> The output directory is always `build/bin/Debug` or `build/bin/Release` depending on the build type, on all platforms.

## Additional Tools Used in GitHub Actions

- **cppcheck**
- **clang-tidy**

## Adding Files

Update `CMakeLists.txt` when adding new source files:

```cmake
set(SOURCES
    src/existing_file.cpp
    src/new_file.cpp  # Add here
    # ...
)

set(HEADERS
    include/existing_file.hpp
    include/new_file.hpp  # Add here
    # ...
)
```

## Directory Structure
```
/project-root
│-- .github/               # GitHub-specific files (workflows, templates)
│   │-- workflows/         # GitHub Actions workflows
│   │   │-- build.yml      # Workflow for building the project
│   │-- PULL_REQUEST_TEMPLATE.md # Pull request guidelines
|-- aseperite-imgs         # Aseperite files to modify image assets
│-- assets/                # Game assets (textures, sounds, etc.)
│   │-- audio/             # Audio files
│   │-- fonts/             # Font files
│   │-- images/            # Image files
│-- build/                 # CMake build files (includes build/bin/Debug and build/bin/Release)
|-- cmake/                 # copy_if_exists.cmake location
│-- docs/                  # Project documentation
│   │-- design_notes.md
│   │-- how_to_play.md
│-- include/               # Header files
│-- src/                   # Source files
│-- .gitattributes         # Git attributes file
│-- .gitignore             # Git ignore file
│-- build.default.bat      # Windows batch default build script
│-- build.default.ps1      # Windows PowerShell default build script
│-- build.default.sh       # macOS/Linux default build script
│-- CMakeLists.txt         # Main CMake build script
|-- DEPENDENCIES.md        # List of dependencies/versions used
│-- LICENSE                # Project LICENSE
│-- README.md              # Project README
|-- vcpkg.json             # JSON file w/ dependencies & versions used in vcpkg install

```