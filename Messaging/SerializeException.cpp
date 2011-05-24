/* @file Messaging/SerializeException.cpp
 * @ingroup UserFriendly
 * @date 2004-2011
 * @author Dominique Vaufreydaz
 */

#include <Messaging/SerializeException.h>

using namespace Omiscid;

SerializeException::SerializeException(const SimpleString m, int i)
 : SimpleException(m, i)
{
}

SerializeException::~SerializeException()
{
}

SerializeException::SerializeException(const SerializeException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{
}

SimpleString SerializeException::GetExceptionType() const
{
	SimpleString type;

	switch( err ) {
	  case UnknownField:
		  type = "Unknown Field";
		  break;

	  case InvalidFormat:
		  type = "Invalid Format";
		  break;

	  case MalformedStream:
		  type = "Malformed Json Stream";
		  break;

	  case IllegalTypeConversion:
		  type = "Illegal Type Conversion";
		  break;

	  case Exception:
		  type = "Json Exception";
		  break;

	  default:
		  type = "Unknown Json Exception Type";
		  break;
	}

	return type;
}

