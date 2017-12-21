cmake_minimum_required (VERSION 3.8)

include(GenerateExportHeader)

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${EMA_LIBS_OUTPUT_DIR} )
set( CMAKE_LIBRARY_OUTPUT_DIRECTORY ${EMA_LIBS_OUTPUT_DIR} )
set( CMAKE_RUNTIME_OUTPUT_DIRECTORY ${EMA_LIBS_OUTPUT_DIR} )

include( cmake.emaLibraryFiles.txt )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_library( libema_shared SHARED ${EmaSrcFiles} ${EmaHeaderFiles} )
  add_library( libema_only_static ${EmaSrcFiles} ${EmaHeaderFiles} )
  target_compile_options( libema_shared PUBLIC "/D EMA_LIBRARY_EXPORTS" )
  target_compile_options( libema_only_static PUBLIC "/D __EMA_STATIC_BUILD__" )

  GENERATE_EXPORT_HEADER( libema_shared NO_EXPORT_MACRO_NAME )

  set( CMAKE_FIND_LIBRARY_PREFIXES "lib" )

  find_library( RSSLVACACHE_DEBUG_SHARED rsslVACache HINTS ${BINARY_PACK_LOCATION}/Debug_MDd/Shared )
  find_library( RSSLVACACHE_RELEASE_SHARED rsslVACache HINTS ${BINARY_PACK_LOCATION}/Release_MD/Shared )

  # fail if libraries not found
  set( MISSING_LIBRARIES "" )
  foreach( lib RSSLVACACHE_DEBUG_SHARED RSSLVACACHE_RELEASE_SHARED )
    if ( NOT ${lib} )
      set( MISSING_LIBRARIES "${MISSING_LIBRARIES} ${lib}" )
      message( STATUS "library ${lib} NOT FOUND" )
    endif()
  endforeach()

  if ( MISSING_LIBRARIES )
    message( FATAL_ERROR "one or more necessary libraries [${MISSING_LIBRARIES}] were not found" )
  endif()

  get_filename_component(_vs_bin_path "${CMAKE_LINKER}" DIRECTORY)
  add_custom_command (
    TARGET libema_only_static PRE_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${EMA_HOME}/Src/Access/Impl/EmaStaticCombined.cpp
    )
  target_link_libraries( libema_shared debug librsslVA libxml2_static ${RSSL_SHARED_DEBUG} ${RSSL_VACACHE_DEBUG_SHARED} )
  target_link_libraries( libema_shared optimized librsslVA libxml2_static ${RSSL_SHARED_RELEASE} ${RSSL_VACACHE_RELEASE_SHARED} )
  target_link_libraries( libema_shared wininet.lib ws2_32.lib )

  add_library( libema_static ${EMA_HOME}/Src/Access/Impl/EmaStaticCombined.cpp )
  target_compile_options( libema_static PUBLIC "/D __EMA_STATIC_BUILD__" )
  add_dependencies( libema_static librsslRDM librsslReactor librsslVAUtil libxml2_static librssl_static )
  add_custom_command ( TARGET libema_static POST_BUILD COMMAND
    COMMAND
    ( ${_vs_bin_path}/lib.exe /IGNORE:4006 /OUT:$<TARGET_FILE:libema_static> /LTCG $<TARGET_FILE:libema_only_static> /LIBPATH:${ETA_LIBS_OUTPUT_DIR}/$(Configuration) librsslRDM.lib librsslReactor.lib librsslVAUtil.lib librssl_static.lib libxml2_static.lib /LIBPATH:${BINARY_PACK_LOCATION}/$(Configuration) librsslVACache.lib )
  )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  add_library ( libema_static STATIC ${EmaSrcFiles} ${EmaHeaderFiles} $<TARGET_OBJECTS:RDMobjects>
    $<TARGET_OBJECTS:ReactorObjects> $<TARGET_OBJECTS:VAUtilObjects> $<TARGET_OBJECTS:XML2objects>
    )
  set_target_properties ( libema_static PROPERTIES PREFIX "" OUTPUT_NAME "libema" )
  add_library ( libema_shared SHARED ${EmaSrcFiles} ${EmaHeaderFiles} )
  target_link_libraries( libema_shared librsslVA_shared librssl_shared libxml2 )
  set_target_properties( libema_shared PROPERTIES PREFIX "" OUTPUT_NAME "libema" )
  set_target_properties( libema_shared PROPERTIES DEFINE_SYMBOL "" ) # avoid -Dlibema_EXPORTS
  set_target_properties( libema_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${EMA_LIBS_OUTPUT_DIR}/${SHARED_PATH}" )
	

  if ( MISSING_LIBRARIES )
    message( FATAL_ERROR "one or more necessary libraries [${MISSING_LIBRARIES}] were not found" )
  endif()

else()
  message( fatal_error " ${CMAKE_SYSTEM_NAME} not supported" )
endif()

include_directories(
  "${EMA_HOME}/Src"
  "${EMA_HOME}/Src/Access/Include"
  "${EMA_HOME}/Src/Access/Impl"
  "${ETA_HOME}/Include/Codec"
  "${ETA_HOME}/Include/Util"
  "${ETA_HOME}/Include/Transport"
  "${ETA_HOME}/Include/Reactor"
  "${ETA_HOME}/Include/RDM"
  "${ETA_HOME}/Utils/Libxml2/include"
  "${EMA_HOME}/Src/Rdm/Impl"
  "${EMA_HOME}/Src/Rdm/Include"
  )
