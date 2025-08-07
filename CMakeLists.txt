cmake_minimum_required(VERSION 3.10)
project(GraphicsApp)

find_package(SDL2 REQUIRED)
find_package(SDL2_IMAGE REQUIRED)
find_package(SDL2_TTF REQUIRED)

add_executable(app
    main.cpp
    graphics_sdl.cpp
)

target_link_libraries(app SDL2::SDL2 SDL2_image::SDL2_image SDL2_ttf::SDL2_ttf)
target_include_directories(app PRIVATE ${SDL2_INCLUDE_DIRS})
