/**
 * \file StructuredMessageException.h
 * \ingroup Messaging
 * \brief Definition of StructuredMessageException class
 */

#ifndef __STRUCTURED_MESSAGE_EXCEPTION_H__
#define __STRUCTURED_MESSAGE_EXCEPTION_H__

#include <System/ConfigSystem.h>
#include <System/SimpleException.h>

namespace Omiscid { 

/**
 * \class StructuredMessageException StructuredMessageException.h Messaging/StructuredMessageException.h
 * \ingroup Messaging
 * \brief Exception thrown by the class SimpleList.
 * \author Rémi Barraquand
 */
class StructuredMessageException : public SimpleException
{
public:
  enum StructuredMessageExceptionCode { Exception = 0, IllegalTypeConversion, InvalidFormat, MalformedJSONStream, UnknownField };

  /** \brief Constructor
  *
   * \param m [in] message for the exception
   */
  StructuredMessageException(const Omiscid::SimpleString m, int i = UnkownSimpleExceptionCode );

  /** \brief Copy Constrcutor */
  StructuredMessageException(const StructuredMessageException& ExceptionToCopy);

  /** \brief Virtual destructor always
  */
  virtual ~StructuredMessageException();

  /** \=\brief Return a human readable exception type 
  */
  SimpleString GetExceptionType() const;
};

} // Omiscid

#endif // __STRUCTURED_MESSAGE_EXCEPTION_H__
