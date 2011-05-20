cmake_minimum_required(VERSION 2.6)
project(gear2d)

find_package(SDL REQUIRED)
find_package(Boost REQUIRED)

# yaml-cpp dependency
add_subdirectory(thirdparty/yaml-cpp)

# component dependecy
add_subdirectory(component)

# gear2d shared library
add_library(gear2d SHARED engine.cpp component.cc object.cc parameter.cc)
target_link_libraries(gear2d yaml-cpp SDL )

# get public headers
file(GLOB public_headers "*.h")

# gear2d executable
add_executable(main main.cc)
target_link_libraries(main gear2d)
set_target_properties(main PROPERTIES OUTPUT_NAME gear2d BUILD_WITH_INSTALL_RPATH true PUBLIC_HEADER "${public_headers}")

# include dirs so we can #include properly
include_directories(thirdparty/yaml-cpp/include/ ${SDL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

# install rules for the executable
install(TARGETS gear2d main
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
		PUBLIC_HEADER DESTINATION include/gear2d
)

# control of what we need from yaml-cpp
option(YAML_CPP_BUILD_TOOLS OFF)
option(YAML_CPP_BUILD_CONTRIB OFF)