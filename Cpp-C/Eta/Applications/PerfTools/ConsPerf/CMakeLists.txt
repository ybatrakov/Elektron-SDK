cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  consPerfConfig.c                  consumerThreads.c               marketByOrderDecoder.c
  marketPriceDecoder.c              upacConsPerf.c                  ../Common/channelHandler.c
  ../Common/getTime.c               ../Common/itemEncoder.c         ../Common/latencyRandomArray.c
  ../Common/marketByOrderEncoder.c  ../Common/marketPriceEncoder.c  ../Common/statistics.c
  ../Common/xmlItemListParser.c     ../Common/xmlMsgDataParser.c
  )
include_directories(
  ${ETA_HOME}/Include/Cache          ${ETA_HOME}/Include/Codec      ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Reactor        ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Util
  ${ETA_HOME}/Utils/Libxml2/include  ../Common
  )

add_executable( ConsPerf_shared ${SOURCE_FILES} )
add_executable( ConsPerf_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES ${SYSTEM_LIBRARIES} libxml2 )
    target_link_libraries( ConsPerf_static librsslReactor librsslRDM librsslVAUtil ${EXAMPLE_LIBRARIES_STATIC} ${OTHER_LIBRARIES} )
	set_target_properties( ConsPerf_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( ConsPerf_static PROPERTIES OUTPUT_NAME ConsPerf )
    target_link_libraries( ConsPerf_shared ${RSSLVACACHE_SHARED} librsslVA_shared ${EXAMPLE_LIBRARIES_SHARED} ${OTHER_LIBRARIES} )
	set_target_properties( ConsPerf_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( ConsPerf_shared PROPERTIES OUTPUT_NAME ConsPerf )

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( ConsPerf_static ${EXAMPLE_STATIC_LIBRARIES} librsslReactor librsslRDM librsslVAUtil libxml2_static psapi )
  target_link_libraries( ConsPerf_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( ConsPerf_shared ${EXAMPLE_SHARED_LIBRARIES} librsslVA libxml2_static psapi )
  target_link_libraries( ConsPerf_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )

endif()
