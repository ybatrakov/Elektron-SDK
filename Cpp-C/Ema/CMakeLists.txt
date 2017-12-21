cmake_minimum_required (VERSION 3.8)

# output directory for examples
set ( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EMA_EXAMPLES_OUTPUT_DIR} )

# windows. Supported builds include Debug_MDd, Release_MD
# arguments from esdk dev build, except no RDEV args
if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /D __EMA_COPY_ON_SET__ " )
  set( "CMAKE_CXX_FLAGS_DEBUG_MDD_SHARED" "${CMAKE_CXX_FLAGS_DEBUG_MDD} /D EMA_LIBRARY_EXPORTS" )

  # set ETA library path names
  STRING( REGEX REPLACE "v" "WIN_64_VS" VSSTR "${CMAKE_VS_PLATFORM_TOOLSET}" )
endif()

include_directories( Src/Include )
include_directories( Src )

add_subdirectory( Src/Access )
add_subdirectory( Examples )
