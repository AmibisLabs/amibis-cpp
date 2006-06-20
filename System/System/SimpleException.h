/**
 * @file SimpleException.h
 * @brief Definition of SimpleException class
 */
#ifndef __SIMPLE_EXCEPTION_H__
#define __SIMPLE_EXCEPTION_H__ 

#include <System/Portage.h>

namespace Omiscid {

/**
 * @class SimpleException SimpleException.h System/SimpleException.h
 * @brief Exception raised by the object of the PRIMA system library
 * 
 * This class is a mother class for exceptions.
 * All derived classes must define their own exception type (const char*)
 * by rewriting the virtual function GetExceptionType.
 * <br>Inherited by SocketException raised by Socket objects 
 * and SimpleListException raised by SimpleList object.
 * \author Domnique Vaufreydaz
 * \author Sebastien Pesnel
 */
class SimpleException
{
public:
  /** \brief Constructor 
   *
   * Create a new SimpleException.
   * \param m [in] message for the error. This string is copied.
   * \param i [in] value for error number
   */
  SimpleException(const char* m, int i = -1);

  /** \brief Copy Constructor */
  SimpleException(const SimpleException& ExceptionToCopy);

  /** \brief Destructor */
  virtual ~SimpleException();
  
  /** \brief Display the message followed by the error number on stderr. */
  void Display();

  /** \brief The virtual function to return the exception type.
   *  \return A const char * pointer to a human readable exception type, usualy the class name.
   *
   *  All classes derived from SimpleException must described themselves by
   *  implementing this fonction.
   */
  virtual const char* GetExceptionType();
  
  char* msg; /*!< string message */
  int err; /*!< error number (for example value returned by errno)*/
};

} // namespace Omiscid

#endif /* __SIMPLE_EXCEPTION_H__ */
