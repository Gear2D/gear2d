cmake_minimum_required(VERSION 2.6)
project(gear2d)

find_package(SDL REQUIRED)

if (MSVC)
	set(YAML_LIBRARY ${CMAKE_BINARY_DIR}/thirdparty/yaml-0.1.4/${CMAKE_CFG_INTDIR}/yaml.lib)
else (MSVC)
	set(YAML_LIBRARY ${CMAKE_BINARY_DIR}/thirdparty/yaml-0.1.4/libyaml.a)
endif ()

message(STATUS "YAML Library: ${YAML_LIBRARY}")

# gear2d shared library
add_definitions(-DYAML_DECLARE_STATIC)
add_library(gear2d SHARED engine.cc component.cc object.cc parameter.cc log.cc sigfile.cc)
target_link_libraries(gear2d ${YAML_LIBRARY} ${SDL_LIBRARY})
add_dependencies(gear2d yaml)

# get public headers
file(GLOB public_headers "*.h")

# engine executable
add_executable(main main.cc)

target_link_libraries(main gear2d)
set_target_properties(main PROPERTIES OUTPUT_NAME gear2d BUILD_WITH_INSTALL_RPATH true)

# include dirs so we can #include properly
include_directories(${CMAKE_SOURCE_DIR}/thirdparty/yaml-0.1.4/include/ ${SDL_INCLUDE_DIR})

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