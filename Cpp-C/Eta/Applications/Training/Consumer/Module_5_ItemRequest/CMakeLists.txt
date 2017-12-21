cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPAConsumer_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

add_executable( ConsMod5_shared ${SOURCE_FILES} )
add_executable( ConsMod5_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )	
	target_link_libraries( ConsMod5_static librssl_static ${SYSTEM_LIBRARIES} )
	set_target_properties( ConsMod5_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( ConsMod5_static PROPERTIES OUTPUT_NAME ConsMod5 )
    target_link_libraries( ConsMod5_shared librssl_shared ${SYSTEM_LIBRARIES} )
	set_target_properties( ConsMod5_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( ConsMod5_shared PROPERTIES OUTPUT_NAME ConsMod5 )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( ConsMod5_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( ConsMod5_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()
