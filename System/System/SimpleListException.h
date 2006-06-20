/**
 * @file SimpleListException.h
 * @brief Definition of SimpleListException class
 */

#ifndef __SIMPLE_LIST_EXCEPTION_H__
#define __SIMPLE_LIST_EXCEPTION_H__

#include <System/Portage.h>
#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class SimpleListException SimpleListException.h System/SimpleListException.h
 * \brief Exception thrown by the class SimpleList.
 * \author Sebastien Pesnel
 */
class SimpleListException : public SimpleException
{
public:
  /** \brief Constructor 
   * \param m [in] message for the exception
   */
  SimpleListException(const char* m);

  /** \brief Copy Constrcutor */
  SimpleListException(const SimpleListException& ExceptionToCopy);

  /** \brief Return a human readable exception type */
  const char* GetExceptionType();
};

} // namespace Omiscid

#endif /* __SIMPLE_LIST_EXCEPTION_H__ */
