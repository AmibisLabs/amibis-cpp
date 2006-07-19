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

} // namespace Omiscid

static OmiscidRandomInitClass OmiscidRandomInitClassInitialisationObject;

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
	Buffer = new char[SizeOfBuffer];
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

#define MAX_LOGIN_LEN 512

#endif // WIN32


void Omiscid::RandomInit()
{
		struct timeval t;    
		gettimeofday(&t, NULL);

#ifdef WIN32
		srand(t.tv_sec ^ t.tv_usec);
#else	// WIN32
		srandom(t.tv_sec ^ t.tv_usec);
#endif	// WIN32
}

SimpleString Omiscid::GetLoggedUser()
{
	SimpleString Login;

#ifdef WIN32
	DWORD len;
	TemporaryMemoryBuffer UserName(MAX_LOGIN_LEN);	// for debugging purpose

	// init data to empty login
	*((char*)UserName) = '\0';
	len = MAX_LOGIN_LEN-1;

	GetUserName( UserName, &len );
	Login = UserName;

#else
	// alternatively we could use getpwuid( geteuid() );
	Login = getenv("LOGNAME");
#endif

	return Login;
}


