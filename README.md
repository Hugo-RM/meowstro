# Meowstro - SDL2 Game Project

## Overview
This is a game project using SDL2 and its related libraries. The project is configured using CMake and follows a structured build process to make it easy for developers to set up and contribute.

## Supported Platforms
- **Windows** ✅
- **Linux & macOS** ❌ (Not supported yet. Future support is planned.)

## Prerequisites
Before setting up the project, ensure you have the following installed:
- **Visual Studio 2022** (or any C++ compiler supporting C++17)
- **[CMake](https://cmake.org/download/)** (minimum version 3.10 | I recommend not getting the Release Candidate)

## Setup Instructions

### 1. Install Dependencies
Download and extract the following libraries:
- **[SDL2](https://github.com/libsdl-org/SDL/releases/download/release-2.32.2/SDL2-devel-2.32.2-VC.zip)**
- **[SDL2_image](https://github.com/libsdl-org/SDL_image/releases/download/release-2.8.8/SDL2_image-devel-2.8.8-VC.zip)**
- **[SDL2_mixer](https://github.com/libsdl-org/SDL_mixer/releases/download/release-2.8.1/SDL2_mixer-devel-2.8.1-VC.zip)**
- **[SDL2_ttf](https://github.com/libsdl-org/SDL_ttf/releases/download/release-2.24.0/SDL2_ttf-devel-2.24.0-VC.zip)**

Make sure to note their extracted paths as you will need them for configuration.

### 2. Configure the Project
Before running CMake, you must configure the project by setting the correct paths in the configuration files:

#### Editing Configuration Files
Create a copy of the default configuration file `config.default.json` and update it with your correct paths:
- `config.default.json` → `config.json`

Edit `config.json` and update the paths for SDL2 and its related libraries:
```json
{
    "SDL2_DIR": "path-to-directory/SDL2-2.32.2",
    "SDL2_IMAGE_DIR": "path-to-directory/SDL2_image-2.8.8",
    "SDL2_MIXER_DIR": "path-to-directory/SDL2_mixer-2.8.1",
    "SDL2_TTF_DIR": "path-to-directory/SDL2_ttf-2.24.0"
}
```

The `CMakePresets.json` will be updated with your path after running the Python Script.

The Python File `generate_cmake_presets.py` can be run with the following command:

```sh
python generate_cmake_presets.py
```

### 3. Creating files (skip if you're not creating new file)
Depending on the method you used to create a new file, you will need to somehow get that file to the src or include directories. They will probably be directly under the ./build directory if you're already using visual studio. If not just locate the file(s) and cut and paste them to the correct place.

If you used add class or new item in visual studio, you will need to simply cut and paste the files into the right folder (include being .hpp files, src being .cpp files).

You can also just create the files in the src and include files then build the project to have them there.

After the files are in the src and include folders, you will need to add them into the `CMakeLists.txt`. You will scroll down to see the following

```txt
# Define Source Files
set(SOURCES
    src/file_1.cpp
    src/file_2.cpp
    ...
    src/file_X.cpp
)
# ... means that there can be more files between them
# Define Header Files
set(HEADERS
    include/file_1.hpp
    ...
    include/file_x.cpp
)
```

Just add the filename(s) to this part of the `CMakeLists.txt` and build the project for it to work :).

### 4. Build the Project
Navigate to the project root and run the following commands:

#### Configure CMake
```sh
cmake --preset my-build
```
(`cmake --preset my-build` will create a `build/` directory automatically)

#### Change to the Build Directory
```sh
cd "./build"
```

In the build directory run:

#### Build the Project
```sh
cmake --build .
```

#### Run the Built Executable
```sh
./bin/Debug/game.exe
```

## Directory Structure
```
/project-root
│-- .github/               # GitHub-specific files (workflows, templates)
│   │-- ISSUE_TEMPLATE/    # Issue templates for bug reports and feature requests
│   │   │-- bug_report.md
│   │   │-- feature_request.md
│   │-- workflows/         # GitHub Actions workflows
│   │   │-- build.yml      # Workflow for building the project
│   │-- PULL_REQUEST_TEMPLATE.md # Pull request guidelines
│-- assets/                # Game assets (textures, sounds, etc.)
│   │-- audio/             # Audio files
│   │-- fonts/             # Font files
│   │-- images/            # Image files
│-- build/                 # CMake build files (includes build/bin/Debug)
│-- docs/                  # Project documentation
│   │-- API_documentation.md
│   │-- design_notes.md
│   │-- how_to_play.md
│-- include/               # Header files
│-- src/                   # Source files
│-- .gitattributes         # Git attributes file
│-- .gitignore             # Git ignore file
│-- CMakeLists.txt         # Main CMake build script
│-- CMakePresets.default.json  # Default CMake preset
│-- CMakePresets.json      # Active CMake preset configuration (created by py script)
│-- config.default.json    # Default configuration file (rename to use)
│-- config.json            # Active configuration file
|-- generate_cmake_presets.py  # Generates the CMakePresets.json based on config.json
│-- LICENSE                # Project LICENSE
│-- CHANGELOG.md           # Project CHANGELOG
│-- README.md              # Project README
```

## Contribution Guidelines
- **DON'T** commit `config.json` or `CMakePresets.json`. These files contain personal paths should be local.
- If modifying build configuration, ensure compatibility with Windows.
- Follow C++ best practices and maintain code clarity.

---
This README will be updated as Linux and macOS support is added. 🚀

