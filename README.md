
# Meowstro - SDL2 Game Project

## Overview
Meowstro is a C++ game project using SDL2 and related libraries. The project uses CMake for building and vcpkg for dependency management, making setup and development easy across platforms.

## Prerequisites

- **Visual Studio 2022** (or any C++17 compiler)
- **[CMake](https://cmake.org/download/)** (version 3.10 or higher)
- **[vcpkg](https://github.com/microsoft/vcpkg)** (for dependency management)

## Setup Instructions

### 1. Install Dependencies

Install [vcpkg](https://github.com/microsoft/vcpkg) and use it to install the required libraries:

```sh
# From the vcpkg directory
./vcpkg install sdl2 sdl2-image sdl2-mixer[mpg123] sdl2-ttf
./vcpkg integrate install
```

### 2. Build the Project

Copy the build.default files and update the path for vcpkg to be accurate. 

Personal build scripts will contain personal paths which are not helpful for others. It is why they are ignored in the `.gitignore`. 

Then use the provided script for your platform:

- **Windows (PowerShell):**
    ```sh
    ./build.ps1
    ```
- **Windows (Batch):**
    ```bat
    build.bat
    ```
- **macOS/Linux:**
    ```sh
    ./build.sh
    ```

This will configure and build the project using CMake and vcpkg.

### 3. Run the Game

After building, run the executable from the build output directory:

- **Windows:**
    ```sh
    ./build/bin/Debug/meowstro.exe
    ```
- **macOS/Linux:**
    ```sh
    ./build/bin/Debug/meowstro
    ```

Or, open the solution file in Visual Studio (`build/Meowstro.sln`), set `meowstro` as the startup project, and run it.

### 4. Add Files

After the files are correctly placed in their directories, you must update the `CMakeLists.txt` file so that they are included in the build:

```cmake
# Define Source Files
set(SOURCES
    src/file_1.cpp
    src/file_2.cpp
    ...
    src/file_X.cpp
)

# Define Header Files
set(HEADERS
    include/file_1.hpp
    ...
    include/file_X.hpp
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

## Contribution Guidelines
- Use vcpkg to add new dependencies and document them in the README.
- If modifying build configuration, ensure compatibility with Windows and cross-platform builds.
- Follow C++ best practices and maintain code clarity.