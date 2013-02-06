cmake_minimum_required(VERSION 2.6)
project(gear2d)

file(GLOB win64dlls "*.dll")

install(FILES ${win64dlls}
		DESTINATION bin)
		
message(STATUS "Using 64bits third party libs")
