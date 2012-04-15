project(helloperson-component)
cmake_minimum_required(VERSION 2.6)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}")

# uses the FindGear2D.cmake to search for Gear2D
find_package(Gear2D REQUIRED)

set(CMAKE_INSTALL_PREFIX ${Gear2D_COMPONENT_PREFIX})
message(STATUS ${CMAKE_INSTALL_PREFIX})

# add the Gear2D include directories and link directories.
# they are calculated by FindGear2D.cmake
include_directories(${Gear2D_INCLUDE_DIR} ${SDL_INCLUDE_DIR})
link_directories(${Gear2D_LINK_DIR})

# add a new library called helloperson that will be created using
# helloperson.cc
add_library(helloperson MODULE helloperson.cc)

# say that the target helloperson will be installed inside
# a folder called "greeter"
install(TARGETS helloperson
		LIBRARY
		DESTINATION greeter)

