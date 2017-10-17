cmake_minimum_required (VERSION 3.8)

set( ReactorSrcFiles
  TunnelStream/bigBufferPool.c
  TunnelStream/bufferPool.c
  TunnelStream/msgQueueEncDec.c
  TunnelStream/persistFile.c
  TunnelStream/rsslRDMQueueMsgInt.c
  TunnelStream/rsslTunnelStream.c
  TunnelStream/tunnelManager.c
  TunnelStream/tunnelStreamImpl.c
  TunnelStream/tunnelSubstream.c
  Watchlist/rsslWatchlistImpl.c
  Watchlist/wlBase.c
  Watchlist/wlDirectory.c
  Watchlist/wlItem.c
  Watchlist/wlLogin.c
  Watchlist/wlMsgReorderQueue.c
  Watchlist/wlPostIdTable.c
  Watchlist/wlService.c
  Watchlist/wlServiceCache.c
  Watchlist/wlSymbolList.c
  Watchlist/wlView.c
  rsslReactor.c
  rsslReactorWorker.c
  )

include_directories(
  ${ETA_HOME}/Include/Reactor
  ${ETA_HOME}/Impl/Reactor
  ${ETA_HOME}/Impl/Reactor/Watchlist
  ${ETA_HOME}/Impl/Reactor/Util
  ${ETA_HOME}/Impl/Reactor/TunnelStream
  )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "OptimizedAssertStatic" ) OR
      ( ${CMAKE_BUILD_TYPE} STREQUAL "DebugStatic") )
    add_library( ReactorObjects OBJECT ${ReactorSrcFiles} )
    add_library( librsslReactor STATIC ${ReactorSrcFiles} )
    set_target_properties( librsslReactor PROPERTIES PREFIX "" )
  endif()

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_library( librsslReactor STATIC ${ReactorSrcFiles} )
  if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    set_target_properties( librsslReactor PROPERTIES PREFIX "" )
    add_custom_command (
      TARGET librsslReactor PRE_BUILD COMMAND ${CMAKE_COMMAND} -E touch ${EMA_HOME}/Src/Access/Impl/EmaStaticCombined.cpp
      )
  endif()

endif()
