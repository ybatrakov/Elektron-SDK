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

add_executable( ProvPerf_shared ${SOURCE_FILES} )
add_executable( ProvPerf_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES ${SYSTEM_LIBRARIES} libxml2 )
    target_link_libraries( ProvPerf_static librsslReactor librsslRDM librsslVAUtil ${EXAMPLE_LIBRARIES_STATIC} ${OTHER_LIBRARIES} )
	set_target_properties( ProvPerf_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( ProvPerf_static PROPERTIES OUTPUT_NAME ProvPerf )
    target_link_libraries( ProvPerf_shared ${RSSLVACACHE_SHARED} librsslVA_shared ${EXAMPLE_LIBRARIES_SHARED} ${OTHER_LIBRARIES} )
	set_target_properties( ProvPerf_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( ProvPerf_shared PROPERTIES OUTPUT_NAME ProvPerf )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( ProvPerf_static ${EXAMPLE_STATIC_LIBRARIES} librsslReactor librsslRDM librsslVAUtil libxml2_static psapi )
  target_link_libraries( ProvPerf_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( ProvPerf_shared ${EXAMPLE_SHARED_LIBRARIES} librsslVA libxml2_static psapi )
  target_link_libraries( ProvPerf_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )
endif()
