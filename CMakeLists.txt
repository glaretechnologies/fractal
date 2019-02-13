cmake_minimum_required(VERSION 3.0)

project(Fractal)

set(SDL_DIR "../SDL2-2.0.9" CACHE FILEPATH "SDL source directory")
set(SDL_BUILD_DIR "../SDL2-2.0.9_build" CACHE FILEPATH "SDL build directory")

include_directories("${SDL_DIR}/include")
include_directories("./")

add_executable(fractal
vert.glsl
frag.glsl
fractal.cpp
gl3w.c
)

target_link_libraries(fractal
"${SDL_BUILD_DIR}/SDL2.lib"
"${SDL_BUILD_DIR}/SDL2main.lib"
Opengl32.lib
)
