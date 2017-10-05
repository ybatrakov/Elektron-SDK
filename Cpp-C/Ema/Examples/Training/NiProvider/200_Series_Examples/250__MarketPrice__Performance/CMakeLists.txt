cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( NiProvider250_shared NiProvider.cpp )
endif()

add_executable( NiProvider250${STATIC_EXT} NiProvider.cpp )

if (WIN32)
  target_link_libraries( NiProvider250_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( NiProvider250_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( NiProvider250 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
