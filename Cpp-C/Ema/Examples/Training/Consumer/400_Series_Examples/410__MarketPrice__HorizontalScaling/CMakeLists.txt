cmake_minimum_required (VERSION 3.8)

if (WIN32)
  set( STATIC_EXT "_static" )
  add_executable( Consumer410_shared Consumer.cpp ConsumerManager.cpp )
endif()

add_executable( Consumer410${STATIC_EXT} Consumer.cpp ConsumerManager.cpp )

if (WIN32)
  target_link_libraries( Consumer410_static ${EXAMPLE_STATIC_LIBRARIES} )
  target_link_libraries( Consumer410_shared ${EXAMPLE_SHARED_LIBRARIES} )
else()
  target_link_libraries( Consumer410 ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
