/**
 * @file MEssaging/Messaging/SerializeException.h
 * \ingroup Messaging
 * \ingroup UserFriendlyAPI
 * @brief Definition of SerializeException class
 */

#ifndef __SERIALIZE_EXCEPTION_H__
#define __SERIALIZE_EXCEPTION_H__

#include <Messaging/ConfigMessaging.h>

#include <System/SimpleException.h>

namespace Omiscid {

/**
 * @class SerializeException SerializeException.h Messaging/SerializeException.h
 * \ingroup Messaging
 * @brief Exception thrown by the class SimpleList.
 * @author Rémi Barraquand
 */
class SerializeException : public SimpleException
{
public:
  enum StructuredMessageExceptionCode { Exception = 0, IllegalTypeConversion, InvalidFormat, MalformedStream, UnsupportedType, UnknownField };

  /** @brief Constructor
  *
   * @param m [in] message for the exception
   */
  SerializeException(const Omiscid::SimpleString m, int i = UnkownSimpleExceptionCode );

  /** @brief Copy Constrcutor */
  SerializeException(const SerializeException& ExceptionToCopy);

  /** @brief Virtual destructor always
  */
  virtual ~SerializeException();

  /** \=@brief Return a human readable exception type
  */
  SimpleString GetExceptionType() const;
};

} // Omiscid

#endif // __SERIALIZE_EXCEPTION_H__

