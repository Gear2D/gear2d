cmake_minimum_required(VERSION 2.6)
project(gear2d)

find_package(SDL REQUIRED)

# default build type should be release.
if (NOT CMAKE_BUILD_TYPE)
	set(CMAKE_BUILD_TYPE "Release")
endif (NOT CMAKE_BUILD_TYPE)

# set RPATH to point to the library install destination
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

# gear2d shared library
add_library(gear2d SHARED engine.cpp component.cc object.cc parameter.cc)
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
	DESTINATION .)


# control of what we need from yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)

# win32 shared libraries for install dependencies
if (WIN32)
	set(CPACK_SOURCE_GENERATOR ZIP)
	set(CPACK_GENERATOR NSIS;ZIP)
	add_subdirectory(thirdparty/win32)
	set(CPACK_NSIS_MODIFY_PATH ON)
	set(CPACK_PACKAGE_NAME "Gear2D")
	
	set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
	set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++")
	set(CMAKE_MODULE_LINKER_FLAGS "")
	
elseif(UNIX)
	set(CPACK_PACKAGE_NAME "gear2d")
	set(CPACK_SOURCE_GENERATOR TGZ)
	set(CPACK_GENERATOR TGZ)
endif()

set(Gear2D_VERSION_MAJOR "0")
set(Gear2D_VERSION_MINOR "0")
set(Gear2D_VERSION_PATCH "3")

set(Gear2D_VERSION "${Gear2D_VERSION_MAJOR}.${Gear2D_VERSION_MINOR}.${Gear2D_VERSION_PATCH}")
add_definitions(-DGEAR2D_VERSION="${Gear2D_VERSION}")


set(CPACK_PACKAGE_VENDOR "Gear2D Labs")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gear2d")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gear2D - Component-based game engine")
set(CPACK_PACKAGE_VERSION ${Gear2D_VERSION})

set(CPACK_SOURCE_IGNORE_FILES "\\\\.git.*;${CMAKE_BINARY_DIR}/.*;.*~;\\\\.kdev4/.*;.*\\\\.kdev4;.*\\\\.swp")
set(PACK_PACKAGE_EXECUTABLES gear2d;Gear2D Game Engine)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)
set(CPACK_RESOURCE_FILE_README ${CMAKE_SOURCE_DIR}/README)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")
if (NOT WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.bin")
endif

include(CPack)
