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
add_subdirectory(src)
add_subdirectory(doc)
