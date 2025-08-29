cmake_minimum_required(VERSION 3.10)
project(Meowstro VERSION 1.0.0)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

find_package(SDL2 CONFIG REQUIRED)
find_package(SDL2_image CONFIG REQUIRED)
find_package(SDL2_mixer CONFIG REQUIRED)
find_package(SDL2_ttf CONFIG REQUIRED)

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

target_link_libraries(meowstro
    PRIVATE
    SDL2::SDL2
    SDL2::SDL2main
    SDL2_image::SDL2_image
    SDL2_mixer::SDL2_mixer
    SDL2_ttf::SDL2_ttf
)

target_include_directories(meowstro PRIVATE include)

# Copy assets folder to the output directory after build
add_custom_command(TARGET meowstro POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        "${CMAKE_SOURCE_DIR}/assets"
        "$<TARGET_FILE_DIR:meowstro>/assets"
)