/**
* @file System/TemporaryMemoryBuffer.cpp
* @ingroup System
* @brief Implementation of TemporaryMemoryBuffer and MemoryBufferException classes.
*/

#include <System/TemporaryMemoryBuffer.h>

using namespace Omiscid;

MemoryBufferException::MemoryBufferException( SimpleString Msg, int i )
	: SimpleException( Msg, i )
{
}

MemoryBufferException::~MemoryBufferException()
{
}

MemoryBufferException::MemoryBufferException( const MemoryBufferException& MemoryBufferExceptionToCopy )
	: SimpleException( MemoryBufferExceptionToCopy )
{
}

SimpleString MemoryBufferException::GetExceptionType() const
{
	return SimpleString("MemoryBufferException");
}

// class for Allocate/Unallocation memory buffer
TemporaryMemoryBuffer::TemporaryMemoryBuffer()
{
	Buffer = (char*)NULL;
	LogicalBufferSize = 0;
	RealBufferSize = 0;
}

// class for Allocate/Unallocation memory buffer
TemporaryMemoryBuffer::TemporaryMemoryBuffer( size_t SizeOfBuffer )
{
	Buffer = new OMISCID_TLM char[SizeOfBuffer+1];	// +1 to prevent buffer overflow using snprintf function for instance
	if ( Buffer == NULL )
	{
		LogicalBufferSize = 0;
		throw MemoryBufferException( "No more memory to allocate buffer" );
	}
	LogicalBufferSize = SizeOfBuffer;
}

TemporaryMemoryBuffer::~TemporaryMemoryBuffer()
{
	if ( Buffer != (char*)NULL )
	{
		delete [] Buffer;
	}
}

void TemporaryMemoryBuffer::SetNewBufferSize( size_t NewSizeOfBuffer, bool KeepData /* = false */ )
{
	if ( NewSizeOfBuffer > LogicalBufferSize && KeepData == true )
	{
		throw MemoryBufferException( "Could not not keep data in a smaller buffer" );
	}

	// If buffer is not large enougth, get a new buffer
	if ( NewSizeOfBuffer > RealBufferSize )
	{
		if ( KeepData == false )
		{
			// simply destroy previous buffer
			if ( Buffer != (char*) NULL )
			{
				delete [] Buffer;
			}

			Buffer = new OMISCID_TLM char[NewSizeOfBuffer+1]; // +1 to prevent going out of buffer
			if ( Buffer == (char*)NULL )
			{
				LogicalBufferSize = 0;
				RealBufferSize = 0;
				throw MemoryBufferException( "No more memory to allocate buffer" );
			}
		}
		else
		{
			char * TmpBuf = new OMISCID_TLM char[NewSizeOfBuffer+1];
			if ( TmpBuf == (char*)NULL )
			{
				// data and buffer remain the same
				throw MemoryBufferException( "No more memory to allocate new buffer" );
			}

			if ( LogicalBufferSize != 0 )
			{
				// Copy data
				memcpy( TmpBuf, Buffer, LogicalBufferSize );
			}

			delete [] Buffer;
			Buffer = TmpBuf;
		}

		// Set the new real size of the buffer because new buffer is larger
		RealBufferSize = NewSizeOfBuffer;
	}

	// In all case, the new size if Set
	LogicalBufferSize = NewSizeOfBuffer;
}

TemporaryMemoryBuffer::operator char*() const
{
	if ( Buffer == (char*)NULL )
	{
		throw MemoryBufferException( "Unitialized memory buffer" );
	}
	return (char*)Buffer;
}

TemporaryMemoryBuffer::operator unsigned char*() const
{
	if ( Buffer == (char*)NULL )
	{
		throw MemoryBufferException( "Unitialized memory buffer" );
	}
	return (unsigned char*)Buffer;
}

TemporaryMemoryBuffer::operator void*() const
{
	if ( Buffer == (char*)NULL )
	{
		throw MemoryBufferException( "Unitialized memory buffer" );
	}
	return (void*)Buffer;
}

size_t TemporaryMemoryBuffer::GetLength() const
{
	return LogicalBufferSize;
}
