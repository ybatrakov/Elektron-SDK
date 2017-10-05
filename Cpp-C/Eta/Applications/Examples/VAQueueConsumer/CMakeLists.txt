cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  rsslVAQueueCons.c        rsslLoginConsumer.c        QConsSimpleDirectoryHandler.c
  QConsSimpleQueueMsgHandler
  )

include_directories(
  ${ETA_HOME}/Include/Transport
  ${ETA_HOME}/Include/Cache
  ${ETA_HOME}/Include/Codec
  ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Util
  ${ETA_HOME}/Include/Reactor
  ../VACommon
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( VAQueueConsumer_shared ${SOURCE_FILES} )
endif()
add_executable( VAQueueConsumer${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} stdc++ )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    target_link_libraries( VAQueueConsumer librsslReactor librsslRDM librsslVAUtil ${OTHER_LIBRARIES} )
  elseif ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    target_link_libraries( VAQueueConsumer ${RSSLVACACHE_SHARED} librsslVA ${OTHER_LIBRARIES} )
  endif()
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( VAQueueConsumer_static PROPERTIES PROJECT_LABEL "VAQueueConsumer_static (ETA)" )
  set_target_properties( VAQueueConsumer_shared PROPERTIES PROJECT_LABEL "VAQueueConsumer_shared (ETA)" )

  target_link_libraries( VAQueueConsumer_static ${EXAMPLE_LIBRARIES} librsslReactor librsslRDM librsslVAUtil )
  target_link_libraries( VAQueueConsumer_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( VAQueueConsumer_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( VAQueueConsumer_shared ${EXAMPLE_LIBRARIES} librsslVA )
  target_link_libraries( VAQueueConsumer_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
  target_link_libraries( VAQueueConsumer_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )
endif()


