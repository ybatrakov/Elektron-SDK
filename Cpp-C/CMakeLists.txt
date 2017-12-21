cmake_minimum_required (VERSION 3.8)

project (ESDK)

set( SHARED_PATH "Shared" )
set( ETA_VERSION "eta3.1.0.L1" )


# set allowed build types for windows and linux; also select default for Linux
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

  find_package(OpenSSL REQUIRED)
  if ( ${OpenSSL_FOUND} )
    message( STATUS "OpenSSL_FOUND: ${OpenSSL_FOUND}" )
  else()
    message( FATAL_ERROR "could not locate openSSL development environment" )
  endif()

  # default for Linux if build type not specified by user
  if ( NOT CMAKE_BUILD_TYPE )
    set( CMAKE_BUILD_TYPE "Optimized" )
  endif()
  message( STATUS "incoming CMAKE_BUILD_TYPE is ${CMAKE_BUILD_TYPE}" )

  # allowed build types for Linux
  set ( "CMAKE_CONFIGURATION_TYPES" "Optimized" "Debug")
  if ( NOT ${CMAKE_BUILD_TYPE} IN_LIST CMAKE_CONFIGURATION_TYPES )
    message( FATAL_ERROR " invalid build type [${CMAKE_BUILD_TYPE}]; allowed build types are ${CMAKE_CONFIGURATION_TYPES}" )
  endif()


# windows build files are generated and will support the following config type
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( "CMAKE_CONFIGURATION_TYPES" "Debug_MDd" "Release_MD"  )

else()
  message( FATAL_ERROR "unsupported CMAKE_SYSTEM_NAME [${CMAKE_SYSTEM_NAME}]" )
endif()

# build flags
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

  # flags for C
  set( CMAKE_C_FLAGS "-m64 -D_SVID_SOURCE=1 -DCOMPILE_64BITS -DLinux -DLINUX -Dx86_Linux_4X -Dx86_Linux_5X -Dx86_Linux_6X -DLinuxVersion=7 -D_iso_stdcpp_ -D_BSD_SOURCE=1 -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=199506L -D_XOPEN_SOURCE=500 -D_REENTRANT -DDEV_POS_THR -D_POSIX_PTHREAD_SEMANTICS -D_GNU_SOURCE" )
  set( "CMAKE_C_FLAGS_OPTIMIZED" "-fPIC -O3 -DNDEBUG" )
  set( "CMAKE_C_FLAGS_DEBUGSHARED" "-fPIC -ggdb3" )

  # flags for C++
  set( CMAKE_CXX_FLAGS "-DLinux  -D__EMA_COPY_ON_SET__ -Wno-ctor-dtor-privacy -Wno-deprecated -std=c++0x -pthread" )
  if ( ${CMAKE_BUILD_TYPE} STREQUAL "Optimized" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DNDEBUG -O3 -fPIC -fbuiltin" )
  elseif ( ${CMAKE_BUILD_TYPE} STREQUAL "Debug" )
    set ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -ggdb3" )
  endif()

elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
    # flags for C
    set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D _WIN32 /D WIN32_LEAN_AND_MEAN /D _WIN32_WINNT=0x0501 /D _CONSOLE /D _CRT_SECURE_NO_WARNINGS /Zc:wchar_t- /Gm- /O2 /Ob2 /Ot /wd\"4290\" /Gy /WX- /Gd /Oi /EHsc /nologo" )
  set( "CMAKE_C_FLAGS_RELEASE_MD" "/MD  /D NDEBUG" )
  set( CMAKE_C_FLAGS_DEBUG_MDD "/MDd /Zi" )

  # flags for C++
    set( CMAKE_CXX_FLAGS "/MP /GS /W3 /Zc:wchar_t- /Z7 /Gm- /fp:precise /D WIN32 /D _WIN32_WINNT=0x0501 /D _LIB /D _iso_stdcpp_ /D _WIN32 /D WIN32_LEAN_AND_MEAN /D x86_WindowsNT_5X /D _CRT_SECURE_NO_WARNINGS /D _MBCS /WX- /Zc:forScope /Gd /Oi /EHsc /nologo /D LIBXML_STATIC /D COMPILE_64BITS" )

  set( "CMAKE_CXX_FLAGS_DEBUG_MDD" "${CMAKE_CXX_FLAGS} /Od /RTC1 /MDd " )
  set( "CMAKE_CXX_FLAGS_DEBUG_MDD_SHARED" "${CMAKE_CXX_FLAGS_DEBUG_MDD} /Gy- /Zi /D _WINDLL /openmp- " )

  set( "CMAKE_CXX_FLAGS_RELEASE_MD" "${CMAKE_CXX_FLAGS} /MD /O2 /Ob2 /Ot /D NDEBUG" )
  set( "CMAKE_CXX_FLAGS_RELEASE_MD_STATIC" "${CMAKE_CXX_FLAGS_RELEASE_MD} /GF /Gy" )
  set( "CMAKE_CXX_FLAGS_RELEASE_MD_SHARED" "${CMAKE_CXX_FLAGS_RELEASE_MD} /GL /GR" )

  set( "CMAKE_SHARED_LINKER_FLAGS_DEBUG_MDD" "${CMAKE_SHARED_LINKER_FLAGS}" )
  set( "CMAKE_SHARED_LINKER_FLAGS_RELEASE_MD" "${CMAKE_SHARED_LINKER_FLAGS} /NODEFAULTLIB:MSVCRTD" )

endif()
# end build flags

# no RPATH for Linux
set( CMAKE_SKIP_RPATH TRUE )

set( EMA_HOME ${ESDK_SOURCE_DIR}/Ema )
set( ETA_HOME ${ESDK_SOURCE_DIR}/Eta )

message( STATUS "Cpp-C CMAKE_SYSTEM_NAME ${CMAKE_SYSTEM_NAME}" )
if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  STRING( REGEX REPLACE "v" "WIN_64_VS" VSSTR "${CMAKE_VS_PLATFORM_TOOLSET}" )
  set( EXPECTED_BINARY_PACK_LOCATION_HINTS ../Elektron-SDK-BinaryPack/Cpp-C/Eta/Libs/${VSSTR} )
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

# location of Libs, Executables
if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  string( REGEX MATCH "^.*/(.+)" UNUSED ${BINARY_PACK_LOCATION} )
  set( OS_COMPILER ${CMAKE_MATCH_1} )
  message( STATUS "OS_COMPILER: ${OS_COMPILER}" )
  message( STATUS "BINARY_PACK_LOCATION: ${BINARY_PACK_LOCATION}" )
  if ( CMAKE_BUILD_TYPE STREQUAL "Optimized" )
    set( TYPE "Optimized" )
  elseif ( CMAKE_BUILD_TYPE STREQUAL "Debug" )
    set(  TYPE "Debug" )
  endif()
  SET( ETA_LIBS_OUTPUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Eta/Libs/${OS_COMPILER}/${TYPE}" )
  SET( ETA_EXAMPLES_OUTPUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Eta/Executables/${OS_COMPILER}/${TYPE}" )
  SET( EMA_LIBS_OUTPUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Ema/Libs/${OS_COMPILER}/${TYPE}" )
  SET( EMA_EXAMPLES_OUTPUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Ema/Executables/${OS_COMPILER}/${TYPE}" )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  SET( ETA_LIBS_OUTPUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Eta/Libs/${VSSTR}" )
  SET( EMA_LIBS_OUTPUT_DIR  "${CMAKE_CURRENT_SOURCE_DIR}/Ema/Libs/${VSSTR}" )
  SET( EMA_EXAMPLES_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Ema/Executables/${VSSTR}" )
  SET( ETA_EXAMPLES_OUTPUT_DIR "${CMAKE_CURRENT_SOURCE_DIR}/Eta/Executables/${VSSTR}" )
endif()

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )

  # run LinuxSoLink from the binary pack so optimized builds will succeed
	unset( LNKCMDPATH CACHE )
	find_file( LNKCMDPATH name "LinuxSoLink" PATHS "${CMAKE_CURRENT_SOURCE_DIR}/../Elektron-SDK-BinaryPack/Cpp-C/Eta" NO_DEFAULT_PATH )
	if ( LNKCMDPATH )
	  get_filename_component( LNKCMDDIR ${LNKCMDPATH} DIRECTORY )
	  execute_process( COMMAND ${LNKCMDPATH} WORKING_DIRECTORY ${LNKCMDDIR} RESULT_VARIABLE result OUTPUT_QUIET ERROR_QUIET )
	  if ( result )
	message( FATAL_ERROR "script (${LNKCMDPATH}) that creates shared library links failed" )
	  endif()
	else()
	  message( FATAL_ERROR "LinuxSoLink not found; cannot run script to create shared library links" )
	endif()

  message( STATUS "BINARY_PACK_SEARCH_PATH ${BINARY_PACK_SEARCH_PATH}" )

  find_library( RSSL_VACACHE_SHARED rsslVACache HINTS ${BINARY_PACK_LOCATION}/Optimized/Shared )
  find_library( RSSL_VACACHE_STATIC rsslVACache HINTS ${BINARY_PACK_LOCATION}/Optimized )
  
  set( MISSING_LIBRARIES "" )
  foreach( lib RSSL_VACACHE_STATIC RSSL_VACACHE_SHARED )
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
