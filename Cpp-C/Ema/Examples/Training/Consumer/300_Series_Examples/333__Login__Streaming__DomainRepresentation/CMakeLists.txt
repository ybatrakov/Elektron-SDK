cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( Consumer333_shared Consumer.cpp )
endif()

add_executable( Consumer333${STATIC_EXT} Consumer.cpp )

if (WIN32)
  target_link_libraries( Consumer333_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( Consumer333_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( Consumer333 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
