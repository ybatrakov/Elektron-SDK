cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  dictionaryProvider.c              loginProvider.c                 provPerfConfig.c
  upacProvPerf.c                    ../Common/channelHandler.c      ../Common/directoryProvider.c
  ../Common/getTime.c               ../Common/itemEncoder.c         ../Common/latencyRandomArray.c
  ../Common/marketByOrderEncoder.c  ../Common/marketPriceEncoder.c  ../Common/providerThreads.c
  ../Common/statistics.c            ../Common/xmlItemListParser.c   ../Common/xmlMsgDataParser.c
  )
include_directories(
  ${ETA_HOME}/Include/Cache    ${ETA_HOME}/Include/Codec      ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Reactor  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Util
  ${ETA_HOME}/Utils/Libxml2/include  ../Common                ../NIProvPerf
  ../ProvPerf
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( ProvPerf_shared ${SOURCE_FILES} )
endif()
add_executable( ProvPerf${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} libxml2 )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    target_link_libraries( ProvPerf librsslReactor librsslRDM librsslVAUtil ${OTHER_LIBRARIES} )
  elseif ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    target_link_libraries( ProvPerf ${RSSLVACACHE_SHARED} librsslVA ${OTHER_LIBRARIES} )
  endif()
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( ProvPerf_static PROPERTIES PROJECT_LABEL "ProvPerf_static (ETA)" )
  set_target_properties( ProvPerf_shared PROPERTIES PROJECT_LABEL "ProvPerf_shared (ETA)" )

  target_link_libraries( ProvPerf_static ${EXAMPLE_LIBRARIES} librsslReactor librsslRDM librsslVAUtil )
  target_link_libraries( ProvPerf_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( ProvPerf_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( ProvPerf_shared ${EXAMPLE_LIBRARIES} librsslVA )
  target_link_libraries( ProvPerf_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
  target_link_libraries( ProvPerf_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )
endif()
