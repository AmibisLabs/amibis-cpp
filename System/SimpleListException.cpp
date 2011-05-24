#include <System/SimpleListException.h>

using namespace Omiscid;


SimpleListException::SimpleListException(const SimpleString m, int i)
 : SimpleException(m, i)
{
}

SimpleListException::~SimpleListException()
{
}

SimpleListException::SimpleListException(const SimpleListException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{
}

SimpleString SimpleListException::GetExceptionType() const
{
	return SimpleString( "SimpleListException" );
}

