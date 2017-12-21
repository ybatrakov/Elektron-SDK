cmake_minimum_required (VERSION 3.8)

include_directories(
  ${ETA_HOME}/Include/Codec
  ${ETA_HOME}/Include/RDM
  ${ETA_HOME}/Include/Transport
  ${ETA_HOME}/Include/Util
  )

# build static libraries only
add_subdirectory( RDM )
add_subdirectory( Reactor )
add_subdirectory( Util )

add_subdirectory( Codec )

# use these for rsslVA only
include_directories(
  ${ETA_HOME}/Include/Reactor
  ${ETA_HOME}/Impl/Reactor
  ${ETA_HOME}/Impl/Reactor/Watchlist
  ${ETA_HOME}/Impl/Reactor/Util
  ${ETA_HOME}/Impl/Reactor/TunnelStream
  )

# shared library containing contents of above directories
set ( rsslVASrcFiles
  RDM/rsslRDMDictionaryMsg.c
  RDM/rsslRDMDirectoryMsg.c
  RDM/rsslRDMLoginMsg.c
  RDM/rsslRDMQueueMsg.c
  Reactor/rsslReactor.c
  Reactor/rsslReactorWorker.c
  Reactor/TunnelStream/bigBufferPool.c
  Reactor/TunnelStream/bufferPool.c
  Reactor/TunnelStream/msgQueueEncDec.c
  Reactor/TunnelStream/persistFile.c
  Reactor/TunnelStream/rsslRDMQueueMsgInt.c
  Reactor/TunnelStream/rsslTunnelStream.c
  Reactor/TunnelStream/tunnelManager.c
  Reactor/TunnelStream/tunnelStreamImpl.c
  Reactor/TunnelStream/tunnelSubstream.c
  Reactor/Watchlist/rsslWatchlistImpl.c
  Reactor/Watchlist/wlBase.c
  Reactor/Watchlist/wlDirectory.c
  Reactor/Watchlist/wlItem.c
  Reactor/Watchlist/wlLogin.c
  Reactor/Watchlist/wlMsgReorderQueue.c
  Reactor/Watchlist/wlPostIdTable.c
  Reactor/Watchlist/wlService.c
  Reactor/Watchlist/wlServiceCache.c
  Reactor/Watchlist/wlSymbolList.c
  Reactor/Watchlist/wlView.c
  Util/DllVAMain.c
  Util/rsslHashTable.c
  Util/rsslMemoryBuffer.c
  Util/rsslNotifier.c
)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_library( librsslVA SHARED ${rsslVASrcFiles} )
  #  for shared libraries, the .lib and .exp files go to ARCHIVE; the .dll files go to RUNTIME
  target_compile_options( librsslVA PUBLIC "/D RSSL_VA_EXPORTS" )
  target_link_libraries( librsslVA debug librssl_shared )
  target_link_libraries( librsslVA optimized librssl_shared )
  target_link_libraries( librsslVA wininet.lib ws2_32.lib )

elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  add_library( librsslVA_shared SHARED ${rsslVASrcFiles} )
  set_target_properties( librsslVA_shared PROPERTIES OUTPUT_NAME "rsslVA" )
  set_target_properties( librsslVA_shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY "${ETA_LIBS_OUTPUT_DIR}/${SHARED_PATH}" )
endif()
