cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( NiProvider100_shared NiProvider.cpp )
endif()

add_executable( NiProvider100${STATIC_EXT} NiProvider.cpp )

if (WIN32)
  target_link_libraries( NiProvider100_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( NiProvider100_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( NiProvider100 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
