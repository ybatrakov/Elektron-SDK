cmake_minimum_required (VERSION 3.8)

# output directory for examples
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Eta/executables)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( "CMAKE_SHARED_LINKER_FLAGS_DEBUG_MDD" ${CMAKE_SHARED_LINKER_FLAGS} )
  set( "CMAKE_SHARED_LINKER_FLAGS_RELEASE_MD" "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD" )


  set( CMAKE_FIND_LIBRARY_PREFIXES "lib" )
  find_library( RSSL_DEBUG rssl HINTS ${BINARY_PACK_LOCATION}/Debug_MDd )
  find_library( RSSL_RELEASE rssl HINTS ${BINARY_PACK_LOCATION}/Release_MD )
  set( RSSL_DEBUG_STATIC ${RSSL_DEBUG} )
  set( RSSL_RELEASE_STATIC ${RSSL_RELEASE} )
  find_library( RSSL_DEBUG_SHARED rssl HINTS ${BINARY_PACK_LOCATION}/Debug_MDd/Shared )
  find_library( RSSL_RELEASE_SHARED rssl HINTS ${BINARY_PACK_LOCATION}/Release_MD/Shared )

  set( MISSING_LIBRARIES "" )
  foreach( lib RSSL_DEBUG RSSL_RELEASE RSSL_DEBUG_STATIC RSSL_RELEASE_STATIC RSSL_DEBUG_SHARED RSSL_RELEASE_SHARED)
    if ( NOT ${lib} )
      set( MISSING_LIBRARIES "${MISSING_LIBRARIES} ${lib}" )
    endif()
  endforeach()

  if ( MISSING_LIBRARIES )
    message( FATAL_ERROR "one or more necessary libraries [${MISSING_LIBRARIES}] were not found" )
  endif()

endif()

add_subdirectory( Impl )

# libxml2
add_subdirectory( Utils/Libxml2 )

add_subdirectory( Applications )
