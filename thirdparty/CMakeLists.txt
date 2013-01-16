include(ExternalProject)
ExternalProject_Add(
  yaml
  SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/yaml-0.1.4/
  CONFIGURE_COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/yaml-0.1.4/configure --prefix=${CMAKE_BINARY_DIR}/yaml-0.1.4/ --enable-shared=no --with-pic
  PREFIX ${CMAKE_BINARY_DIR}/yaml-0.1.4/
  BUILD_COMMAND make
)