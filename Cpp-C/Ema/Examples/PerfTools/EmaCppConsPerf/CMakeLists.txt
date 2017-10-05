cmake_minimum_required (VERSION 3.8)

set(EmaCppConsPerfSrcFiles
  "EmaCppConsPerf.cpp"
  "ConsPerfConfig.cpp"
  "ConsumerThread.cpp"
  "../Common/AppUtil.cpp"
  "../Common/CtrlBreakHandler.cpp"
  "../Common/GetTime.cpp"
  "../Common/Mutex.cpp"
  "../Common/PerfConfig.cpp"
  "../Common/Statistics.cpp"
  "../Common/ThreadAffinity.cpp"
  "../Common/XmlItemParser.cpp"
)

set(EmaCppConsPerfFilesFiles
  "EmaCppConsPerf.h"
  "ConsPerfConfig.h"
  "ConsumerThread.h"
  "../Common/AppUtil.h"
  "../Common/AppVector.h"
  "../Common/CtrlBreakHandler.h"
  "../Common/GetTime.h"
  "../Common/Mutex.h"
  "../Common/PerfConfig.h"
  "../Common/Statistics.h"
  "../Common/ThreadAffinity.h"
  "../Common/ThreadBinding.h"
  "../Common/XmlItemParser.h"
)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( STATIC_EXT "_static" )
endif()

add_executable( EmaCppConsPerf${STATIC_EXT} ${EmaCppConsPerfSrcFiles} )

include_directories( "${ETA_HOME}/Utils/Libxml2/include" )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  target_link_libraries( EmaCppConsPerf_static ${EXAMPLE_STATIC_LIBRARIES} psapi.lib )
else()
  target_link_libraries( EmaCppConsPerf ${EXAMPLE_LIBRARIES} ${SYSTEM_LIBRARIES} )
endif()
