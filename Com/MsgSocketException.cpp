#include <Com/MsgSocketException.h>

using namespace Omiscid;

MsgSocketException::MsgSocketException(const char* m, int i)
  : SimpleException(m, i)
{}

MsgSocketException::MsgSocketException(const MsgSocketException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{}

const char* MsgSocketException::GetExceptionType()
{
	return "MsgSocketException";
};
