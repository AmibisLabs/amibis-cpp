#include <Com/MsgSocketException.h>

using namespace Omiscid;

MsgSocketException::MsgSocketException(const SimpleString m, int i)
  : SimpleException(m, i)
{}

MsgSocketException::MsgSocketException(const MsgSocketException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{}

SimpleString MsgSocketException::GetExceptionType() const
{
	return SimpleString( "MsgSocketException" );
}
