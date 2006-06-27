#include <System/Portage.h>

#ifdef WIN32

#include <sys/timeb.h>

using namespace Omiscid;

int Omiscid::gettimeofday(struct timeval * tv,struct timezone * tz )
{
	struct _timeb tb;
	_ftime(&tb);

	tv->tv_sec  = tb.time;
	tv->tv_usec = tb.millitm * (long)1000;

	return 0;
}

#endif // WIN32

#ifdef WIN32
#define MAX_LOGIN_LEN 512
#endif

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
	len = MAX_LOGIN_LEN;

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


