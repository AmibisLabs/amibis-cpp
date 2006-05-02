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
	#include <process.h>
#else
	#include <sys/types.h>
	#include <unistd.h>
#endif

unsigned int ControlUtils::GenerateServiceId()
{
#ifdef WIN32
  static unsigned int number = 0;
  unsigned int res = getpid() << 16;

  res = res | (0x0000FFFF & number++ );
#else
  struct timeval t;    
  gettimeofday(&t, NULL);

  unsigned int res = t.tv_sec << 16;
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
