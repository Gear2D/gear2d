find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Boost REQUIRED)

add_library(space2d MODULE space2d.cc)
add_library(kinetic2d MODULE kinetic2d.cc)
add_library(renderer MODULE renderer.cc)
add_library(keyboard MODULE keyboard.cc)
add_library(charactercontroller MODULE charactercontroller.cc)
add_library(collider2d MODULE collider2d.cc)
add_library(rigidbody2d MODULE rigidbody2d.cc)
add_library(audiosource MODULE audiosource.cc)

set (COMPONENT_INSTALL_DIR lib/gear2d/component)

include_directories(../ ${SDL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
install(TARGETS space2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/spatial)

install(TARGETS kinetic2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/kinetics)

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer)

install(TARGETS keyboard
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/keyboard)

install(TARGETS charactercontroller
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/controller)

install(TARGETS collider2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/collider)

install(TARGETS rigidbody2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/dynamics)

install(TARGETS audiosource
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/audiosource)

target_link_libraries(keyboard SDL)
target_link_libraries(renderer SDL SDL_image SDL_gfx SDL_ttf)
target_link_libraries(audiosource SDL SDL_mixer)