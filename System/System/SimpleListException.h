/**
 * @file SimpleListException.h
 * @ingroup System
 * @brief Definition of SimpleListException class
 */

#ifndef __SIMPLE_LIST_EXCEPTION_H__
#define __SIMPLE_LIST_EXCEPTION_H__

#include <System/ConfigSystem.h>
#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class SimpleListException SimpleListException.h System/SimpleListException.h
 * @brief Exception thrown by the class SimpleList.
 * @author Sebastien Pesnel
 */
class SimpleListException : public SimpleException
{
public:
  /** @brief Constructor
   * @param m [in] message for the exception
   */
  SimpleListException(const SimpleString m, int i = UnkownSimpleExceptionCode );

  /** @brief Copy Constrcutor */
  SimpleListException(const SimpleListException& ExceptionToCopy);

	// Virtual destructor always
	virtual ~SimpleListException();

  /** @brief Return a human readable exception type */
  SimpleString GetExceptionType() const;
};

} // namespace Omiscid

#endif // __SIMPLE_LIST_EXCEPTION_H__

