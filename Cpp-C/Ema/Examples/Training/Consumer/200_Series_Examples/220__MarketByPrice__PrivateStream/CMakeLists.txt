cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( Consumer220_shared Consumer.cpp )
endif()

add_executable( Consumer220${STATIC_EXT} Consumer.cpp )

if (WIN32)
  target_link_libraries( Consumer220_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( Consumer220_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( Consumer220 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
