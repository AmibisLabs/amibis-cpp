#include <System/Portage.h>

#ifdef WIN32

#include <sys/timeb.h>
#include <math.h>

#endif

using namespace Omiscid;

namespace Omiscid {

class OmiscidRandomInitClass
{
public:
	OmiscidRandomInitClass()
	{
		RandomInit();
	};

};


static OmiscidRandomInitClass OmiscidRandomInitClassInitialisationObject;

} // namespace Omiscid


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
TemporaryMemoryBuffer::TemporaryMemoryBuffer( size_t SizeOfBuffer )
{
	Buffer = new OMISCID_TLM char[SizeOfBuffer];
	if ( Buffer == NULL )
	{
		BufferSize = 0;
		throw MemoryBufferException( "No more memory to allocate buffer" );
	}
	BufferSize = SizeOfBuffer;
}

TemporaryMemoryBuffer::~TemporaryMemoryBuffer()
{
	if ( Buffer != NULL )
	{
		delete [] Buffer;
	}
}

TemporaryMemoryBuffer::operator char*() const
{
	return (char*)Buffer;
}

TemporaryMemoryBuffer::operator unsigned char*() const
{
	return (unsigned char*)Buffer;
}

size_t TemporaryMemoryBuffer::GetLength() const
{
	return BufferSize;
}

#ifdef WIN32

int Omiscid::random()
{
	int result;

#if RAND_MAX <= 32767
	int i;
	short * where = (short*)&result;
	for( i = 0; i < (sizeof(result)/sizeof(short)); i++ )
	{
		where[i] = (short)rand();
	}
#else
	result = rand();
#endif
	return result;
}

int Omiscid::gettimeofday(struct timeval * tv,struct timezone * tz )
{
	struct _timeb tb;
	_ftime(&tb);

	tv->tv_sec  = (long)tb.time;
	tv->tv_usec = tb.millitm * (long)1000;

	return 0;
}

#endif // WIN32

#if defined WIN32 || defined __APPLE__

// Add libc extension from gnu to Windows/Mac OSX

void * Omiscid::memrchr( const void * Buf, int c, size_t size )
{
	char *	TmpChar;
	size_t		Pos;
	char	TmpC;

	if ( Buf == NULL || size <= 0 )
	{
		return NULL;
	}

	TmpChar = (char*)Buf;
	TmpC = (char)c;

	for( Pos = size-1; Pos >= 0; Pos-- )
	{
		if ( TmpChar[Pos] == TmpC )
		{
			return (void*)&TmpChar[Pos];
		}
	}
	return NULL;
}

#endif // defined WIN32 || defined __APPLE__

#ifndef __APPLE__

// Port of this interesting apple function
size_t Omiscid::strlcpy(char *dst, const char *src, size_t size)
{
	size_t i;
	size_t max;

	if ( dst == NULL || src == NULL || size == 0 )
	{
		return (size_t)0;
	}

	// max pos to copy (we want to keep a place for '\0'
	max = size-1;

	// copy data
	for( i = 0; i < max && src[i] != '\0'; i++ )
	{
		dst[i] = src[i];
	}

	// Just to be sure, let's put a '\0' at end
	dst[i] = '\0';

	return i;
}

#endif


void Omiscid::RandomInit()
{
		struct timeval t;    
		gettimeofday(&t, NULL);

#ifdef WIN32
		srand(t.tv_sec ^ t.tv_usec ^ (long int)GetCurrentThreadId());
#else	// WIN32
		srandom(t.tv_sec ^ t.tv_usec ^ (long int)pthread_self() );
#endif	// WIN32
}

SimpleString Omiscid::GetLoggedUser()
{
	SimpleString Login;

#ifdef WIN32
	#define MAX_LOGIN_LEN 512

	DWORD len;
	TemporaryMemoryBuffer UserName(MAX_LOGIN_LEN);	// for debugging purpose

	// init data to empty login
	*((char*)UserName) = '\0';
	len = MAX_LOGIN_LEN-1;

	GetUserName( UserName, &len );
	Login = UserName;

	#undef MAX_LOGIN_LEN
#else
	// alternatively we could use getpwuid( geteuid() );
	Login = getenv("LOGNAME");
#endif

	return Login;
}


