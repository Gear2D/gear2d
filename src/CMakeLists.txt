cmake_minimum_required(VERSION 2.6)
project(gear2d)

find_package(SDL REQUIRED)

# default build type should be release.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE)

if (${CMAKE_BUILD_TYPE} STREQUAL "Debug")
  add_definitions(-DLOGTRACE)
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug")

# set RPATH to point to the library install destination
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

# gear2d shared library
add_library(gear2d SHARED engine.cc component.cc object.cc parameter.cc log.cc)
target_link_libraries(gear2d yaml-cpp SDL)

# get public headers
file(GLOB public_headers "*.h")

# engine executable
add_executable(main main.cc)

target_link_libraries(main gear2d)
set_target_properties(main PROPERTIES OUTPUT_NAME gear2d BUILD_WITH_INSTALL_RPATH true)

# include dirs so we can #include properly
include_directories(thirdparty/yaml-cpp/include/ ${SDL_INCLUDE_DIR})
link_directories(${SDL_LINK_DIR})

add_subdirectory(thirdparty/yaml-cpp)
# add_subdirectory(thirdparty/bullet-2.78)

# install rules for the executable
install(TARGETS gear2d main
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
)

install(FILES ${public_headers}
		DESTINATION include/gear2d

)

install(FILES ${CMAKE_SOURCE_DIR}/LICENSE ${CMAKE_SOURCE_DIR}/README
	DESTINATION share/doc/${PROJECT_NAME})


# control of what we need from yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)