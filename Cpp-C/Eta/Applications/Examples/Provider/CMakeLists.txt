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

add_executable( Provider_shared ${SOURCE_FILES} )
add_executable( Provider_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( Provider_static ${EXAMPLE_LIBRARIES_STATIC} ${SYSTEM_LIBRARIES} )
  set_target_properties( Provider_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
  set_target_properties( Provider_static PROPERTIES OUTPUT_NAME NIProvider )
  target_link_libraries( Provider_shared ${EXAMPLE_LIBRARIES_SHARED} ${SYSTEM_LIBRARIES} )
  set_target_properties( Provider_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
  set_target_properties( Provider_shared PROPERTIES OUTPUT_NAME NIProvider )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( Provider_static PROPERTIES PROJECT_LABEL "Provider_static (ETA)" )
  set_target_properties( Provider_shared PROPERTIES PROJECT_LABEL "Provider_shared (ETA)" )
  target_link_libraries( Provider_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( Provider_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()

