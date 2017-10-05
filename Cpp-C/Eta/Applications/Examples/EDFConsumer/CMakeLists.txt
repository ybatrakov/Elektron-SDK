cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  edfExampleConfig.c         itemList.c                 msgQueue.c
  realTimeSession.c          rsslMarketByOrderHandler.c rsslMarketPriceHandler.c
  rsslMarketByPriceHandler.c rsslEDFConsumer.c          gapRequestSession.c
  gapFillSession.c           refDataSession.c           snapshotSession.c
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Codec  ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Util  ../Common
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( EDFConsumer_shared ${SOURCE_FILES} )
endif()
add_executable( EDFConsumer${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( EDFConsumer ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} ${EXAMPLE_LIBRARIES} )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    # library order matters
    target_link_libraries( EDFConsumer librsslRDM librsslVAUtil )
  elseif ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    target_link_libraries( EDFConsumer ${EXAMPLE_LIBRARIES} librsslVA ${SYSTEM_LIBRARIES} )
  endif()

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( EDFConsumer_static PROPERTIES PROJECT_LABEL "EDFConsumer_static (ETA)" )
  set_target_properties( EDFConsumer_shared PROPERTIES PROJECT_LABEL "EDFConsumer_shared (ETA)" )
  target_link_libraries( EDFConsumer_static librsslRDM librsslVAUtil )
  target_link_libraries( EDFConsumer_shared librsslVA )
  target_link_libraries( EDFConsumer_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( EDFConsumer_static ${EXAMPLE_LIBRARIES} )
endif()
