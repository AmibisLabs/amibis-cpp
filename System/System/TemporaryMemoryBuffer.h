/**
 * @file System/TemporaryMemoryBuffer.h
 * @ingroup System
 * @brief interface for TemporaryMemoryBuffer and MemoryBufferException classes.
 */

#ifndef __TEMPORARY_MEMORY_BUFFER_H__
#define __TEMPORARY_MEMORY_BUFFER_H__

#include <System/ConfigSystem.h>
#include <System/SimpleException.h>

namespace Omiscid {

// classes for Allocate/Unallocation memory buffer
class MemoryBufferException : public SimpleException
{
public:
	// Virtual destructor always
	virtual ~MemoryBufferException();

	MemoryBufferException( SimpleString Msg, int i = UnkownSimpleExceptionCode );
	MemoryBufferException( const MemoryBufferException& MemoryBufferExceptionToCopy );

	SimpleString GetExceptionType() const;
};

class TemporaryMemoryBuffer
{
public:
	TemporaryMemoryBuffer();

	TemporaryMemoryBuffer( size_t SizeOfBuffer );

	virtual ~TemporaryMemoryBuffer();

	void SetNewBufferSize( size_t SizeOfBuffer, bool KeepData = false );

	operator char*() const;
	operator void*() const;
	operator unsigned char*() const;

	size_t GetLength() const;

	char * GetBuffer()
	{
		return Buffer;
	}

private:
	char * Buffer;
	size_t LogicalBufferSize;
	size_t RealBufferSize;
};

} // namespace Omiscid

#endif // __TEMPORARY_MEMORY_BUFFER_H__

