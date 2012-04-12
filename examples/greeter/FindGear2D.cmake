# FindGear2D.cmake, version 1.0.1
# Locate Gear2D library
# This module defines
# Gear2D_LIBRARY, the name of the library to link against
# Gear2D_FOUND, if it was found
# Gear2D_VERSION, version of found library
# Gear2D_INCLUDE_DIR, where to find gear2d.h
# Gear2D_LINK_DIR, where to find gear2d.so/gear2d.dll
# Gear2D_COMPONENT_PREFIX, prefix to install components

if (NOT Gear2D_FOUND)

if (WIN32)
	find_path(Gear2D_INCLUDE_DIR gear2d.h
		HINTS
		$ENV{GEAR2DDIR}
		PATHS
		"C:\\$ENV{PROGRAMFILES}\\Gear2D\\include"
		"C:\\$ENV{PROGRAMFILES(X86)}\\Gear2D\\include"
		"C:\\Gear2D\\include"
		"C:\\$ENV{PROGRAMFILES}\\Gear2D\\include\\gear2d"
		"C:\\$ENV{PROGRAMFILES(X86)}\\Gear2D\\include\\gear2d"
		"C:\\Gear2D\\include\\gear2d"
	)
	
	find_library(
		Gear2D_LIBRARY gear2d
		PATHS
		"C:\\$ENV{PROGRAMFILES}\\Gear2D\\bin"
		"C:\\$ENV{PROGRAMFILES(x86)}\\Gear2D\\bin"
		"C:\\Gear2D\\bin"
	)
else ()
	find_path(Gear2D_INCLUDE_DIR gear2d.h
		HINTS
		$ENV{GEAR2DDIR}
		PATH_SUFFIXES include/gear2d include
		PATHS
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local/include/gear2d
		/usr/include/gear2d
		/sw # Fink
		/opt/local # DarwinPorts
		/opt/csw # Blastwave
		/opt
	)
	
	find_library(
		Gear2D_LIBRARY gear2d
		HINTS
		$ENV{GEAR2DDIR}
		PATH_SUFFIXES lib64 lib
		PATHS
		~/Library/Frameworks
		/Library/Frameworks
		/usr/local/lib/gear2d
		/usr/lib/gear2d
		/opt/local/lib
		/opt/lib
		/opt
		/usr/local
		/usr
	)
endif(WIN32)



IF(Gear2D_INCLUDE_DIR AND Gear2D_LIBRARY)
	SET(Gear2D_FOUND TRUE)
ENDIF(Gear2D_INCLUDE_DIR AND Gear2D_LIBRARY)

IF(Gear2D_FOUND)
	get_filename_component(Gear2D_LINK_DIR ${Gear2D_LIBRARY} PATH)
	execute_process(COMMAND gear2d -v OUTPUT_VARIABLE Gear2D_VERSION)
	SET(Gear2D_COMPONENT_PREFIX ${Gear2D_LINK_DIR}/../lib/gear2d/component)
	IF (NOT Gear2D_FIND_QUIETLY)
		MESSAGE(STATUS "Found Gear2D: ${Gear2D_LIBRARY} version ${Gear2D_VERSION}")
		MESSAGE(STATUS "Guessed Gear2D component prefix: ${Gear2D_COMPONENT_PREFIX}")
	ENDIF (NOT Gear2D_FIND_QUIETLY)
ELSE(Gear2D_FOUND)
	IF (Gear2D_FIND_REQUIRED)
		MESSAGE(FATAL_ERROR "Could not find Gear2D")
	ENDIF (Gear2D_FIND_REQUIRED)
ENDIF(Gear2D_FOUND)

ENDIF(NOT Gear2D_FOUND)