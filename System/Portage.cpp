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

#endif
