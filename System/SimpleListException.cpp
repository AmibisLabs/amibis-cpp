#include <System/SimpleListException.h>

using namespace Omiscid;

  
SimpleListException::SimpleListException(const char* m)
 : SimpleException(m, -1)
{}

SimpleListException::SimpleListException(const SimpleListException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{}

const char* SimpleListException::GetExceptionType()
{
	return "SimpleListException"; 
};
