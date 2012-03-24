if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
message(FATAL_ERROR "CMake generation for gear2d not allowed within the source directory! 
Remove the CMakeCache.txt file and try again from another folder, e.g.: 

	rm CMakeCache.txt 
	cd ..
	mkdir build
	cd build
	cmake -G \"Unix Makefiles\" ../gear2d
")
endif()

cmake_minimum_required(VERSION 2.6)
project(gear2d)

set(Gear2D_VERSION_MAJOR "0")
set(Gear2D_VERSION_MINOR "0")
set(Gear2D_VERSION_PATCH "4")

set(Gear2D_VERSION "${Gear2D_VERSION_MAJOR}.${Gear2D_VERSION_MINOR}.${Gear2D_VERSION_PATCH}")
add_definitions(-DGEAR2D_VERSION="${Gear2D_VERSION}")
message(STATUS "Building Gear2D ${Gear2D_VERSION}")

add_subdirectory(src)
add_subdirectory(doc)

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
if(NOT WIN32)
	set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}.bin")
endif(NOT WIN32)

include(CPack)
