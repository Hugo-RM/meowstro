cmake_minimum_required(VERSION 3.10)
project(Meowstro VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Set output directory to bin/Debug or bin/Release depending on build type
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_BINARY_DIR}/bin/Debug)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR}/bin/Release)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin) # fallback

# Tested versions - other versions use at your own risk
set(TESTED_SDL2_VERSION "2.30.0")
set(TESTED_SDL2_IMAGE_VERSION "2.8.2")
set(TESTED_SDL2_MIXER_VERSION "2.8.0")
set(TESTED_SDL2_TTF_VERSION "2.22.0")

# Try vcpkg first, then fall back to system packages
find_package(SDL2 ${TESTED_SDL2_VERSION} CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2>=${TESTED_SDL2_VERSION})
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image>=${TESTED_SDL2_IMAGE_VERSION})
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer>=${TESTED_SDL2_MIXER_VERSION})
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf>=${TESTED_SDL2_TTF_VERSION})
    
    message(STATUS "Using system SDL2 packages with minimum versions: SDL2 ${TESTED_SDL2_VERSION}, SDL2_image ${TESTED_SDL2_IMAGE_VERSION}, SDL2_mixer ${TESTED_SDL2_MIXER_VERSION}, SDL2_ttf ${TESTED_SDL2_TTF_VERSION}")
else()
    find_package(SDL2_image ${TESTED_SDL2_IMAGE_VERSION} CONFIG REQUIRED)
    find_package(SDL2_mixer ${TESTED_SDL2_MIXER_VERSION} CONFIG REQUIRED)
    find_package(SDL2_ttf ${TESTED_SDL2_TTF_VERSION} CONFIG REQUIRED)
endif()

set(SOURCES
    src/meowstro.cpp
    src/RenderWindow.cpp
    src/Entity.cpp
    src/Audio.cpp
    src/AudioLogic.cpp
    src/Font.cpp
    src/Sprite.cpp
    src/GameStats.cpp
    src/ResourceManager.cpp
    src/GameConfig.cpp
    src/InputHandler.cpp
    src/GameStateManager.cpp
    src/RhythmGame.cpp
    src/MenuSystem.cpp
    src/AnimationSystem.cpp
)

set(HEADERS
    include/RenderWindow.hpp
    include/Entity.hpp
    include/Audio.hpp
    include/AudioLogic.hpp
    include/Font.hpp
    include/Sprite.hpp
    include/GameStats.hpp
    include/ResourceManager.hpp
    include/GameConfig.hpp
    include/InputHandler.hpp
    include/GameStateManager.hpp
    include/RhythmGame.hpp
    include/MenuSystem.hpp
    include/AnimationSystem.hpp
)

add_executable(meowstro ${SOURCES} ${HEADERS})

# Link libraries based on which method was used
if(TARGET SDL2::SDL2)
    # vcpkg targets
    target_link_libraries(meowstro
        PRIVATE
        SDL2::SDL2
        SDL2::SDL2main
        SDL2_image::SDL2_image
        SDL2_mixer::SDL2_mixer
        SDL2_ttf::SDL2_ttf
    )
else()
    # System packages
    target_link_libraries(meowstro
        PRIVATE
        ${SDL2_LIBRARIES}
        ${SDL2_IMAGE_LIBRARIES}
        ${SDL2_MIXER_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
    )
    target_include_directories(meowstro
        PRIVATE
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_IMAGE_INCLUDE_DIRS}
        ${SDL2_MIXER_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
    )
endif()

target_include_directories(meowstro PRIVATE include)

# Copy assets folder to the output directory after build
add_custom_command(TARGET meowstro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:meowstro>/assets"
)

# Create a copy of compile_commands.json in the project root for IDE support
if(CMAKE_EXPORT_COMPILE_COMMANDS)
    add_custom_target(copy_compile_commands ALL
        COMMAND ${CMAKE_COMMAND} -Dsrc="${CMAKE_BINARY_DIR}/compile_commands.json" -Ddst="${CMAKE_SOURCE_DIR}/compile_commands.json" -P "${CMAKE_SOURCE_DIR}/cmake/copy_if_exists.cmake"
        BYPRODUCTS ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to project root if it exists"
        VERBATIM
    )
endif()