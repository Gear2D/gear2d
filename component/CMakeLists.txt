find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)

add_library(dummy MODULE dummy.cc)

add_library(spatial MODULE spatial.cc)

add_library(spin MODULE spin.cc)

add_library(visualgfx MODULE visualgfx.cc)
target_link_libraries(visualgfx SDL SDL_image)

include_directories(../ ${SDL_INCLUDE_DIR})
install(TARGETS spatial dummy visualgfx spin
		LIBRARY DESTINATION lib/gear2d/component)