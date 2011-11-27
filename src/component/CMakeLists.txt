find_package(SDL REQUIRED)
find_package(SDL_image REQUIRED)
find_package(SDL_mixer REQUIRED)
find_package(SDL_ttf REQUIRED)
find_package(Boost REQUIRED)

add_library(space2d MODULE space2d.cc)
add_library(kinematic2d MODULE kinematic2d.cc)
add_library(renderer MODULE renderer.cc)
add_library(keyboard MODULE keyboard.cc)
add_library(mouse MODULE mouse.cc)
add_library(mouseover MODULE mouseover.cc)
add_library(charactercontroller MODULE charactercontroller.cc)
#add_library(moveto MODULE moveto.cc)
add_library(collider2d MODULE collider2d.cc)
add_library(rigidbody2d MODULE rigidbody2d.cc)
add_library(audiosource MODULE audiosource.cc)
add_library(singleselect MODULE singleselect.cc)
add_library(spritesheet MODULE spritesheet.cc)
add_library(bullet MODULE bullet.cc)

set (COMPONENT_INSTALL_DIR lib/gear2d/component)

include_directories(../ ../thirdparty/bullet-2.78/src ${SDL_INCLUDE_DIR} ${Boost_INCLUDE_DIRS})

install(TARGETS space2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/spatial
)

install(TARGETS kinematic2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/kinematics
)

install(TARGETS renderer
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/renderer
)

install(TARGETS keyboard
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/keyboard
)

install(TARGETS mouse
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/mouse
)

install(TARGETS mouseover
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/mouseover
)

install(TARGETS charactercontroller
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/controller
)

install(TARGETS collider2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/collider
)

install(TARGETS rigidbody2d
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/dynamics
)

install(TARGETS bullet
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/simulation
)

install(TARGETS audiosource
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/audiosource
)

install(TARGETS singleselect
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/menu
)

install(TARGETS spritesheet
		LIBRARY DESTINATION ${COMPONENT_INSTALL_DIR}/animation
)

target_link_libraries(mouseover gear2d)
target_link_libraries(keyboard SDL gear2d)
target_link_libraries(mouse SDL gear2d)
target_link_libraries(renderer SDL SDL_image SDL_ttf SDL_gfx gear2d)
target_link_libraries(audiosource SDL SDL_mixer gear2d)
target_link_libraries(space2d gear2d)
target_link_libraries(kinematic2d gear2d)
target_link_libraries(charactercontroller gear2d)
target_link_libraries(collider2d gear2d)
target_link_libraries(rigidbody2d gear2d)
target_link_libraries(bullet gear2d BulletDynamics BulletCollision LinearMath)
target_link_libraries(singleselect gear2d)
target_link_libraries(spritesheet gear2d)

#target_link_libraries(moveto gear2d)

# WARNING Remember to add those components here too

#	keyboard render kinematic2d space2d)

