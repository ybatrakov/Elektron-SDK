cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  transportPerfConfig.c       transportThreads.c   upacTransportPerf.c
  ../Common/channelHandler.c  ../Common/getTime.c  ../Common/latencyRandomArray.c
  ../Common/statistics.c
  )
include_directories(
  ${ETA_HOME}/Include/Cache    ${ETA_HOME}/Include/Codec      ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Reactor  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Util
  ../Common
  )

add_executable( TransportPerf_shared ${SOURCE_FILES} )
add_executable( TransportPerf_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set(OTHER_LIBRARIES ${SYSTEM_LIBRARIES} )
    target_link_libraries( TransportPerf_static librsslReactor librsslRDM librsslVAUtil ${EXAMPLE_LIBRARIES_STATIC} ${OTHER_LIBRARIES} )
	set_target_properties( TransportPerf_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( TransportPerf_static PROPERTIES OUTPUT_NAME TransportPerf )
    target_link_libraries( TransportPerf_shared ${RSSLVACACHE_SHARED} librsslVA_shared ${EXAMPLE_LIBRARIES_SHARED} ${OTHER_LIBRARIES} )
	set_target_properties( TransportPerf_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( TransportPerf_shared PROPERTIES OUTPUT_NAME TransportPerf )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( TransportPerf_static ${EXAMPLE_STATIC_LIBRARIES} librsslReactor librsslRDM librsslVAUtil psapi )
  target_link_libraries( TransportPerf_static debug ${RSSL_VACACHE_DEBUG_STATIC} optimized ${RSSL_VACACHE_RELEASE_STATIC} )

  target_link_libraries( TransportPerf_shared ${EXAMPLE_SHARED_LIBRARIES} librsslVA psapi )
  target_link_libraries( TransportPerf_shared debug ${RSSL_VACACHE_DEBUG_SHARED} optimized ${RSSL_VACACHE_RELEASE_SHARED} )
endif()
