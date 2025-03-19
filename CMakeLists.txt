cmake_minimum_required(VERSION 3.10)
project(game-test VERSION 1.0.0)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output path
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Read JSON Configuration File (if it exists)
if(EXISTS "${CMAKE_SOURCE_DIR}/config.json")
    file(READ "${CMAKE_SOURCE_DIR}/config.json" CONFIG_JSON)

    # Extract paths from JSON
    string(JSON SDL2_DIR GET ${CONFIG_JSON} SDL2_DIR)
    string(JSON SDL2_IMAGE_DIR GET ${CONFIG_JSON} SDL2_IMAGE_DIR)
    string(JSON SDL2_MIXER_DIR GET ${CONFIG_JSON} SDL2_MIXER_DIR)
    string(JSON SDL2_TTF_DIR GET ${CONFIG_JSON} SDL2_TTF_DIR)

    # Validate JSON Extraction
    if (NOT SDL2_DIR OR NOT SDL2_IMAGE_DIR OR NOT SDL2_MIXER_DIR OR NOT SDL2_TTF_DIR)
        message(FATAL_ERROR "Invalid config.json format! Ensure all paths are correctly set.")
    endif()

    # Set Library and Include Paths
    set(SDL2_LIBRARY "${SDL2_DIR}/lib/x64/SDL2.lib")
    set(SDL2MAIN_LIBRARY "${SDL2_DIR}/lib/x64/SDL2main.lib")
    set(SDL2_INCLUDE_DIR "${SDL2_DIR}/include")

    set(SDL2_IMAGE_LIBRARY "${SDL2_IMAGE_DIR}/lib/x64/SDL2_image.lib")
    set(SDL2_IMAGE_INCLUDE_DIR "${SDL2_IMAGE_DIR}/include")

    set(SDL2_MIXER_LIBRARY "${SDL2_MIXER_DIR}/lib/x64/SDL2_mixer.lib")
    set(SDL2_MIXER_INCLUDE_DIR "${SDL2_MIXER_DIR}/include")

    set(SDL2_TTF_LIBRARY "${SDL2_TTF_DIR}/lib/x64/SDL2_ttf.lib")
    set(SDL2_TTF_INCLUDE_DIR "${SDL2_TTF_DIR}/include")

else()
    message(WARNING "config.json not found! Falling back to find_package().")

    # Find SDL2 Automatically if JSON is missing
    find_package(SDL2 REQUIRED)
    find_package(SDL2_image REQUIRED)
    find_package(SDL2_mixer REQUIRED)
    find_package(SDL2_ttf REQUIRED)
endif()

# Define Source Files
set(SOURCES
    src/game.cpp
    src/RenderWindow.cpp
    src/Entity.cpp
)

# Define Header Files
set(HEADERS
    include/RenderWindow.hpp
    include/Entity.hpp
)

# Ensure at least one source file exists
if (NOT SOURCES)
    message(FATAL_ERROR "No source files found in src/ directory.")
endif()

# Create the Executable
set(BIN_NAME game)
if(DEFINED ENV{GITHUB_ACTIONS})
    add_definitions(-DCI_BUILD)
endif()
cmake_policy(SET CMP0156 NEW)
add_executable(${BIN_NAME} ${SOURCES} ${HEADERS})

# Include Directories
target_include_directories(${BIN_NAME} PUBLIC include)
target_include_directories(${BIN_NAME} PUBLIC ${SDL2_INCLUDE_DIR} ${SDL2_IMAGE_INCLUDE_DIR} ${SDL2_MIXER_INCLUDE_DIR} ${SDL2_TTF_INCLUDE_DIR})

# Ensure SDL2main is linked
target_link_libraries(${BIN_NAME} ${SDL2_LIBRARY} ${SDL2MAIN_LIBRARY} ${SDL2_IMAGE_LIBRARY} ${SDL2_MIXER_LIBRARY} ${SDL2_TTF_LIBRARY})

cmake_policy(SET CMP0112 NEW)

# Move DLLs to the same location as the executable
add_custom_command(TARGET ${BIN_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
        "${SDL2_DIR}/lib/x64/SDL2.dll"
        "${SDL2_IMAGE_DIR}/lib/x64/SDL2_image.dll"
        "${SDL2_MIXER_DIR}/lib/x64/SDL2_mixer.dll"
        "${SDL2_TTF_DIR}/lib/x64/SDL2_ttf.dll"
        "$<TARGET_FILE_DIR:${BIN_NAME}>"
)

# Copy assets (res/ directory) to the output bin folder
file(COPY assets DESTINATION ${CMAKE_RUNTIME_OUTPUT_DIRECTORY})

# Explicitly tell Visual Studio that headers belong to the project
source_group("Header Files" FILES ${HEADERS})
source_group("Source Files" FILES ${SOURCES})
