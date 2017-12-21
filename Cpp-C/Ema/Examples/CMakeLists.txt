cmake_minimum_required (VERSION 3.8)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set( SYSTEM_LIBRARIES rt dl )
  set( EXAMPLE_LIBRARIES_STATIC libema_static librssl_static )
  set( EXAMPLE_LIBRARIES_SHARED libema_shared librssl_shared )
elseif(WIN32)
  set( EXAMPLE_STATIC_LIBRARIES libema_static wininet.lib ws2_32.lib )
  set( EXAMPLE_SHARED_LIBRARIES libema_shared wininet.lib ws2_32.lib )
endif()

add_subdirectory( Training ) 
add_subdirectory( PerfTools )

