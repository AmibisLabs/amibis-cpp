#include <stdio.h>
#include <string.h>

#include <System/Config.h>
#include <System/SimpleException.h>

using namespace Omiscid;

SimpleException::SimpleException(const char* m, int i) 
{
	TraceError( "%s : %d\n", m, i );

	if ( m != NULL )
	{
		size_t msgLen = strlen(m);
		msg = new char[msgLen+1];
		if ( msg )
		{
			strcpy( msg, m );
		}
	}
	err = i;
}

SimpleException::SimpleException(const SimpleException& ExceptionToCopy)
{ 
	if ( ExceptionToCopy.msg != NULL )
	{
		size_t msgLen = strlen(ExceptionToCopy.msg);
		msg = new char[msgLen+1];
		if ( msg )
		{
			strcpy( msg, ExceptionToCopy.msg );
		}
	}
	err = ExceptionToCopy.err;
}


SimpleException::~SimpleException() 
{ 
  if ( msg != NULL )
  {   
    delete [] msg;
  }
}

void SimpleException::Display()
{
  TraceError( "SimpleException: %s (%d)\n", msg, err);
}

const char* SimpleException::GetExceptionType() 
{
	return "SimpleException";
}
