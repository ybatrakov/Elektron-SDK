cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  elementListEncDec.c        fieldListEncDec.c        filterListEncDec.c
  mapEncDec.c                msgEncDec.c              rsslEncDecExample.c
  seriesEncDec.c             vectorEncDec.c
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Codec  ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Util  ../Common
  )

add_executable( EncDecExample_shared ${SOURCE_FILES} )
add_executable( EncDecExample_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( EncDecExample_static ${EXAMPLE_LIBRARIES_STATIC} ${SYSTEM_LIBRARIES} )
  set_target_properties( EncDecExample_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
  set_target_properties( EncDecExample_static PROPERTIES OUTPUT_NAME EncDecExample )
  target_link_libraries( EncDecExample_shared ${EXAMPLE_LIBRARIES_SHARED} ${SYSTEM_LIBRARIES} )
  set_target_properties( EncDecExample_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
  set_target_properties( EncDecExample_shared PROPERTIES OUTPUT_NAME EncDecExample )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( EncDecExample_static PROPERTIES PROJECT_LABEL "EncDecExample_static (ETA)" )
  set_target_properties( EncDecExample_shared PROPERTIES PROJECT_LABEL "EncDecExample_shared (ETA)" )
  target_link_libraries( EncDecExample_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( EncDecExample_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()
