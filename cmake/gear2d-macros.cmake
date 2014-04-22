# Set of macro helpers to create and manage gear2d components

include(CMakeParseArguments)


function(G2DCOMPONENT)
  set(oneValueArgs FAMILY TYPE)
  set(multiValueArgs SOURCES)
  cmake_parse_arguments(G2D "" "${oneValueArgs}" "${multiValueArgs}"  ${ARGN})
  
  if(CCF_UNPARSED_ARGUMENTS)
    message(FATAL_ERROR "Unknown keywords given to G2DCOMPONENT(): \"${G2D_UNPARSED_ARGUMENTS}\"")
  endif()
  
  if(NOT G2D_TYPE)
    message(FATAL_ERROR "No TYPE was passed to G2DCOMPONENT(). Please specify a component type using TYPE")
  endif()
  
  if (NOT G2D_FAMILY)
    set(G2D_FAMILY ${G2D_TYPE})
  endif()
  message(STATUS "Component ${G2D_FAMILY}/${G2D_TYPE} from sources ${G2D_SOURCES}")
  add_library(${G2D_TYPE} MODULE ${G2D_SOURCES})
  add_library(${G2D_TYPE}-static STATIC ${G2D_SOURCES})
  include_directories(${GEAR2D_INCLUDE_DIR})
  target_link_libraries(${G2D_TYPE} ${GEAR2D_LIBRARY})
  install(TARGETS ${G2D_TYPE} ${G2D_TYPE}-static LIBRARY DESTINATION ${Gear2D_COMPONENT_PREFIX}/${G2D_FAMILY} ARCHIVE DESTINATION ${Gear2D_COMPONENT_PREFIX}/${G2D_FAMILY})
endfunction()