# Meowstro - SDL2 Game Project

## Overview
Meowstro is a C++ game project using SDL2 and related libraries. The project uses CMake for building with platform-specific dependency management to optimize setup and development across different systems.

## Prerequisites

- **C++17 compiler** (Visual Studio 2022, GCC, Clang)
- **[CMake](https://cmake.org/download/)** (version 3.10 or higher)
- **Platform-specific dependencies** (see below)

## Platform-Specific Setup

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
```

Then build directly:
```bash
cmake -B build -S . -DCMAKE_EXPORT_COMPILE_COMMANDS=ON
cmake --build build
```

### Windows

Install [vcpkg](https://github.com/microsoft/vcpkg) and use it to install the required libraries:

```cmd
# From the vcpkg directory
.\vcpkg install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf
.\vcpkg integrate install
```

Copy `build.default.bat` to `build.bat` (Command Prompt) and update the vcpkg path, then:
```cmd
.\build.bat
```

For Powershell, do the same process but with the `build.default.ps1` file:
```ps1
.\build.ps1
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

- **All platforms**: `./build/bin/Debug/meowstro` (with `.exe` on Windows)

## Additional Tools Used

- **cppcheck**: Static analysis
- **clang-tidy**: Advanced linting

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
│-- build/                 # CMake build files (includes build/bin/Debug)
│-- docs/                  # Project documentation
│   │-- design_notes.md
│   │-- how_to_play.md
│-- include/               # Header files
│-- src/                   # Source files
│-- .gitattributes         # Git attributes file
│-- .gitignore             # Git ignore file
│-- CMakeLists.txt         # Main CMake build script
│-- LICENSE                # Project LICENSE
│-- README.md              # Project README
│-- build.ps1              # Windows PowerShell build script
│-- build.default.bat      # Windows batch build script
│-- build.default.sh       # macOS/Linux build script
```