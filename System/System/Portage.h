#ifndef PORTAGE_H
#define PORTAGE_H

#include <stdio.h>
#include <stdarg.h>

// Ugly for the moment
#if defined DEBUG || defined _DEBUG
	// in debug mode, we plan to trace every thing
	inline int TraceError(char * format, ... )
	{
	   va_list args;
	   va_start( args, format );
	   vprintf( format, args );
	   va_end( args );
	   return 0;
	}
#else
	// in release mode, we do not print nothing
	inline int TraceError(...)
	{
	   return 0;
	}
#endif

#endif
