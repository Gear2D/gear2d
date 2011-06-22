cmake_minimum_required(VERSION 2.6)
project(Gear2D)


find_package(SDL REQUIRED)
find_package(Boost REQUIRED)

# gear2d shared library
add_library(gear2d SHARED engine.cpp component.cc object.cc parameter.cc)
target_link_libraries(gear2d yaml-cpp SDL )

# get public headers
file(GLOB public_headers "*.h")

# gear2d executable
add_executable(main main.cc)
target_link_libraries(main gear2d)
set_target_properties(main PROPERTIES OUTPUT_NAME gear2d BUILD_WITH_INSTALL_RPATH true)

# include dirs so we can #include properly
include_directories(thirdparty/yaml-cpp/include/ ${SDL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS} ${OPENGL_INCLUDE_DIR})

add_subdirectory(thirdparty/yaml-cpp)

# install rules for the executable
install(TARGETS gear2d main
		RUNTIME DESTINATION bin
		LIBRARY DESTINATION lib
)

install(FILES ${public_headers}
		DESTINATION include/gear2d

)

add_subdirectory(component)

# win32 shared libraries for install dependencies
if (WIN32)
	set(CPACK_SOURCE_GENERATOR ZIP)
	set(CPACK_GENERATOR NSIS;ZIP)
	add_subdirectory(thirdparty/win32)
	set(CPACK_NSIS_MODIFY_PATH ON)
elseif(UNIX)
	set(CPACK_SOURCE_GENERATOR TGZ)
	set(CPACK_GENERATOR TGZ)
endif()

set(CPACK_PACKAGE_NAME "Gear2D")
set(CPACK_PACKAGE_VENDOR "Gear2D")
set(CPACK_PACKAGE_INSTALL_DIRECTORY "Gear2D")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Gear2D - Component-based game engine")
set(CPACK_PACKAGE_VERSION_MAJOR "0")
set(CPACK_PACKAGE_VERSION_MINOR "1")
set(CPACK_PACKAGE_VERSION_PATCH "i")
set(CPACK_SOURCE_IGNORE_FILES "\\\\.svn/*;${CMAKE_BINARY_DIR}/*")
set(PACK_PACKAGE_EXECUTABLES gear2d;Gear2D Game Engine)

# control of what we need from yaml-cpp
set(YAML_CPP_BUILD_TOOLS OFF)
set(YAML_CPP_BUILD_CONTRIB OFF)

include(CPack)
