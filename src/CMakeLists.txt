cmake_minimum_required(VERSION 2.6)
project(gear2d)
set(SDL_BUILDING_LIBRARY 1)
set(SDL2_BUILDING_LIBRARY 1)
find_package(SDL2 REQUIRED)

# SDL2 needs to go with us if using windows.
if (WIN32)
  message(STATUS "Packaging SDL2 Libraries: ${SDL2_LIBRARY_DIR}/SDL2.dll")
  message(STATUS "SDL2 Library dir: ${SDL2_LIBRARY_DIR}")
  install(FILES ${SDL2_LIBRARY_DIR}/SDL2.dll DESTINATION bin)
  if (MINGW)
    # pass all these flags to avoid passing gcc libs around in mingw
    message(STATUS "Compiling with MINGW!")
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -static -lpthread -dynamic")
    set(CMAKE_SHARED_LINKER_FLAGS "-static-libstdc++ -static-libgcc -static -lpthread -dynamic")
    set(CMAKE_MODULE_LINKER_FLAGS "-static-libstdc++ -static-libgcc -static -lpthread -dynamic")
    endif(MINGW)
endif()

# get public headers
file(GLOB public_headers "*.h")

# get YAML sources
file(GLOB yaml_sources "${CMAKE_SOURCE_DIR}/thirdparty/yaml-0.1.4/src/*.c")

# get SDL2_framerate sources
set(SDL2_framerate_source "${CMAKE_SOURCE_DIR}/thirdparty/SDL2_framerate/SDL2_framerate.c")

# flags to enable c++11 on gcc
if(CMAKE_COMPILER_IS_GNUCXX)
  set(CMAKE_CXX_FLAGS -std=c++11)
  if (UNIX)
    add_definitions(-fPIC)
  endif()
endif()

# compile yaml library ourselves. Pass all the defines they want.
add_library(yaml OBJECT ${yaml_sources})
set_target_properties(yaml PROPERTIES COMPILE_FLAGS "-w -fPIC -DYAML_DECLARE_STATIC -DYAML_VERSION_MAJOR=0 -DYAML_VERSION_MINOR=1 -DYAML_VERSION_PATCH=4 -DYAML_VERSION_STRING=\\\"0.1.4\\\"")

# generate an object library to avoid compiling these files twice
add_library(gear2d-objects OBJECT engine.cc component.cc object.cc parameter.cc sigfile.cc logtrace.cc)
add_library(gear2d
  SHARED 
  $<TARGET_OBJECTS:gear2d-objects>
  $<TARGET_OBJECTS:yaml> ${SDL2_framerate_source} ${public_headers}
)
add_library(gear2d-static
  STATIC 
  $<TARGET_OBJECTS:gear2d-objects>
  $<TARGET_OBJECTS:yaml> ${SDL2_framerate_source} ${public_headers}
)

set_target_properties(gear2d-objects PROPERTIES COMPILE_FLAGS "-Dgear2d_EXPORTS -Dlogtrace_build_dll")

# link gear2d lib against SDL2
target_link_libraries(gear2d ${SDL2_LIBRARIES})

# engine executable, linked against the library
add_executable(main main.cc)
target_link_libraries(main gear2d)
set_target_properties(main PROPERTIES OUTPUT_NAME gear2d BUILD_WITH_INSTALL_RPATH true)

# include dirs so we can #include properly
include_directories(
  ${CMAKE_SOURCE_DIR}/thirdparty/yaml-0.1.4/include/
  ${CMAKE_SOURCE_DIR}/thirdparty/SDL2_framerate/
  ${SDL2_INCLUDE_DIR}
)

# install rules for the executable
install(TARGETS gear2d gear2d-static
  EXPORT gear2d-targets
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

install(TARGETS main
  RUNTIME DESTINATION bin
  LIBRARY DESTINATION lib
  ARCHIVE DESTINATION lib
)

# install rules for the headers
install(FILES ${public_headers} DESTINATION include/gear2d)

# install main.cc source file in case of static linking
install(FILES ${CMAKE_SOURCE_DIR}/src/main.cc DESTINATION share/${PROJECT_NAME})
