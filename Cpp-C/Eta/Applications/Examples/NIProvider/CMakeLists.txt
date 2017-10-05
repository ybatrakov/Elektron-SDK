cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  rsslDirectoryHandler.c             rsslItemHandler.c
  rsslNIProvider.c
  ../Common/rsslDictionaryProvider.c ../Common/rsslDirectoryEncodeDecode.c
  ../Common/rsslItemEncode.c         ../Common/rsslLoginConsumer.c
  ../Common/rsslLoginEncodeDecode.c  ../Common/rsslMarketByOrderItems.c
  ../Common/rsslMarketPriceItems.c   ../Common/rsslSendMessage.c 
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Codec  ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Util  ../Common
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( NIProvider_shared ${SOURCE_FILES} )
endif()
add_executable( NIProvider${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( NIProvider ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( NIProvider_static PROPERTIES PROJECT_LABEL "NIProvider_static (ETA)" )
  set_target_properties( NIProvider_shared PROPERTIES PROJECT_LABEL "NIProvider_shared (ETA)" )
  target_link_libraries( NIProvider_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( NIProvider_shared ${EXAMPLE_LIBRARIES} )
  target_link_libraries( NIProvider_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( NIProvider_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
endif()

