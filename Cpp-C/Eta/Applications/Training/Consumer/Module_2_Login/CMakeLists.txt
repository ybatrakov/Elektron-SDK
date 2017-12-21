cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPAConsumer_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

add_executable( ConsMod2_shared ${SOURCE_FILES} )
add_executable( ConsMod2_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )    
	target_link_libraries( ConsMod2_static librssl_static ${SYSTEM_LIBRARIES} )
	set_target_properties( ConsMod2_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( ConsMod2_static PROPERTIES OUTPUT_NAME ConsMod2 )
    target_link_libraries( ConsMod2_shared librssl_shared ${SYSTEM_LIBRARIES} )
	set_target_properties( ConsMod2_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( ConsMod2_shared PROPERTIES OUTPUT_NAME ConsMod2 )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( ConsMod2_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( ConsMod2_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()
