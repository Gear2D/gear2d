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
  message(STATUS "Trying windoze $ENV{PROGRAMFILES}")
  find_path(Gear2D_INCLUDE_DIR gear2d.h
    HINTS
    $ENV{GEAR2DDIR}
    PATHS
    "$ENV{PROGRAMFILES}\\Gear2D\\include"
    "$ENV{PROGRAMFILES(X86)}\\Gear2D\\include"
    "C:\\Gear2D\\include"
    "$ENV{PROGRAMFILES}\\Gear2D\\include\\gear2d"
    "$ENV{PROGRAMFILES(X86)}\\Gear2D\\include\\gear2d"
    "C:\\Gear2D\\include\\gear2d"
  )
  
  find_library(
    Gear2D_LIBRARY gear2d
    HINTS
    $ENV{GEAR2DDIR}
    PATHS
    "$ENV{PROGRAMFILES}\\Gear2D\\"
    "$ENV{PROGRAMFILES(X86)}\\Gear2D\\"
    "C:\\Gear2D\\"
    PATH_SUFFIXES lib bin
  )
  
  find_program(
    Gear2D_EXECUTABLE gear2d.exe
    HINTS
    $ENV{GEAR2DDIR}
    PATHS
    "$ENV{PROGRAMFILES}\\Gear2D\\bin"
    "$ENV{PROGRAMFILES(x86)}\\Gear2D\\bin"
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
  
  find_program(
    Gear2D_EXECUTABLE gear2d
    HINTS
    $ENV{GEAR2DDIR}
    PATH_SUFFIXES bin
    PATHS
    /usr/local/
    /usr/
    /opt/local/
    /opt
  )
endif(WIN32)



IF(Gear2D_INCLUDE_DIR)
  IF(Gear2D_LIBRARY)
    SET(Gear2D_FOUND TRUE)
  ELSE(Gear2D_LIBRARY)
    message(ERROR "Missing Gear2D_LIBRARY")
  ENDIF(Gear2D_LIBRARY)
ELSE(Gear2D_INCLUDE_DIR)
  message(FATAL_ERROR "Missing Gear2D_INCLUDE_DIR")
ENDIF(Gear2D_INCLUDE_DIR)

IF(Gear2D_FOUND)
  get_filename_component(Gear2D_LINK_DIR ${Gear2D_LIBRARY} PATH)
    execute_process(COMMAND ${Gear2D_EXECUTABLE} -v OUTPUT_VARIABLE Gear2D_VERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
  SET(Gear2D_COMPONENT_PREFIX ${Gear2D_LINK_DIR}/../lib/gear2d/component)
  IF (NOT Gear2D_FIND_QUIETLY)
    MESSAGE(STATUS "Found Gear2D: ${Gear2D_LIBRARY} version ${Gear2D_VERSION}")
    MESSAGE(STATUS "Guessed Gear2D component prefix: ${Gear2D_COMPONENT_PREFIX}")
  ENDIF (NOT Gear2D_FIND_QUIETLY)
ELSE(Gear2D_FOUND)
  IF (Gear2D_FIND_REQUIRED)
    MESSAGE(FATAL_ERROR "Could not find Gear2D.")
  ENDIF (Gear2D_FIND_REQUIRED)
ENDIF(Gear2D_FOUND)

ENDIF(NOT Gear2D_FOUND)