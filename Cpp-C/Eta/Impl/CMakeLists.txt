cmake_minimum_required (VERSION 3.8)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Eta/Libs)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Eta/Libs)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Eta/Libs)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /D WIN32_LEAN_AND_MEAN /D _CONSOLE /D _CRT_SECURE_NO_WARNINGS /Zc:wchar_t- /Gm- /O2 /Ob2 /Ot /wd\"4290\" /Gy /WX- /Gd /Oi /EHsc /nologo" )
  set( "CMAKE_C_FLAGS_RELEASE_MD" "/MD  /D NDEBUG" )
  set( CMAKE_C_FLAGS_DEBUG_MDD "/MDd /Zi" )
elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set( CMAKE_C_FLAGS "-m64 -D_SVID_SOURCE=1 -DCOMPILE_64BITS -DLinux -Dx86_Linux_4X -Dx86_Linux_5X -DLinuxVersion=5 -D_iso_stdcpp_ -D_BSD_SOURCE=1 -D_POSIX_SOURCE=1 -D_POSIX_C_SOURCE=199506L -D_XOPEN_SOURCE=500 -D_REENTRANT -DDEV_POS_THR -D_POSIX_PTHREAD_SEMANTICS -D_GNU_SOURCE" )
  set( "CMAKE_C_FLAGS_OPTIMIZEDSTATIC" "-O3 -DNDEBUG" )
  set( "CMAKE_C_FLAGS_OPTIMIZEDSHARED" "-O3 -DNDEBUG" )
  set( "CMAKE_C_FLAGS_DEBUGSTATIC" "-ggdb3" )
  set( "CMAKE_C_FLAGS_DEBUGSHARED" "-ggdb3" )
endif()

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
  target_compile_options( librsslVA PUBLIC "/D RSSL_VA_EXPORTS" )

  target_link_libraries( librsslVA debug ${RSSL_DEBUG_SHARED} )
  target_link_libraries( librsslVA optimized ${RSSL_RELEASE_SHARED} )
  target_link_libraries( librsslVA wininet.lib ws2_32.lib )
elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  if ( (${CMAKE_BUILD_TYPE} STREQUAL "OptimizedShared") OR
      (${CMAKE_BUILD_TYPE} STREQUAL "DebugShared") )
    add_library( librsslVA SHARED ${rsslVASrcFiles} )
    set_target_properties( librsslVA PROPERTIES PREFIX "" )
  endif()
endif()
