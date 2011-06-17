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
add_library(moveto MODULE moveto.cc)
add_library(collider2d MODULE collider2d.cc)
add_library(rigidbody2d MODULE rigidbody2d.cc)
add_library(audiosource MODULE audiosource.cc)
add_library(singleselect MODULE singleselect.cc)
add_library(spritesheet MODULE spritesheet.cc)

set (COMPONENT_INSTALL_DIR lib/gear2d/component)

include_directories(../ ${SDL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})
install(TARGETS space2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/spatial
		COMPONENT spatial2d)
set(CPACK_COMPONENT_SPATIAL2D_DEPENDS runtime)
set(CPACK_COMPONENT_SPATIAL2D_GROUP "components")

install(TARGETS kinetic2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/kinetics
		COMPONENT kinetic2d)
set(CPACK_COMPONENT_KINETIC2D_DEPENDS runtime)
set(CPACK_COMPONENT_KINETIC2D_GROUP "components")

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
		COMPONENT renderer)
set(CPACK_COMPONENT_RENDER_DEPENDS runtime)
set(CPACK_COMPONENT_RENDER_GROUP "components")

install(TARGETS keyboard
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/keyboard
		COMPONENT keyboard)
set(CPACK_COMPONENT_KEYBOARD_DEPENDS keyboard)
set(CPACK_COMPONENT_KEYBOARD_GROUP "components")

install(TARGETS charactercontroller
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/controller
		COMPONENT charactercontroller)
set(CPACK_COMPONENT_CHARACTERCONTROLLER_DEPENDS runtime)
set(CPACK_COMPONENT_CHARACTERCONTROLLER_GROUP "components")

# install(TARGETS moveto
# 		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/controller)

install(TARGETS collider2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/collider
		COMPONENT collider2d)
set(CPACK_COMPONENT_COLLIDER2D_DEPENDS runtime)
set(CPACK_COMPONENT_COLLIDER2D_GROUP "components")

install(TARGETS rigidbody2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/dynamics
		COMPONENT rigidbody2d)
set(CPACK_COMPONENT_RIGIDBODY2D_DEPENDS runtime)
set(CPACK_COMPONENT_RIGIDBODY2D_GROUP "components")

install(TARGETS audiosource
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/audiosource
		COMPONENT audiosource)
set(CPACK_COMPONENT_AUDIOSOURCE_DEPENDS runtime)
set(CPACK_COMPONENT_AUDIOSOURCE_GROUP "components")

install(TARGETS singleselect
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/menu
		COMPONENT singleselect)
set(CPACK_COMPONENT_SINGLESELECT_DEPENDS runtime)
set(CPACK_COMPONENT_SINGLESELECT_GROUP "components")

install(TARGETS spritesheet
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/animation
		COMPONENT spritesheet)
set(CPACK_COMPONENT_SPRITESHEET_DEPENDS runtime)
set(CPACK_COMPONENT_SPRITESHEET_GROUP "components")

set(CPACK_COMPONENT_GROUP_COMPONENTS_DISPLAY_NAME "Components")
set(CPACK_COMPONENT_GROUP_COMPONENTS_DESCRIPTION "Base components for your game")


target_link_libraries(keyboard SDL gear2d)
target_link_libraries(renderer SDL SDL_image SDL_ttf gear2d)
target_link_libraries(audiosource SDL SDL_mixer gear2d)
target_link_libraries(space2d gear2d)
target_link_libraries(kinetic2d gear2d)
target_link_libraries(charactercontroller gear2d)
target_link_libraries(collider2d gear2d)
target_link_libraries(rigidbody2d gear2d)
target_link_libraries(singleselect gear2d)
target_link_libraries(spritesheet gear2d)
target_link_libraries(moveto gear2d)

# WARNING Remember to add those components here too
set(CPACK_COMPONENTS_ALL spritesheet singleselect audiosource rigidbody2d collider2d charactercontroller
	keyboard render kinetic2d space2d)

