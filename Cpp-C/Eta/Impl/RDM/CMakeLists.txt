cmake_minimum_required (VERSION 3.8)

set( RDMSrcFiles
  "rsslRDMDictionaryMsg.c"
  "rsslRDMDirectoryMsg.c"
  "rsslRDMLoginMsg.c"
  "rsslRDMQueueMsg.c"
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    add_library( RDMobjects OBJECT ${RDMSrcFiles} )
    add_library( librsslRDM STATIC ${RDMSrcFiles} )
    set_target_properties( librsslRDM PROPERTIES PREFIX "" )
  endif()

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_library( librsslRDM STATIC ${RDMSrcFiles} )
  add_custom_command (
    TARGET librsslRDM PRE_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${EMA_HOME}/Src/Access/Impl/EmaStaticCombined.cpp
    )

endif()
