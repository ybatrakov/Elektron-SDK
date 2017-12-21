cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  rsslConsumer.c           rsslDictionaryHandler.c        rsslDirectoryHandler.c
  rsslMarketPriceHandler.c rsslMarketByOrderHandler.c     rsslMarketByPriceHandler.c
  rsslSymbolListHandler.c  rsslYieldCurveHandler.c        rsslPostHandler.c
  ../Common/rsslDirectoryEncodeDecode.c	../Common/rsslLoginConsumer.c
  ../Common/rsslLoginEncodeDecode.c     ../Common/rsslSendMessage.c
  ../Common/rsslMarketPriceItems.c
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Codec  ../Common
  )

add_executable( Consumer_shared ${SOURCE_FILES} )
add_executable( Consumer_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( Consumer_static ${EXAMPLE_LIBRARIES_STATIC} ${SYSTEM_LIBRARIES} )
  set_target_properties( Consumer_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
  set_target_properties( Consumer_static PROPERTIES OUTPUT_NAME Consumer )
  target_link_libraries( Consumer_shared ${EXAMPLE_LIBRARIES_SHARED} ${SYSTEM_LIBRARIES} )
  set_target_properties( Consumer_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
  set_target_properties( Consumer_shared PROPERTIES OUTPUT_NAME Consumer )

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( Consumer_static PROPERTIES PROJECT_LABEL "Consumer_static (ETA)" )
  set_target_properties( Consumer_shared PROPERTIES PROJECT_LABEL "Consumer_shared (ETA)" )
  target_link_libraries( Consumer_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( Consumer_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()
