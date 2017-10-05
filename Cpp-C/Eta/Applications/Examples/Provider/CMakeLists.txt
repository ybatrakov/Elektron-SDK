cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES
  rsslDirectoryHandler.c        rsslItemHandler.c
  rsslLoginHandler.c            rsslProvider.c
  ../Common/rsslDirectoryEncodeDecode.c        ../Common/rsslItemEncode.c
  ../Common/rsslLoginEncodeDecode.c            ../Common/rsslMarketByOrderItems.c
  ../Common/rsslMarketByPriceItems.c           ../Common/rsslMarketPriceItems.c
  ../Common/rsslSendMessage.c                  ../Common/rsslSymbolListItems.c
  ../Common/rsslDictionaryProvider.c
  )

include_directories(
  ${ETA_HOME}/Include/Transport  ${ETA_HOME}/Include/Codec  ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Util  ../Common
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_executable( Provider_shared ${SOURCE_FILES} )
endif()
add_executable( Provider${STATIC_EXT} ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( Provider ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( Provider_static PROPERTIES PROJECT_LABEL "Provider_static (ETA)" )
  set_target_properties( Provider_shared PROPERTIES PROJECT_LABEL "Provider_shared (ETA)" )
  target_link_libraries( Provider_static ${EXAMPLE_LIBRARIES} )
  target_link_libraries( Provider_shared ${EXAMPLE_LIBRARIES} )
  target_link_libraries( Provider_static debug ${RSSL_DEBUG_STATIC} optimized ${RSSL_RELEASE_STATIC} )
  target_link_libraries( Provider_shared debug ${RSSL_DEBUG_SHARED} optimized ${RSSL_RELEASE_SHARED} )
endif()

