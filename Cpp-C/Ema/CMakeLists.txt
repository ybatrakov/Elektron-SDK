cmake_minimum_required (VERSION 3.8)

# no RPATH for Linux
set( CMAKE_SKIP_RPATH TRUE )

# output directory for examples
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Ema/executables)

# windows. Supported builds include Debug_MDd, Release_MD
# arguments from esdk dev build, except no RDEV args
if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( CMAKE_CXX_FLAGS "/MP /GS /W3 /Zc:wchar_t- /Z7 /Gm- /fp:precise /D WIN32 /D _LIB /D _iso_stdcpp_ /D _WIN32 /D _WIN32_WINNT=0x0501 /D WIN32_LEAN_AND_MEAN /D x86_WindowsNT_5X /D _CRT_SECURE_NO_WARNINGS /D _MBCS /WX- /Zc:forScope /Gd /Oi /EHsc /nologo  /D __EMA_COPY_ON_SET__  /D LIBXML_STATIC /D COMPILE_64BITS" )

  set( "CMAKE_CXX_FLAGS_DEBUG_MDD" "${CMAKE_CXX_FLAGS} /Od /RTC1 /MDd " )
  set( "CMAKE_CXX_FLAGS_DEBUG_MDD_SHARED" "${CMAKE_CXX_FLAGS_DEBUG_MDD} /Gy- /Zi /D _WINDLL /openmp- /D EMA_LIBRARY_EXPORTS" )

  set( "CMAKE_CXX_FLAGS_RELEASE_MD" "${CMAKE_CXX_FLAGS} /MD /O2 /Ob2 /Ot /D NDEBUG" )
  set( "CMAKE_CXX_FLAGS_RELEASE_MD_STATIC" "${CMAKE_CXX_FLAGS_RELEASE_MD} /GF /Gy" )
  set( "CMAKE_CXX_FLAGS_RELEASE_MD_SHARED" "${CMAKE_CXX_FLAGS_RELEASE_MD} /GL /GR" )

  set( "CMAKE_SHARED_LINKER_FLAGS_DEBUG_MDD" "${CMAKE_SHARED_LINKER_FLAGS}" )
  set( "CMAKE_SHARED_LINKER_FLAGS_RELEASE_MD" "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD" )

  # set ETA library path names
  STRING( REGEX REPLACE "v" "WIN_64_VS" VSSTR "${CMAKE_VS_PLATFORM_TOOLSET}" )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set( CMAKE_CXX_FLAGS "-DLinux  -D__EMA_COPY_ON_SET__ -Wno-ctor-dtor-privacy -Wno-deprecated -std=c++0x -pthread" )
  if ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EMA_STATIC_BUILD__ -DNDEBUG -O3 -fbuiltin" )
  elseif ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -O3 -fbuiltin" )
  elseif ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugShared" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb3" )
  elseif ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -D__EMA_STATIC_BUILD__ -ggdb3" )
  endif()
else()
  message( FATAL_ERROR " ${CMAKE_SYSTEM_NAME} not supported" )
endif()

include_directories( Src/Include )
include_directories( Src )

add_subdirectory( Src/Access )
add_subdirectory( Examples )
