cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( IProvider140_shared IProvider.cpp )
endif()

add_executable( IProvider140${STATIC_EXT} IProvider.cpp )

if (WIN32)
  target_link_libraries( IProvider140_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( IProvider140_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( IProvider140 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
