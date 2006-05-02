//      -*- C++ -*-
/**
 * @file MsgSocketException.h
 * @brief Definition of MsgSocketException class
 */
#ifndef MSGSOCKET_EXCEPTION_H 
#define MSGSOCKET_EXCEPTION_H 

#include <System/SimpleException.h>

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

//////////// inline methods ///////////////
#ifndef RAVI_INTERFACE

inline const char* MsgSocketException::GetExceptionType()
{ return "MsgSocketException"; };

#endif /* RAVI_INTERFACE */

#endif /* MSGSOCKET_EXCEPTION_H */
