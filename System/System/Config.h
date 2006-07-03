#ifndef __CONFIG_H__
#define __CONFIG_H__

/*! \def FUNCTION_CALL_TYPE
 *	\brief DNS-SD callbacks type (WIN32 specific).
 *
 *	As WIN32 plateform supports many function call types, we must use the right
 *	one for DNS-SD callbacks. As defined in DNS-SD documentation, under WIN32 plateforms
 *	the callback type is __stdcall.	On other plateform, this define is empty.
 */
/*! \def strcasecmp
 *	\brief Wrapper for the unix strcasecmp function (WIN32 only).
 *
 */
/*! \def strncasecmp
 *	\brief Wrapper for the unix strncasecmp function (WIN32 only).
 *
 */
#ifdef WIN32
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif

	#include <Winsock2.h>
	#include <process.h>		// Process action

	#define FUNCTION_CALL_TYPE __stdcall
	/*! \def DEBUG
	 *	\brief Define the gcc like debug symbol (WIN32 only).
     *
	 *	In order to have portable DEBUG support, we need to define a common
	 *	debug symbol. We choose to use the debug symbol used by gcc : DEBUG.
	 */
	#if defined _DEBUG && ! defined DEBUG
		#define DEBUG	// let Visual Studio be gcc -g compliant
	#endif

	#if defined _WIN32 && ! defined WIN32
		#define WIN32	// WIN32 is more interesting for us
	#endif

	#define strcasecmp stricmp
	#define strncasecmp strnicmp
#else
	#define FUNCTION_CALL_TYPE
	#include <sys/socket.h>
	#define strcasecmp strcasecmp
	#define strncasecmp strncasecmp

	#include <sys/types.h>
	#include <sys/socket.h>
	#include <sys/time.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	typedef int SOCKET;
	#define SOCKET_ERROR (-1)

	#include <unistd.h>

	#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

namespace Omiscid {

// Ugly for the moment
#ifdef OMISCID_TRACE_ENABLE
	// in trace mode, we plan to trace every thing
	inline int TraceError(const char * format, ... )
	{
	   va_list args;
	   va_start( args, format );
	   vprintf( format, args );
	   va_end( args );
	   return 0;
	}
#else
	// in other mode, we do not print nothing
	inline int TraceError(...)
	{
	   return 0;
	}
#endif


#ifdef WIN32

// For Time function port under Windows
struct timezone {
       int  tz_minuteswest; /* minutes W of Greenwich */
       int  tz_dsttime;     /* type of dst correction */
};

#endif	// ifdef WIN32

} // namespace Omiscid

#endif	// __CONFIG_H__
