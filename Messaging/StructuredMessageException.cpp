#include <Messaging/StructuredMessageException.h>

using namespace Omiscid;
using namespace Messaging;

StructuredMessageException::StructuredMessageException(const SimpleString m, int i)
 : SimpleException(m, i)
{
}

StructuredMessageException::~StructuredMessageException()
{
}

StructuredMessageException::StructuredMessageException(const StructuredMessageException& ExceptionToCopy)
 : SimpleException(ExceptionToCopy)
{
}

SimpleString StructuredMessageException::GetExceptionType() const
{
    SimpleString type;

    switch( err ) {
      case UnknownField: type = "Unknown Field"; break;
      case InvalidFormat: type = "Invalid Format"; break;
      case MalformedJSONStream: type = "Malformed Json Stream"; break;
      case IllegalTypeConversion: type = "Illegal Type Conversion"; break;
      case Exception: type = "Json Exception"; break;
      default: type = "Unknown Json Exception Type";
    }

    return type;
}
