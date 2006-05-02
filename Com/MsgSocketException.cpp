#include <Com/MsgSocketException.h>

MsgSocketException::MsgSocketException(const char* m, int i)
  : SimpleException(m, i)
{}


MsgSocketException::MsgSocketException(const MsgSocketException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{}


