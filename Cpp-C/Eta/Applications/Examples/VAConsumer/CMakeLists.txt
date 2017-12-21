cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  rsslConsumer.c            rsslDictionaryHandler.c        rsslDirectoryHandler.c
  rsslLoginConsumer.c       rsslMarketByOrderHandler.c     rsslMarketByPriceHandler.c
  rsslMarketPriceHandler.c  rsslPostHandler.c              rsslSymbolListHandler.c
  rsslYieldCurveHandler.c
  ../VACommon/queueMsgHandler.c        ../VACommon/rsslVACacheHandler.c
  ../VACommon/rsslVAMarketPriceItems.c ../VACommon/simpleTunnelMsgHandler.c
  ../VACommon/tunnelStreamHandler.c    ../VACommon/rsslVASendMessage.c
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Cache  ${ETA_HOME}/Include/Codec
  ${ETA_HOME}/Include/RDM  ${ETA_HOME}/Include/Util  ${ETA_HOME}/Include/Reactor
  ../VACommon
  )

add_executable( VAConsumer_shared ${SOURCE_FILES} )
add_executable( VAConsumer_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES ${SYSTEM_LIBRARIES} ${RSSLVACACHE} stdc++ )
    target_link_libraries( VAConsumer_static librsslReactor librsslRDM librsslVAUtil ${EXAMPLE_LIBRARIES_STATIC} ${OTHER_LIBRARIES} )
	set_target_properties( VAConsumer_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( VAConsumer_static PROPERTIES OUTPUT_NAME VAConsumer )
    target_link_libraries( VAConsumer_shared ${RSSLVACACHE_SHARED} librsslVA_shared ${EXAMPLE_LIBRARIES_SHARED}  ${OTHER_LIBRARIES} )
	set_target_properties( VAConsumer_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( VAConsumer_shared PROPERTIES OUTPUT_NAME VAConsumer )

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( VAConsumer_static ${EXAMPLE_STATIC_LIBRARIES} librsslReactor librsslRDM librsslVAUtil )
  target_link_libraries( VAConsumer_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( VAConsumer_shared ${EXAMPLE_SHARED_LIBRARIES} librsslVA )
  target_link_libraries( VAConsumer_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )
endif()
