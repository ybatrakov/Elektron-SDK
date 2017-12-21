cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPAConsumer_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )


add_executable( ConsMod1b_shared ${SOURCE_FILES} )
add_executable( ConsMod1b_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    target_link_libraries( ConsMod1b_static librssl_static ${SYSTEM_LIBRARIES} )
	set_target_properties( ConsMod1b_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( ConsMod1b_static PROPERTIES OUTPUT_NAME ConsMod1b )
    target_link_libraries( ConsMod1b_shared librssl_shared ${SYSTEM_LIBRARIES} )
	set_target_properties( ConsMod1b_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( ConsMod1b_shared PROPERTIES OUTPUT_NAME ConsMod1b )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( ConsMod1b_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( ConsMod1b_shared ${EXAMPLE_SHARED_LIBRARIES} )
endif()
