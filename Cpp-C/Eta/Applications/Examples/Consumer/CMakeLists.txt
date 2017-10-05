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

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( Consumer_shared ${SOURCE_FILES} )
endif()
add_executable( Consumer${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( Consumer ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( Consumer_static PROPERTIES PROJECT_LABEL "Consumer_static (ETA)" )
  set_target_properties( Consumer_shared PROPERTIES PROJECT_LABEL "Consumer_shared (ETA)" )
  target_link_libraries( Consumer_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( Consumer_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( Consumer_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
  target_link_libraries( Consumer_shared ${EXAMPLE_LIBRARIES} )
endif()

