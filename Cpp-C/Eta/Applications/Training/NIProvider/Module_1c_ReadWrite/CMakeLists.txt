cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPANIProvider_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

add_executable( NIProvMod1c_shared ${SOURCE_FILES} )
add_executable( NIProvMod1c_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )	
	target_link_libraries( NIProvMod1c_static librssl_static ${SYSTEM_LIBRARIES} )
	set_target_properties( NIProvMod1c_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( NIProvMod1c_static PROPERTIES OUTPUT_NAME NIProvMod1c )
    target_link_libraries( NIProvMod1c_shared librssl_shared ${SYSTEM_LIBRARIES} )
	set_target_properties( NIProvMod1c_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( NIProvMod1c_shared PROPERTIES OUTPUT_NAME NIProvMod1c )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( NIProvMod1c_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( NIProvMod1c_shared ${EXAMPLE_STATIC_LIBRARIES} )
endif()
