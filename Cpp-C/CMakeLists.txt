cmake_minimum_required (VERSION 3.8)

project (ESDK)

# set allowed build types for windows and linux; also select default for Linux
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

  # default for Linux if build type not specified by user
  if ( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "OptimizedStatic" )
  endif()
  message( STATUS "incoming CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}" )

  # allowed build types for Linux
  set ( "CMAKE_CONFIGURATION_TYPES" "OptimizedStatic" "OptimizedShared" "DebugShared" "DebugStatic" )
  if ( NOT ${CMAKE_BUILD_TYPE} IN_LIST CMAKE_CONFIGURATION_TYPES )
    message( FATAL_ERROR " invalid build type [${CMAKE_BUILD_TYPE}]; allowed build types are ${CMAKE_CONFIGURATION_TYPES}" )
  endif()

# windows build files are generated and will support the following config type
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( "CMAKE_CONFIGURATION_TYPES" "Debug_MDd" "Release_MD"  )

else()
  message( FATAL_ERROR "unsupported CMAKE_SYSTEM_NAME [${CMAKE_SYSTEM_NAME}]" )
endif()

# no RPATH for Linux
set( CMAKE_SKIP_RPATH TRUE )

set( EMA_HOME ${ESDK_SOURCE_DIR}/Ema )
set( ETA_HOME ${ESDK_SOURCE_DIR}/Eta )

message( STATUS "Cpp-C CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}" )
if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  STRING( REGEX REPLACE "v" "WIN_64_VS" VSSTR "${CMAKE_VS_PLATFORM_TOOLSET}" )
  set ( EXPECTED_BINARY_PACK_LOCATION_HINTS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Libs/${VSSTR} )
  set( EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Utils/Dacs/Libs/${VSSTR} )

  # note: we had to look for some path in ${EXPECTED_BINARY_PACK_LOCATION_HINTS} so we selected Debug_MDd
  # but BINARY_PACK_LOCATION includes the directory Debug_MDd is found in
  find_path( BINARY_PACK_LOCATION name Debug_MDd HINTS ${EXPECTED_BINARY_PACK_LOCATION_HINTS} )
  if ( NOT BINARY_PACK_LOCATION )
    message( FATAL_ERROR "could not locate binary pack at ${EXPECTED_BINARY_PACK_LOCATION_HINTS}" )
  endif()
  find_path( BINARY_PACK_LOCATION_DACS name Debug_MDd HINTS ${EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS} )
  if ( NOT BINARY_PACK_LOCATION_DACS )
    message( FATAL_ERROR "could not locate binary pack for DACS at ${EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS}" )
  endif()

  set( CMAKE_FIND_LIBRARY_PREFIXES "lib" )
  find_library( RSSL_VACACHE_DEBUG_STATIC rsslVACache HINTS ${BINARY_PACK_LOCATION}/Debug_MDd )
  find_library( RSSL_VACACHE_DEBUG_SHARED rsslVACache HINTS ${BINARY_PACK_LOCATION}/Debug_MDd/Shared )
  find_library( RSSL_VACACHE_RELEASE_STATIC rsslVACache HINTS ${BINARY_PACK_LOCATION}/Release_MD )
  find_library( RSSL_VACACHE_RELEASE_SHARED rsslVACache HINTS ${BINARY_PACK_LOCATION}/Release_MD/Shared )
  set( MISSING_LIBRARIES "" )
  foreach( lib RSSL_VACACHE_DEBUG_STATIC RSSL_VACACHE_DEBUG_SHARED RSSL_VACACHE_RELEASE_STATIC RSSL_VACACHE_RELEASE_SHARED )
    if ( NOT ${lib} )
      set( MISSING_LIBRARIES "${MISSING_LIBRARIES} ${lib}" )
      message( STATUS "library ${lib} NOT FOUND" )
    endif()
  endforeach()
  if ( MISSING_LIBRARIES )
    message( FATAL_ERROR "one or more necessary libraries [${MISSING_LIBRARIES}] were not found" )
  endif()

  set( "CMAKE_EXE_LINKER_FLAGS" "/machine:x64 /INCREMENTAL:NO" )
  set( "CMAKE_EXE_LINKER_FLAGS_DEBUG_MDD" "${CMAKE_EXE_LINKER_FLAGS} /debug /NODEFAULTLIB:MSVCRT" )

  set( "CMAKE_EXE_LINKER_FLAGS_RELEASE_MD" "${CMAKE_EXE_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD" )

  set( "CMAKE_SHARED_LINKER_FLAGS_DEBUG_MDD" "${CMAKE_SHARED_LINKER_FLAGS}" )

  set( "CMAKE_SHARED_LINKER_FLAGS_RELEASE_MD" "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD" )

  set_property ( GLOBAL PROPERTY DEBUG_CONFIGURATIONS "Debug_MDd" ) #all other configurartions considered optimized

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  execute_process( COMMAND bash "-c" "lsb_release -rs" RESULT_VARIABLE result OUTPUT_VARIABLE IDENT OUTPUT_STRIP_TRAILING_WHITESPACE )
  if ( ${result} EQUAL 0 )
    execute_process( COMMAND bash "-c" "echo ${IDENT} | sed 's/\\..*//'" RESULT_VARIABLE result OUTPUT_VARIABLE IDENT OUTPUT_STRIP_TRAILING_WHITESPACE )
    if ( ${result} EQUAL 0 )
      if ( ${IDENT} EQUAL 6 )
	set( EXPECTED_BINARY_PACK_LOCATION_HINTS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Libs/RHEL6_64_GCC444 )
	set( EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Utils/Dacs/Libs/RHEL6_64_GCC444 )
      elseif ( ${IDENT} EQUAL 7 )
	set( EXPECTED_BINARY_PACK_LOCATION_HINTS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Libs/OL7_64_GCC482 )
	set( EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Utils/Dacs/Libs/OL7_64_GCC482 )
      else()
	message( FATAL_ERROR "unexpected result [${IDENT}] for lsb_release | sed" )
      endif()
    else()
      message( FATAL_ERROR "sed failed to process IDENT returned by lsb_release -rs" )
    endif()

    find_path( BINARY_PACK_LOCATION name Optimized HINTS ${EXPECTED_BINARY_PACK_LOCATION_HINTS} )
    if ( NOT BINARY_PACK_LOCATION )
      message( FATAL_ERROR "could not locate binary pack at ${EXPECTED_BINARY_PACK_LOCATION_HINTS}" )
    endif()
    find_path( BINARY_PACK_LOCATION_DACS name Optimized HINTS ${EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS} )
    if ( NOT BINARY_PACK_LOCATION_DACS )
      message( FATAL_ERROR "could not locate binary pack (dacs) at ${EXPECTED_BINARY_PACK_LOCATION_HINTS_DACS}" )
    endif()
  endif()			# CMAKE_SYSTEM_NAME == Linux
else()
  message( FATAL_ERROR "need code to find binary pack" )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic" )
    find_library( RSSL_VACACHE rsslVACache HINTS ${BINARY_PACK_LOCATION}/Optimized )
    find_library( RSSL rssl HINTS ${BINARY_PACK_LOCATION}/Optimized )
  elseif ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared" OR
      ${CMAKE_BUILD_TYPE} STREQUAL "DebugShared" )
    find_library( RSSL_VACACHE rsslVACache HINTS ${BINARY_PACK_LOCATION}/Optimized/Shared )
    find_library( RSSL rssl HINTS ${BINARY_PACK_LOCATION}/Optimized/Shared )
  endif()

  set( MISSING_LIBRARIES "" )
  foreach( lib RSSL RSSL_VACACHE )
    if ( NOT ${lib} )
      set( MISSING_LIBRARIES "${MISSING_LIBRARIES} ${lib}" )
      message( STATUS "library ${lib} NOT FOUND" )
    endif()
  endforeach()
  if ( MISSING_LIBRARIES )
    message( FATAL_ERROR "one or more necessary libraries [${MISSING_LIBRARIES}] were not found" )
  endif()
endif()

add_subdirectory( Eta )
add_subdirectory( Ema )
