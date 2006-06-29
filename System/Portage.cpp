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

#ifdef WIN32

int Omiscid::random()
{
	int result;
	int i;

	if ( RAND_MAX <= 32767 )	// 16 bits generator
	{
		short * where = (short*)&result;
		for( i = 0; i < (sizeof(result)/sizeof(short)); i++ )
		{
			where[i] = (short)rand();
		}
	}
	else							// >32 nits generator
	{
		result = rand();
	}

	return result;
}

int Omiscid::gettimeofday(struct timeval * tv,struct timezone * tz )
{
	struct _timeb tb;
	_ftime(&tb);

	tv->tv_sec  = tb.time;
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
	char * UserName = new char[MAX_LOGIN_LEN];	// for debugging purpose
	if ( UserName == NULL )
		// No more memory
		return Login;

	// init data
	UserName[0] = '\0';
	len = MAX_LOGIN_LEN-1;

	GetUserName( UserName, &len );
	Login = UserName;

	// delete buffer
	delete UserName;
#else
	// alternatively we could use getpwuid( geteuid() );
	Login = getenv("LOGNAME");
#endif

	return Login;
}
