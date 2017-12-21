/*|-----------------------------------------------------------------------------
 *|            This source code is provided under the Apache 2.0 license      --
 *|  and is provided AS IS with no warranty or guarantee of fit for purpose.  --
 *|                See the project's LICENSE.md for details.                  --
 *|           Copyright Thomson Reuters 2018. All rights reserved.            --
 *|-----------------------------------------------------------------------------
 */

#ifndef _rsslLoadInitTransport_h
#define _rsslLoadInitTransport_h

#include <stdio.h>
#include <stdlib.h>
#if defined(_WIN32)
#include <windows.h>
#include <direct.h>
#else
#include <dlfcn.h>
#endif
#include "rtr/os.h"

#if defined( WIN32 )

#define RSSL_LI_LIB						HMODULE
#define RSSL_LI_ERR_T					DWORD
#define RSSL_LI_L_ERR_FMT					"ERROR: [%s]:%d Loading %s, <%u>\n"
#define RSSL_LI_C_ERR_FMT					"ERROR: [%s]:%d Closing %s, <%u>\n"
#define RSSL_LI_RESET_DLERROR                  
#define RSSL_LI_DLOPEN(FILENAME)		LoadLibrary(TEXT(FILENAME))
#define RSSL_LI_DLCLOSE( HANDLE )		FreeLibrary( HANDLE )
#define RSSL_LI_DLSYM(MODULE,FUNCNAME)	GetProcAddress((MODULE), (FUNCNAME))
#define RSSL_LI_GET_DLERROR				GetLastError()
#define RSSL_LI_CHK_DLERROR( pfx, err )\
	( (!pfx ) ? GetLastError() : 0)

#else

#define RSSL_LI_LIB						void *
#define RSSL_LI_ERR_T					char *
#define RSSL_LI_L_ERR_FMT					"ERROR: [%s]:%d Loading %s, <%s>\n"
#define RSSL_LI_C_ERR_FMT					"ERROR: [%s]:%d Closing %s, <%s>\n"
#define RSSL_LI_RESET_DLERROR			dlerror() 
#define RSSL_LI_DLOPEN(FILENAME)		dlopen(FILENAME, RTLD_LAZY)
//  If using RTLD_NOW, library load will fail with undefined interfaces not related
//  to this library interface. i(i.e. undefined z-lib interface)
//#define RSSL_LI_DLOPEN(FILENAME)               dlopen(FILENAME, RTLD_NOW | RTLD_GLOBAL)
#define RSSL_LI_DLCLOSE( HANDLE )		dlclose( HANDLE )
#define RSSL_LI_DLSYM(MODULE,FUNCNAME)	dlsym((MODULE), (FUNCNAME))
#define RSSL_LI_GET_DLERROR				dlerror()
#define RSSL_LI_CHK_DLERROR( pfx, err )\
        ( ((err = dlerror()) != NULL) ? err : NULL )
#endif

#ifdef WIN32
#define DEFAULT_LIB_NAME		"librsslExtLine.dll"
#define DEFAULT_RRCP_LIB_NAME	"librsslRRCP.dll"
#elif defined SOLARIS2
#define DEFAULT_LIB_NAME		 "librsslextline.so.1.0.0"
#define DEFAULT_RRCP_LIB_NAME    "librsslRRCP.so.1.0.0"
#else
#define DEFAULT_LIB_NAME		 "librsslExtLine.so.1"
#define DEFAULT_RRCP_LIB_NAME    "librsslRRCP.so.1"
#endif

static ripcTransportFuncs* rllFunctionsInitialized = 0;
static RsslTransportChannelFuncs* rsslTransportFunctionsInitialized = 0;
static int(*initFuncPointers)(ripcTransportFuncs*, void*);
static int(*initTransportFuncPointers)(RsslTransportChannelFuncs*, void*);
static RSSL_LI_LIB rllHndl = 0;
static char* rllLib = 0;
static RSSL_LI_LIB rsslTransHndl = 0;
static char* rsslTransLib = 0;


RTR_C_INLINE int rsslLoadInitError(RSSL_LI_LIB handle, const char* libname, const char* fname, int lnum, RSSL_LI_ERR_T err)
{
	if (handle != 0)
	{
		RSSL_LI_DLCLOSE(handle);
        handle = 0;
	}
	if (err)
		fprintf(stderr, RSSL_LI_L_ERR_FMT, fname, lnum, libname, err);

	return -1;
}

/* rsslLoadInintTransport has been created to load one generic shared library for an rssl transport.
 * The requirement for the shared library is it should have one exported function:
 *     int initTransportFunctionPointers(ripcTransportFuncs*, void*)
 *        ret <= 0 failure
 *        ret 1 success
 *     All function pointers must be assigned to a valid finction address.  If a transport
 *     does not support a function, the a stub should be defined which returns 1 or SUCCESS/TRUE
 * If a future need requires multiple/different shared library transports to be loaded within the same
 * process, then the local static values for the library handle and funtion pointers should be added to a list
 * in order to keep track of any libraries already loaded.
 */
RTR_C_INLINE int rsslLoadInitTransport(ripcTransportFuncs* rTransFuncs, char* lName, void* pdata)
{
	RSSL_LI_ERR_T err = 0;

    if (!rllHndl) 
    {
        if(!lName)
            rllLib = DEFAULT_LIB_NAME;
        else
            rllLib = lName;
        
        RSSL_LI_RESET_DLERROR;
        rllHndl = RSSL_LI_DLOPEN(rllLib);
        if (!rllHndl)
			return rsslLoadInitError(rllHndl, rllLib, __FILE__, __LINE__, (RSSL_LI_ERR_T)RSSL_LI_GET_DLERROR);
        
	}

	if (!rllFunctionsInitialized)
	{
        RSSL_LI_RESET_DLERROR;
        initFuncPointers = (int (*)(ripcTransportFuncs *, void *)) RSSL_LI_DLSYM(rllHndl, "initTransportFunctionPointers");
        if (err = RSSL_LI_CHK_DLERROR(initFuncPointers, err))
            return rsslLoadInitError(rllHndl, rllLib, __FILE__, __LINE__, err);
        
        if (!(*(initFuncPointers))(rTransFuncs, pdata))
        {
#ifdef _WIN32
            DWORD errcode = 0;
#else
            char* errcode = "Error initializing transport function pointers";
#endif
            return rsslLoadInitError(rllHndl, rllLib, __FILE__, __LINE__, errcode);
        }
		rllFunctionsInitialized = rTransFuncs;
	} else
		rTransFuncs = rllFunctionsInitialized;

	return 1;
}

RTR_C_INLINE int rsslLoadInitRsslTransportChannel(RsslTransportChannelFuncs* rTransFuncs, char* lName, void* pdata)
{
	RSSL_LI_ERR_T err = 0;

	if (!rsslTransHndl)
	{
		if (!lName)
			rsslTransLib = DEFAULT_RRCP_LIB_NAME;
		else
			rsslTransLib = lName;

		RSSL_LI_RESET_DLERROR;
		rsslTransHndl = RSSL_LI_DLOPEN(rsslTransLib);
		if (!rsslTransHndl)
			return rsslLoadInitError(rsslTransHndl, rsslTransLib, __FILE__, __LINE__, (RSSL_LI_ERR_T)RSSL_LI_GET_DLERROR);
	}

	if (!rsslTransportFunctionsInitialized)
	{
		RSSL_LI_RESET_DLERROR;
		initTransportFuncPointers = (int(*)(RsslTransportChannelFuncs *, void *)) RSSL_LI_DLSYM(rsslTransHndl, "initTransportChannelFuncPointers");
		if (err = RSSL_LI_CHK_DLERROR(initTransportFuncPointers, err))
			return rsslLoadInitError(rsslTransHndl, rsslTransLib, __FILE__, __LINE__, err);

		if (!(*(initTransportFuncPointers))(rTransFuncs, pdata))
		{
#ifdef _WIN32
			DWORD errcode = 0;
#else
			char* errcode = "Error initializing transport channel function pointers";
#endif
			return rsslLoadInitError(rsslTransHndl, rsslTransLib, __FILE__, __LINE__, errcode);
		}
		rsslTransportFunctionsInitialized = rTransFuncs;
	}
	else
		rTransFuncs = rsslTransportFunctionsInitialized;

	return 1;
}

RTR_C_INLINE int rsslUnloadTransport()
{
	RSSL_LI_ERR_T err = 0;

    if (rllHndl) 
    {
        RSSL_LI_RESET_DLERROR;
        if(RSSL_LI_DLCLOSE(rllLib))
		{
			fprintf(stderr, RSSL_LI_C_ERR_FMT, __FILE__, __LINE__, rllLib, (RSSL_LI_ERR_T)RSSL_LI_GET_DLERROR);
            return -1;
		}
    }

	if (rsslTransHndl)
	{
		RSSL_LI_RESET_DLERROR;
		if (RSSL_LI_DLCLOSE(rsslTransLib))
		{
			fprintf(stderr, RSSL_LI_C_ERR_FMT, __FILE__, __LINE__, rsslTransLib, (RSSL_LI_ERR_T)RSSL_LI_GET_DLERROR);
			return -1;
		}
	}

	return 1;
}

#endif
