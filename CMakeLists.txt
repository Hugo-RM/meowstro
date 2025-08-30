cmake_minimum_required(VERSION 3.10)
project(Meowstro VERSION 2.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Try vcpkg first, then fall back to system packages
find_package(SDL2 CONFIG QUIET)
if(NOT SDL2_FOUND)
    find_package(PkgConfig REQUIRED)
    pkg_check_modules(SDL2 REQUIRED sdl2)
    pkg_check_modules(SDL2_IMAGE REQUIRED SDL2_image)
    pkg_check_modules(SDL2_MIXER REQUIRED SDL2_mixer)
    pkg_check_modules(SDL2_TTF REQUIRED SDL2_ttf)
else()
    find_package(SDL2_image CONFIG REQUIRED)
    find_package(SDL2_mixer CONFIG REQUIRED)
    find_package(SDL2_ttf CONFIG REQUIRED)
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
)

set(HEADERS
    include/RenderWindow.hpp
    include/Entity.hpp
    include/Audio.hpp
    include/AudioLogic.hpp
    include/Font.hpp
    include/Sprite.hpp
    include/GameStats.hpp
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
        COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_BINARY_DIR}/compile_commands.json
            ${CMAKE_SOURCE_DIR}/compile_commands.json
        DEPENDS ${CMAKE_BINARY_DIR}/compile_commands.json
        COMMENT "Copying compile_commands.json to project root"
    )
endif()