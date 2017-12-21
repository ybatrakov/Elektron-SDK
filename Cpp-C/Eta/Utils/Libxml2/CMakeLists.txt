cmake_minimum_required (VERSION 3.8)

set( CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${ETA_LIBS_OUTPUT_DIR} )
#set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${ETA_LIBS_OUTPUT_DIR}) #not needed for windows
#set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Eta/Libs) #not needed for windows

if( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  set( "CMAKE_C_FLAGS_RELEASE_MD" "/MD  /D NDEBUG /D _CRT_SECURE_NO_WARNINGS" )
  set( "CMAKE_C_FLAGS_DEBUG_MDD" "${CMAKE_C_FLAGS} /MDd /D _CRT_SECURE_NO_WARNINGS" )
elseif( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
  set( CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fPIC -DHAVE_CONFIG_H -DLIBXML_THREAD_ENABLED -D_POSIX_PTHREAD_SEMANTICS" )
endif()

set( Libxml2SrcFiles
  DOCBparser.c HTMLparser.c HTMLtree.c SAX.c SAX2.c
  c14n.c catalog.c chvalid.c debugXML.c dict.c
  encoding.c entities.c error.c globals.c hash.c
  legacy.c list.c nanoftp.c nanohttp.c parser.c
  parserInternals.c pattern.c relaxng.c schematron.c threads.c
  tree.c uri.c valid.c xinclude.c xlink.c
  xmlIO.c xmlmemory.c xmlmodule.c xmlreader.c xmlregexp.c
  xmlsave.c xmlschemas.c xmlschemastypes.c xmlstring.c xmlunicode.c
  xmlwriter.c xpath.c xpointer.c
)

set( Libxml2HeaderFiles
  acconfig.h config.h elfgcchack.h libxml.h trio.h
  triodef.h trionan.h triop.h triostr.h
  )

include_directories( include )

if ( ${CMAKE_SYSTEM_NAME} STREQUAL "Linux" )
    add_library( XML2objects OBJECT ${Libxml2SrcFiles} ${Libxml2HeaderFiles} )
    add_library( libxml2 STATIC ${Libxml2SrcFiles} ${Libxml2HeaderFiles} )
	set_target_properties( libxml2 PROPERTIES PREFIX "" OUTPUT_NAME libxml2 )
elseif ( ${CMAKE_SYSTEM_NAME} STREQUAL "Windows" )
  add_library( libxml2_static STATIC ${Libxml2SrcFiles} ${Libxml2HeaderFiles} )
endif()


# no need to build libxml2 shared as libema links in the static version for both static and shared versions
#add_library( libxml2_shared SHARED ${Libxml2SrcFiles} ${Libxml2HeaderFiles} )
#target_compile_options( libxml2_shared PUBLIC "/D RSSL_VA_EXPORTS" )



