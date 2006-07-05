
#include <System/SimpleException.h>

#include <string.h>

using namespace Omiscid;

SimpleException::SimpleException(const SimpleString m, int i) 
{
	TraceError( "%s : %d\n", m.GetStr(), i );

	msg = m;
	err = i;
}

SimpleException::SimpleException(const SimpleException& ExceptionToCopy)
{ 
	if ( ExceptionToCopy.msg.IsEmpty() )
	{
		msg = ExceptionToCopy.msg;
	}
	err = ExceptionToCopy.err;
}


SimpleException::~SimpleException() 
{ 
}

void SimpleException::Display() const
{
  TraceError( "SimpleException: %s (%d)\n", msg.GetStr(), err);
}

SimpleString SimpleException::GetExceptionType() const 
{
	return SimpleString("SimpleException");
}
