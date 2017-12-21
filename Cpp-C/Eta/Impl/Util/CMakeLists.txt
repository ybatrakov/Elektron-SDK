cmake_minimum_required (VERSION 3.8)

set( VAUtilSrcFiles
  rsslMemoryBuffer.c
  rsslNotifier.c
  rsslHashTable.c
)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    add_library( VAUtilObjects OBJECT ${VAUtilSrcFiles} )
    add_library( librsslVAUtil STATIC ${VAUtilSrcFiles} )
    set_target_properties( librsslVAUtil PROPERTIES PREFIX "" )
elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_library( librsslVAUtil STATIC ${VAUtilSrcFiles} )
  add_custom_command (
    TARGET librsslVAUtil PRE_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${EMA_HOME}/Src/Access/Impl/EmaStaticCombined.cpp
    )

endif()
