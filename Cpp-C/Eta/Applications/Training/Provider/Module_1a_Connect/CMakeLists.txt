cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPAProvider_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( ProvMod1a_shared ${SOURCE_FILES} )
endif()
add_executable( ProvMod1a${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    target_link_libraries( ProvMod1a ${RSSL} ${SYSTEM_LIBRARIES} )
  elseif ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    target_link_libraries( ProvMod1a ${RSSL} ${SYSTEM_LIBRARIES} )
  endif()
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( ProvMod1a_static PROPERTIES PROJECT_LABEL "ProvMod1a_static (ETA)" )
  set_target_properties( ProvMod1a_shared PROPERTIES PROJECT_LABEL "ProvMod1a_shared (ETA)" )

  target_link_libraries( ProvMod1a_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( ProvMod1a_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( ProvMod1a_shared ${EXAMPLE_LIBRARIES} )
  target_link_libraries( ProvMod1a_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
endif()
