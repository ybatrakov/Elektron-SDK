cmake_minimum_required (VERSION 3.8)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
  set( SYSTEM_LIBRARIES rt dl )
  set( EXAMPLE_LIBRARIES libema ${RSSL} )
#  if ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" OR
#      ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic" )
#    set( EXAMPLE_LIBRARIES libema ${RSSL} )
#  elseif ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared" OR
#      ${CMAKE_BUILD_TYPE} STREQUAL "DebugShared" )
#    set( EXAMPLE_LIBRARIES libema ${RSSL} )
#  endif()

elseif(WIN32)
  set( EXAMPLE_STATIC_LIBRARIES libema_static wininet.lib ws2_32.lib )
  set( EXAMPLE_SHARED_LIBRARIES libema_shared wininet.lib ws2_32.lib )
endif()

add_subdirectory( Training ) 
add_subdirectory( PerfTools )

