//      -*- C++ -*-
/**
 * @file MsgSocketException.h
 * @brief Definition of MsgSocketException class
 */
#ifndef MSGSOCKET_EXCEPTION_H 
#define MSGSOCKET_EXCEPTION_H 

#include <System/Portage.h>
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

#endif /* MSGSOCKET_EXCEPTION_H */
