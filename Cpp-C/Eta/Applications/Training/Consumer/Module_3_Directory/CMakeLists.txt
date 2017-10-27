cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPAConsumer_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( ConsMod3_shared ${SOURCE_FILES} )
endif()
add_executable( ConsMod3${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    target_link_libraries( ConsMod3 ${RSSL} ${SYSTEM_LIBRARIES} )
  elseif ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    target_link_libraries( ConsMod3 ${RSSL} ${SYSTEM_LIBRARIES} )
  endif()
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( ConsMod3_static PROPERTIES PROJECT_LABEL "ConsMod3_static (ETA)" )
  set_target_properties( ConsMod3_shared PROPERTIES PROJECT_LABEL "ConsMod3_shared (ETA)" )

  target_link_libraries( ConsMod3_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( ConsMod3_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( ConsMod3_shared ${EXAMPLE_LIBRARIES} )
  target_link_libraries( ConsMod3_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
endif()
