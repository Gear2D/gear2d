find_package(SDL2 REQUIRED)

add_definitions(-fPIC)
add_library(SDL2_framerate STATIC SDL2_framerate.c SDL2_framerate.h)
include_directories(${SDL2_INCLUDE_DIR})