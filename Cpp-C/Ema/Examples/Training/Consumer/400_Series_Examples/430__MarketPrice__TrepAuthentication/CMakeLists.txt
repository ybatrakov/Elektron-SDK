cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( Consumer430_shared Consumer.cpp )
endif()

add_executable( Consumer430${STATIC_EXT} Consumer.cpp )

if (WIN32)
  target_link_libraries( Consumer430_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( Consumer430_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( Consumer430 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
