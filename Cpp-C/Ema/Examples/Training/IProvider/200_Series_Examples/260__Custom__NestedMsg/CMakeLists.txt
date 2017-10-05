cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( IProvider260_shared IProvider.cpp )
endif()

add_executable( IProvider260${STATIC_EXT} IProvider.cpp )

if (WIN32)
  target_link_libraries( IProvider260_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( IProvider260_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( IProvider260 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
