/**
 * @file SocketException.h
 * @ingroup System
 * @brief Definition of SocketException class
 */

#ifndef __SOCKET_EXCEPTION_H__
#define __SOCKET_EXCEPTION_H__

#include <System/ConfigSystem.h>
#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class SocketException SocketException.h System/SocketException.h
 * @brief Exception thrown by the class Socket.
 * @author Domnique Vaufreydaz
 * @author Sebastien Pesnel
 */
class SocketException : public SimpleException
{
public:
  /** @brief Constructor
   * @param m [in] the message for the exception
   * @param i [in] the number for the error
   */
  SocketException(const SimpleString m, int i = -1);

  /** @brief Copy Constructor
   * @param ExceptionToCopy [in] the exception to copy
   */
  SocketException(const SocketException& ExceptionToCopy);

	// Virtual destructor always
	virtual ~SocketException();

  /** @brief Return a human readable exception type */
  SimpleString GetExceptionType() const;

protected:
  void SocketErrorCheck();
};

} // namespace Omiscid

#endif // __SOCKET_EXCEPTION_H__

