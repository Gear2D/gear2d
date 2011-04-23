add_library(dummy MODULE dummy.cc)
add_library(spatial MODULE spatial.cc)
include_directories(../)
install(TARGETS spatial dummy
		LIBRARY DESTINATION lib/gear2d/component)