cmake_minimum_required (VERSION 3.8)

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  message( FATAL_ERROR "RmtesNewsDisplay for Windows Only" )
endif()

