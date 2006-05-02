#include <System/SocketException.h>

SocketException::SocketException(const char* m, int i)
 : SimpleException(m, i)
{}

SocketException::SocketException(const SocketException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{}


SocketException::~SocketException() 
{}
