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

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( EncDecExample_shared ${SOURCE_FILES} )
endif()
add_executable( EncDecExample${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( EncDecExample ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( EncDecExample_static PROPERTIES PROJECT_LABEL "EncDecExample_static (ETA)" )
  set_target_properties( EncDecExample_shared PROPERTIES PROJECT_LABEL "EncDecExample_shared (ETA)" )
  target_link_libraries( EncDecExample_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( EncDecExample_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( EncDecExample_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
endif()
