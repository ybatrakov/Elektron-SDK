cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPANIProvider_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( NIProvMod1b_shared ${SOURCE_FILES} )
endif()
add_executable( NIProvMod1b${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    target_link_libraries( NIProvMod1b ${RSSL} ${SYSTEM_LIBRARIES} )
  elseif ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    target_link_libraries( NIProvMod1b ${RSSL} ${SYSTEM_LIBRARIES} )
  endif()
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( NIProvMod1b_static PROPERTIES PROJECT_LABEL "NIProvMod1b_static (ETA)" )
  set_target_properties( NIProvMod1b_shared PROPERTIES PROJECT_LABEL "NIProvMod1b_shared (ETA)" )

  target_link_libraries( NIProvMod1b_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( NIProvMod1b_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( NIProvMod1b_shared ${EXAMPLE_LIBRARIES} )
  target_link_libraries( NIProvMod1b_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
endif()
