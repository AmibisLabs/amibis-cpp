/**
 * @file System/Portage.h
 * @ingroup System
 * @brief Definition of portable functions for multiplateforme purpose
 */

#ifndef __PORTAGE_H__
#define __PORTAGE_H__

#include <System/Config.h>
#include <System/SimpleString.h>
#include <System/SimpleException.h>

namespace Omiscid {

#ifdef WIN32

// Time function port under Windows
int gettimeofday(struct timeval*tv,struct timezone*tz ); // tz is ignored on windows plateforms

// random port
int random();

#endif	// ifdef WIN32

// RandomInit()
void RandomInit();

// Retrieve the logged username
SimpleString GetLoggedUser();

// classes for Allocate/Unallocation memory buffer
class MemoryBufferException : public SimpleException
{
public:
	// Virtual destructor always
	virtual ~MemoryBufferException();

	MemoryBufferException( SimpleString Msg, int i = UnkownSimpleExceptionCode );
	MemoryBufferException::MemoryBufferException( const MemoryBufferException& MemoryBufferExceptionToCopy );

	SimpleString GetExceptionType() const;
};

class TemporaryMemoryBuffer
{
public:
	TemporaryMemoryBuffer( size_t SizeOfBuffer );
	virtual ~TemporaryMemoryBuffer();

	operator char*() const;
	operator unsigned char*() const;

private:
	char * Buffer;
	size_t BufferSize;
};


} // namespace Omiscid

#endif	// __PORTAGE_H__
