#include <ServiceControl/ControlUtils.h>

#include <stdlib.h>
#include <time.h>

using namespace Omiscid;

int ControlUtils::StrToInt(const unsigned char* buffer, int len)
{
  return atoi((const char*)buffer);
}

void ControlUtils::IntToStr(int value, SimpleString& str)
{
  str = "";
  str += value;
}
