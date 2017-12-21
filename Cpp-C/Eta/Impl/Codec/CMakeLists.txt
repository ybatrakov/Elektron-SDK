cmake_minimum_required (VERSION 3.8)

project (RSSL)

include(GenerateExportHeader)

set( RSSL_CODEC_HOME ${RSSL_SOURCE_DIR} )
get_filename_component( PARENT_DIR ${RSSL_SOURCE_DIR} DIRECTORY )
set( RSSL_TRANSPORT_HOME ${PARENT_DIR}/Transport )
message( STATUS "RSSL_CODEC_HOME: ${RSSL_CODEC_HOME}" )
message( STATUS "RSSL_TRANSPORT_HOME: ${RSSL_TRANSPORT_HOME}" )

get_filename_component( ETA ${PARENT_DIR} DIRECTORY )
set( LZ4_HOME ${ETA}/Utils/Lzf )
set( OPENSSL_HOME "${ETA}/Utils/openssl" ) 
set( ZLIB_HOME ${ETA}/Utils/zlib )
message( STATUS "ETA: ${ETA}" )

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Dx86_Linux_CLOCK_MONOTONIC -D_SVID_SOURCE=1 -D_BSD_SOURCE=1 -DLINUX -DDEV_LITTLE_ENDIAN -DTRACING_ENABLED  -DMCAST_BUFFER_MSGS" )
endif()

include( cmake.rsslLibraryFiles.txt )

add_library( librssl_static STATIC ${rsslLibrarySrcFiles} rsslVersionStatic.c )
add_library( librssl_shared SHARED ${rsslLibrarySrcFiles} rsslVersionShared.c )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set_target_properties( librssl_static PROPERTIES PREFIX "" OUTPUT_NAME "librssl" )
  set_target_properties( librssl_shared PROPERTIES PREFIX "" OUTPUT_NAME "librssl" )
  set_target_properties( librssl_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${ETA_LIBS_OUTPUT_DIR}/${SHARED_PATH}" )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_custom_command (
    TARGET librssl_static PRE_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${EMA_HOME}/Src/Access/Impl/EmaStaticCombined.cpp
    )
  GENERATE_EXPORT_HEADER( librssl_shared NO_EXPORT_MACRO_NAME )
  target_link_libraries( librssl_shared wininet.lib ws2_32.lib )
  target_compile_options( librssl_shared PUBLIC "/D RSSL_EXPORTS" )
endif()

set( librssl_shared_VERSION_MAJOR 6 )
set( librssl_shared_VERSION_STRING ${librssl_shared_VERSION_MAJOR} )
set_target_properties( librssl_shared PROPERTIES VERSION ${librssl_shared_VERSION_STRING} SOVERSION ${librssl_shared_VERSION_MAJOR} )

include_directories(
  "${ETA}/Codec/Include"
  "${RSSL_TRANSPORT_HOME}"
  "${RSSL_TRANSPORT_HOME}/Util/Include"
  "${RSSL_CODEC_HOME}"
  "${LZ4_HOME}"
  "${OPENSSL_HOME}/include"
  )
include_directories(BEFORE "${ZLIB_HOME}")


