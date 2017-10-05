cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( IProvider320_shared IProvider.cpp )
endif()

add_executable( IProvider320${STATIC_EXT} IProvider.cpp )

if (WIN32)
  target_link_libraries( IProvider320_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( IProvider320_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( IProvider320 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
