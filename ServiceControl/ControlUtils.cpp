#include <ServiceControl/ControlUtils.h>

#include <time.h>

using namespace Omiscid;

int ControlUtils::StrToInt(const unsigned char* buffer)
{
	if ( buffer == NULL )
	{
		return 0;
	}
	return atoi((const char*)buffer);
}

void ControlUtils::IntToStr(int value, SimpleString& str)
{
  str = value;
}
