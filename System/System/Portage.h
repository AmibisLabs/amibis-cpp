/**
 * @file System/Portage.h
 * @ingroup System
 * @brief Definition of portable functions for multiplateforme purpose
 */

#ifndef __PORTAGE_H__
#define __PORTAGE_H__

#include <System/ConfigSystem.h>
#include <System/SimpleString.h>
#include <System/SimpleException.h>
#include <System/TemporaryMemoryBuffer.h>

namespace Omiscid {

#ifdef WIN32

	// Time function port under Windows
	int gettimeofday(struct timeval*tv,struct timezone*tz ); // tz is ignored on windows plateforms

	// random port
	int random();

#endif	// ifdef WIN32

#if defined WIN32 || defined __APPLE__

	// Add libc extension from gnu to Windows/Mac OSX
	void * memrchr( const void * Buf, int c, size_t size );
#endif


#ifndef __APPLE__
	// Port of the apple function
	size_t strlcpy(char *dst, const char *src, size_t size);
#endif

// RandomInit()
void RandomInit();

// Retrieve the logged username
SimpleString GetLoggedUser();

// retreive the local host name
SimpleString GetLocaHostName();

} // namespace Omiscid

#endif	// __PORTAGE_H__
