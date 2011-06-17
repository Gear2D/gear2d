cmake_minimum_required(VERSION 2.6)
project(Gear2D)

file(GLOB win32dlls "*.dll")

install(FILES ${win32dlls}
		DESTINATION bin)
