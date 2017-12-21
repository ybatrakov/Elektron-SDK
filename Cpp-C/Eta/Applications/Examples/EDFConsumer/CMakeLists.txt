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

add_executable( EDFConsumer_shared ${SOURCE_FILES} )
add_executable( EDFConsumer_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( EDFConsumer_static librsslRDM librsslVAUtil ${EXAMPLE_LIBRARIES_STATIC} ${SYSTEM_LIBRARIES} ${EXAMPLE_LIBRARIES} )
  set_target_properties( EDFConsumer_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
  set_target_properties( EDFConsumer_static PROPERTIES OUTPUT_NAME EDFConsumer )
  target_link_libraries( EDFConsumer_shared ${EXAMPLE_LIBRARIES_SHARED} librsslVA ${SYSTEM_LIBRARIES} )
  set_target_properties( EDFConsumer_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
  set_target_properties( EDFConsumer_shared PROPERTIES OUTPUT_NAME EDFConsumer )

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  target_link_libraries( EDFConsumer_static librsslRDM librsslVAUtil )
  target_link_libraries( EDFConsumer_shared librsslVA )
  target_link_libraries( EDFConsumer_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( EDFConsumer_static ${EXAMPLE_STATIC_LIBRARIES} )
endif()
