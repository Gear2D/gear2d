set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/" "${CMAKE_SOURCE_DIR}")

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

# Handy script by rpavlik http://github.com/rpavlik/
include(GetGitRevisionDescription) 

# Gets the tag version of the repository
git_describe(Gear2D_VERSION --tags)

# Check if we are building from git. If we are, configure the VERSION file, if not,
# use the VERSION file to calculate the project version
if(Gear2D_VERSION AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in")
  configure_file("${CMAKE_CURRENT_SOURCE_DIR}/VERSION.in" "${CMAKE_CURRENT_SOURCE_DIR}/VERSION" @ONLY)
elseif(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/VERSION")
  file(STRINGS VERSION Gear2D_VERSION)
else()
  set(Gear2D_VERSION "unknown")
endif()

# Identify the build type
if (NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE "Debug")
endif (NOT CMAKE_BUILD_TYPE)

set(CMAKE_CXX_FLAGS_DEBUG "-ggdb -Wall")

# Identify if logtrace should be defined
if (${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  add_definitions(-DLOGTRACE)
  message(STATUS "Build type enables logging facilities")
endif(${CMAKE_BUILD_TYPE} STREQUAL "Debug" OR ${CMAKE_BUILD_TYPE} STREQUAL "RelWithDebInfo")
  
# set RPATH to point to the library install destination
set(CMAKE_INSTALL_RPATH ${CMAKE_INSTALL_PREFIX}/lib)

# Add version as a compiler definition
add_definitions(-DGEAR2D_VERSION="${Gear2D_VERSION}")
message(STATUS "Building Gear2D game engine version ${Gear2D_VERSION} as ${CMAKE_BUILD_TYPE}")

export(PACKAGE Gear2D)

add_subdirectory(src)
add_subdirectory(doc)

include(InstallRequiredSystemLibraries)

# win32 shared libraries for install dependencies
if (WIN32)
	set(CPACK_SOURCE_GENERATOR ZIP)
	set(CPACK_GENERATOR NSIS;ZIP)
	set(CPACK_NSIS_MODIFY_PATH ON)
	set(CPACK_PACKAGE_NAME "Gear2D")
elseif(UNIX)
	set(CPACK_PACKAGE_NAME "gear2d")
	set(CPACK_SOURCE_GENERATOR TGZ)
	set(CPACK_GENERATOR STGZ)
endif()

set(CPACK_PACKAGE_VENDOR "Gear2D Labs")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "gear2d")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gear2D - Component-based game engine")
set(CPACK_PACKAGE_VERSION ${Gear2D_VERSION})

set(CPACK_SOURCE_IGNORE_FILES "\\\\.git.*;${CMAKE_BINARY_DIR}/.*;.*~;\\\\.kdev4/.*;.*\\\\.kdev4;.*\\\\.swp;.*examples/.*/build/.*;.*\\\\.o;VERSION.in")
set(PACK_PACKAGE_EXECUTABLES gear2d;Gear2D Game Engine)
set(CPACK_RESOURCE_FILE_LICENSE ${CMAKE_SOURCE_DIR}/LICENSE)

set(CPACK_SOURCE_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}")

if (MSVC)
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-MSVC-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
else()
  set(CPACK_PACKAGE_FILE_NAME "${CPACK_PACKAGE_NAME}-${CPACK_PACKAGE_VERSION}-${CMAKE_SYSTEM_PROCESSOR}")
endif()

include(CPack)

# configure the gear2d-config stuff
if (UNIX)
  set(CMAKECONFIG_INSTALL_DIR "lib/${PROJECT_NAME}/cmake" CACHE STRING "Where ${PROJECT_NAME}-config.cmake and companions will be installed")
elseif(WIN32)
  set(CMAKECONFIG_INSTALL_DIR "cmake"  CACHE STRING "Where ${PROJECT_NAME}-config.cmake and companions will be installed")
elseif(APPLE)
  set(CMAKECONFIG_INSTALL_DIR "cmake" CACHE STRING "Where ${PROJECT_NAME}-config.cmake and companions will be installed")
endif()

set(INCLUDE_INSTALL_DIR include/gear2d)
set(LIB_INSTALL_DIR lib)

# Generate gear2d-config.cmake and friends
include(CMakePackageConfigHelpers)
configure_package_config_file(cmake/gear2d-config.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/tmpg2dconfig # find_package goes crazy searching in the build in other folders
                              INSTALL_DESTINATION ${CMAKECONFIG_INSTALL_DIR}
                              PATH_VARS INCLUDE_INSTALL_DIR LIB_INSTALL_DIR)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/gear2d-config-version.cmake
                                 VERSION ${Gear2D_VERSION}
                                 COMPATIBILITY SameMajorVersion)


install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/gear2d-config-version.cmake
        ${CMAKE_SOURCE_DIR}/cmake/gear2d-macros.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR} )
        
install(FILES       
        ${CMAKE_CURRENT_BINARY_DIR}/tmpg2dconfig
        RENAME gear2d-config.cmake
        DESTINATION ${CMAKECONFIG_INSTALL_DIR} )

# install gear2d-targets.cmake
install(EXPORT gear2d-targets DESTINATION ${CMAKECONFIG_INSTALL_DIR})

message(STATUS "Gear2D will be installed at ${CMAKE_INSTALL_PREFIX}")
