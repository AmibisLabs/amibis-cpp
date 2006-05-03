#include <ServiceControl/ControlUtils.h>
#include <stdlib.h>
#include <time.h>

#ifdef WIN32
	#ifdef USE_AFX
		#include "StdAfx.h"
	#else
		#define _WINSOCKAPI_   /* Prevent inclusion of winsock.h in windows.h */
		#include <windows.h>
	#endif
#else
	#include <sys/time.h>
#endif

#ifdef WIN32
	#include <System/Portage.h>
	#include <process.h>
#else
	#include <sys/types.h>
	#include <unistd.h>
#endif

class OmiscidRandomInitClass
{
public:
	OmiscidRandomInitClass()
	{
		struct timeval t;    
		gettimeofday(&t, NULL);

#ifdef WIN32
		srand(t.tv_sec ^ t.tv_usec);
#else
		srandom(t.tv_sec ^ t.tv_usec);
#endif
	};
};

static OmiscidRandomInitClass OmiscidRandomInitClassInitialisationObject;

unsigned int ControlUtils::GeneratePeerId()
{
  struct timeval t;    
  gettimeofday(&t, NULL);

  unsigned int res = t.tv_sec << 16;

#ifdef WIN32
  res += (0x0000FFFF & rand());
#else
  res += (0x0000FFFF & random());
#endif

  return res;
}

int ControlUtils::StrToInt(const unsigned char* buffer, int len)
{
  return atoi((const char*)buffer);
}

void ControlUtils::IntToStr(int value, SimpleString& str)
{
  str = "";
  str += value;
}
