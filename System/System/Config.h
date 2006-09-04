/** 
 * @defgroup System Layer 0 : Omiscid multiplateform System layer. 
 *
 */

/** 
 * @file System/Config.h
 * @ingroup System
 * @brief this file is the first file included in the System layer. it defines all mandatory includes.
 */

#ifndef __SYSTEM_CONFIG_H__
#define __SYSTEM_CONFIG_H__

#if defined _WIN32 && ! defined WIN32
   /**
	* @def WIN32
	* @ingroup System
	* @brief Define the WIN32 symbol
	*
	* In order to be sure to have the correct WIN32 symbol.
	*/
	#define WIN32	// WIN32 is more general for us
#endif


#ifdef WIN32

	// To prevent warning about printf, strcpy in Visual Studio 2005, etc...
	#ifndef _CRT_SECURE_NO_DEPRECATE
		#define _CRT_SECURE_NO_DEPRECATE
	#endif

	#pragma warning(disable : 4996) // deprecated API
	#pragma warning(disable : 4100) // formal parameter not used, mostly in virtual fonction
	#pragma warning(disable : 4512) // Could not generate automatically operator= for a class
	// #pragma warning(disable : 4127)	// to remove carning about constant expression when compiling...

	// If U are using MFC and co...
	#ifdef USE_AFX
		#define  WINVER 0x0501
		#include "StdAfx.h"
		// To prevent problems using iostream later...
		#ifndef _IOSTREAM_
			#error "You must include '<iostream>' in your 'StdAfx.h' file."
		#endif
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif

	#include <Winsock2.h>
	#include <process.h>		// Process action

   /**	@def FUNCTION_CALL_TYPE
    *	@ingroup System
	*	@brief DNS-SD callbacks type (WIN32 specific).
	*
	*	As WIN32 plateform supports many function call types, we must use the right
	*	one for DNS-SD callbacks. As defined in DNS-SD documentation, under WIN32 plateforms
	*	the callback type is __stdcall.	On other plateform, this define is empty.
	*/
	#define FUNCTION_CALL_TYPE __stdcall

	/** @def DEBUG
     *	@ingroup System
	 *	@brief Define the gcc like debug symbol (WIN32 only).
     *
	 *	In order to have portable DEBUG support, we need to define a common
	 *	debug symbol. We choose to use the debug symbol used by gcc : DEBUG.
	 */
	#if defined _DEBUG && ! defined DEBUG
		#define DEBUG	// let Visual Studio be gcc -g compliant
	#endif

	/*! @def strcasecmp
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	For multiplateform source code.
	 */
	

	/*! @def strncasecmp
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	For multiplateform source code.
	 */

	/*! @def _snprintf
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	For snprintf source code.
	 */
	#define strcasecmp stricmp
	#define strncasecmp strnicmp
	#define snprintf _snprintf

	/*! @def SOCKET
     *	@ingroup System
	 *	@brief Define the SOCKET type as on WIN32 plateform.
     *
	 *	For multiplateform source code.
	 */
	/*! @def SOCKET_ERROR
     *	@ingroup System
	 *	@brief Define the socket error symbol as on WIN32 plateform.
     *
	 *	For multiplateform source code.
	 */

#else
	#include <sys/types.h>
	#include <sys/time.h>

	#include <sys/socket.h>
	#include <netinet/in.h>
	#include <arpa/inet.h>
	
   /*!	@def FUNCTION_CALL_TYPE
    *	@ingroup System
	*	@brief DNS-SD callbacks type (WIN32 specific).
	*
	*	As WIN32 plateform supports many function call types, we must use the right
	*	one for DNS-SD callbacks. As defined in DNS-SD documentation, under WIN32 plateforms
	*	the callback type is __stdcall.	On other plateform, this define is empty.
	*/
	#define FUNCTION_CALL_TYPE

	/*! @def SOCKET
     *	@ingroup System
	 *	@brief Define the SOCKET type as on WIN32 plateform.
     *
	 *	For multiplateform source code.
	 */
	/*! @def SOCKET_ERROR
     *	@ingroup System
	 *	@brief Define the socket error symbol as on WIN32 plateform.
     *
	 *	For multiplateform source code.
	 */
	typedef int SOCKET;
	#define SOCKET_ERROR (-1)

	/*! @def DEBUG
     *	@ingroup System
	 *	@brief Define the gcc like debug symbol (WIN32 only).
     *
	 *	In order to have portable DEBUG support, we need to define a common
	 *	debug symbol. We choose to use the debug symbol used by gcc : DEBUG.
	 */
	#if defined DEBUG
		#undef DEBUG
		#define DEBUG
	#endif

	/*! @def WIN32
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	In order to be sure to have the correct WIN32 symbol.
	 */
	#if defined WIN32
		#define WIN32	// WIN32 is more interesting for us
	#endif

	/*! @def strcasecmp
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	For multiplateform source code.
	 */

	/*! @def strncasecmp
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	For multiplateform source code.
	 */

	/*! @def _snprintf
     *	@ingroup System
	 *	@brief Define the WIN32 symbol
     *
	 *	For snprintf source code.
	 */

	#include <unistd.h>
	#include <pthread.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

// #include <System/TrackingMemoryLeaks.h>

namespace Omiscid {


// Almost ugly for the moment
#ifdef OMISCID_TRACE_ENABLE
	#define OMISCID_TRACE_IS_ENABLED 1
#else
	#define OMISCID_TRACE_IS_ENABLED 0
#endif

/** @fct TraceError
	*  @brief used to warn messages
	*/
inline void TraceError(const char * format, ... )
{
	va_list args;
	va_start( args, format );
	vfprintf( stderr, format, args );
	va_end( args );
}

inline void Trace(const char * format, ... )
{
	va_list args;
	va_start( args, format );
	vprintf( format, args );
	va_end( args );
}

/* 

#define TraceError if (OMISCID_TRACE_IS_ENABLED)	TraceErrorRealCall
#define Trace	   if (OMISCID_TRACE_IS_ENABLED)	TraceRealCall

*/
#define TraceError  !(OMISCID_TRACE_IS_ENABLED) ? (void)0 : TraceError
#define Trace		!(OMISCID_TRACE_IS_ENABLED) ? (void)0 : Trace

#ifdef WIN32

// For Time function port under Windows
struct timezone {
       int  tz_minuteswest; /* minutes W of Greenwich */
       int  tz_dsttime;     /* type of dst correction */
};

#endif	// ifdef WIN32

} // namespace Omiscid

#endif	// __SYSTEM_CONFIG_H__
