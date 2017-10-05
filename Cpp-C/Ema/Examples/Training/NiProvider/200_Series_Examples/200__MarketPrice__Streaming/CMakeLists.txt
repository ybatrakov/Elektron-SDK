cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( NiProvider200_shared NiProvider.cpp )
endif()

add_executable( NiProvider200${STATIC_EXT} NiProvider.cpp )

if (WIN32)
  target_link_libraries( NiProvider200_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( NiProvider200_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( NiProvider200 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
