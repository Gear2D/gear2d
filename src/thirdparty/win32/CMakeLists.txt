cmake_minimum_required(VERSION 2.6)
project(gear2d)

file(GLOB win32dlls "*.dll")

install(FILES ${win32dlls}
		DESTINATION bin)
