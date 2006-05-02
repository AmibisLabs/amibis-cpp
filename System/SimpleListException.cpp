#include <System/SimpleListException.h>

  
SimpleListException::SimpleListException(const char* m)
 : SimpleException(m, -1)
{}

SimpleListException::SimpleListException(const SimpleListException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{}

