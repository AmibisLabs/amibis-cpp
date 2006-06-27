//      -*- C++ -*-
/**
 * @file SocketException.h
 * @brief Definition of SocketException class
 */
#ifndef SOCKET_EXCEPTION_H 
#define SOCKET_EXCEPTION_H 

#include <System/Config.h>
#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class SocketException SocketException.h System/SocketException.h
 * @brief Exception thrown by the class Socket.
 * \author Domnique Vaufreydaz
 * \author Sebastien Pesnel
 */
class SocketException : public SimpleException
{
public:
  /** \brief Constructor 
   * \param m [in] the message for the exception
   * \param i [in] the number for the error
   */
  SocketException(const char* m, int i = -1);

  /** \brief Copy Constructor 
   * \param ExceptionToCopy [in] the exception to copy
   */
  SocketException(const SocketException& ExceptionToCopy);

  /** \brief Destructor */
  ~SocketException();  

  /** \brief Return a human readable exception type */
  const char* GetExceptionType();

protected:
  void SocketErrorCheck();
};

} // namespace Omiscid

#endif /* SOCKET_EXCEPTION_H */
