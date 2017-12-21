cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  itemDecoder.c        postHandler.c        rsslWatchlistConsumer.c
  watchlistConsumerConfig.c
  ../VACommon/simpleTunnelMsgHandler.c        ../VACommon/tunnelStreamHandler.c
  ../VACommon/queueMsgHandler.c
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Cache  ${ETA_HOME}/Include/Codec
  ${ETA_HOME}/Include/RDM  ${ETA_HOME}/Include/Util  ${ETA_HOME}/Include/Reactor
  ../VACommon
  )

add_executable( WatchlistConsumer_shared ${SOURCE_FILES} )
add_executable( WatchlistConsumer_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES  ${SYSTEM_LIBRARIES} stdc++ )
    target_link_libraries( WatchlistConsumer_static librsslReactor librsslRDM librsslVAUtil ${EXAMPLE_LIBRARIES_STATIC} ${OTHER_LIBRARIES} )
	set_target_properties( WatchlistConsumer_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( WatchlistConsumer_static PROPERTIES OUTPUT_NAME WatchlistConsumer )
    target_link_libraries( WatchlistConsumer_shared ${RSSLVACACHE_SHARED} librsslVA_shared ${EXAMPLE_LIBRARIES_SHARED} ${OTHER_LIBRARIES} )
		set_target_properties( WatchlistConsumer_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( WatchlistConsumer_shared PROPERTIES OUTPUT_NAME WatchlistConsumer )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( WatchlistConsumer_static ${EXAMPLE_STATIC_LIBRARIES} librsslReactor librsslRDM librsslVAUtil )
  target_link_libraries( WatchlistConsumer_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( WatchlistConsumer_shared ${EXAMPLE_SHARED_LIBRARIES} librsslVA )
  target_link_libraries( WatchlistConsumer_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )
endif()
