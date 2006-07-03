//      -*- C++ -*-
/**
 * @file MsgSocketException.h
 * @brief Definition of MsgSocketException class
 */
#ifndef __MSG_SOCKET_EXCEPTION_H__
#define __MSG_SOCKET_EXCEPTION_H__ 

#include <System/Config.h>
#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class MsgSocketException MsgSocketException.h Com/MsgSocketException.h
 * @brief Exception thrown by the class MsgSocket.
 */
class MsgSocketException : public SimpleException
{
public:
  /** @brief Constructor
   * @param m [in] message for the exception
   * @param i [in] the error number
   */
  MsgSocketException(const char* m, int i = -1);

  /** @brief Copy Constructor
   * @param ExceptionToCopy [in] the exception to copy
   */
  MsgSocketException(const MsgSocketException& ExceptionToCopy);

  /** @brief Return a human readable exception type */
  const char* GetExceptionType();
};

} // namespace Omiscid

#endif // __MSG_SOCKET_EXCEPTION_H__
