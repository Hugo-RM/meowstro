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

### 3. Creating Files (Skip if You're Not Creating a New File)

> ⚠️ **IMPORTANT: YOU MUST DO THIS EVERY TIME YOU CREATE A NEW `.cpp` OR `.hpp` FILE.**  
> If you don’t do this, the file will not be included in the project build. You must also rebuild the project after adding new files.

Depending on how you created your new file, you may need to manually move it to the correct directory. Visual Studio, for example, might place new files inside the `./build` directory.

If you used **Add Class** or **New Item** in Visual Studio:
- Move the generated `.cpp` files to the `src/` directory.
- Move the generated `.hpp` files to the `include/` directory.

Alternatively, you can just **create the files directly inside the `src/` and `include/` folders** before building.

After the files are correctly placed, you must update the `CMakeLists.txt` file so that they are included in the build:

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

### 4. Build the Project
You have two common ways to build and run the project: **Visual Studio** or the **Command Line**.

---

#### 🔷 Method 1: Using Visual Studio

1. Open the `.sln` file located inside the `build/` directory.
2. In the **Solution Explorer**, right-click on the project named `game` and select **"Set as Startup Project"**.
3. Press `Ctrl + F5` or click **Debug → Start Without Debugging** to run the project.

✅ That’s it! Visual Studio will automatically build and run the project.

---

#### ⚙️ Method 2: Using Command Line

Make sure you're in the root of the project and then run the following:

#### Change to the Build Directory
```sh
cd "./build"
```

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

