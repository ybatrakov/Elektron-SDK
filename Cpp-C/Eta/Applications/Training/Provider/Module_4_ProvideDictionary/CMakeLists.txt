cmake_minimum_required (VERSION 3.8)

set( SOURCE_FILES UPAProvider_Training.c )
include_directories( ${ETA_HOME}/Include/Codec    ${ETA_HOME}/Include/Transport )

add_executable( ProvMod4_shared ${SOURCE_FILES} )
add_executable( ProvMod4_static ${SOURCE_FILES} )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )	
	target_link_libraries( ProvMod4_static librssl_static ${SYSTEM_LIBRARIES} )
	set_target_properties( ProvMod4_static PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR} )
	set_target_properties( ProvMod4_static PROPERTIES OUTPUT_NAME ProvMod4 )
    target_link_libraries( ProvMod4_shared librssl_shared ${SYSTEM_LIBRARIES} )
	set_target_properties( ProvMod4_shared PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${ETA_EXAMPLES_OUTPUT_DIR}/Shared )
	set_target_properties( ProvMod4_shared PROPERTIES OUTPUT_NAME ProvMod4 )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )

  target_link_libraries( ProvMod4_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( ProvMod4_shared ${EXAMPLE_STATIC_LIBRARIES} )
endif()
