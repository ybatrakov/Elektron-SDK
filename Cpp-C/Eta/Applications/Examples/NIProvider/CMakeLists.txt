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

add_executable( NIProvider_shared ${SOURCE_FILES} )
add_executable( NIProvider_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  target_link_libraries( NIProvider_static ${EXAMPLE_LIBRARIES_STATIC} ${SYSTEM_LIBRARIES} )
  set_target_properties( NIProvider_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
  set_target_properties( NIProvider_static PROPERTIES OUTPUT_NAME NIProvider )
  target_link_libraries( NIProvider_shared ${EXAMPLE_LIBRARIES_SHARED} ${SYSTEM_LIBRARIES} )
  set_target_properties( NIProvider_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
  set_target_properties( NIProvider_shared PROPERTIES OUTPUT_NAME NIProvider )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set_target_properties( NIProvider_static PROPERTIES PROJECT_LABEL "NIProvider_static (ETA)" )
  set_target_properties( NIProvider_shared PROPERTIES PROJECT_LABEL "NIProvider_shared (ETA)" )
  target_link_libraries( NIProvider_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( NIProvider_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()

