#include <Messaging/StructuredMessage.h>

using namespace std;
using namespace Omiscid;

  /** @brief Constructor
  */
StructuredMessage::StructuredMessage() : Serializer()
{
}

 /** @brief Constructor
  */
StructuredMessage::StructuredMessage( const SerializeValue& SerValue )
{
	Serializer = SerValue;
}

 /** @brief Constructor
  */
StructuredMessage::StructuredMessage( SerializeValue& SerValue )
{
	Serializer = SerValue;
}

 /** @brief Constructor from a Message received
  */
StructuredMessage::StructuredMessage( const Message& Msg )
{
	std::string sTmp =  Msg.GetBuffer();

	if( !json_spirit::read(sTmp, Serializer) && (Serializer.type() != json_spirit::obj_type) )
	{
		throw SerializeException("Argument is not a valid serialization stream", SerializeException::MalformedStream );
	}
}

 /** @brief Constructor from a Message received
  */
StructuredMessage::StructuredMessage( Message& Msg )
{
	std::string sTmp =  Msg.GetBuffer();

	if( !json_spirit::read(sTmp, Serializer) && (Serializer.type() != json_spirit::obj_type) )
	{
		throw SerializeException("Argument is not a valid serialization stream", SerializeException::MalformedStream );
	}
}

 /** @brief Copy Constructor
  */
StructuredMessage::StructuredMessage( const StructuredMessage& SMsg )
{
	Serializer = SMsg.Serializer;
}

 /** @brief Copy Constructor
  */
StructuredMessage::StructuredMessage( StructuredMessage& SMsg )
{
	Serializer = SMsg.Serializer;
}


 /** @brief Copy Constructor
  */
StructuredMessage::StructuredMessage( SimpleString& SMsg )
{
	std::string sTmp =  SMsg.GetStr();

	if( !json_spirit::read(sTmp, Serializer) && (Serializer.type() != json_spirit::obj_type) )
	{
		throw SerializeException("Argument is not a valid serialization stream", SerializeException::MalformedStream );
	}
}

 /** @brief Copy Constructor
  */
StructuredMessage:: StructuredMessage( const SimpleString& SMsg )
{
	std::string sTmp =  SMsg.GetStr();

	if( !json_spirit::read(sTmp, Serializer) && (Serializer.type() != json_spirit::obj_type) )
	{
		throw SerializeException("Argument is not a valid serialization stream", SerializeException::MalformedStream );
	}
}

  /** @brief Desctructor
  */
StructuredMessage::~StructuredMessage()
{
}

 /** operator=
  */
StructuredMessage& StructuredMessage::operator=( SerializeValue& SerValue )
{
	Serializer = SerValue;

	return *this;
}

 /** operator=
  */
StructuredMessage& StructuredMessage::operator=( const SerializeValue& SerValue )
{
	Serializer = SerValue;

	return *this;
}

 /** operator=
  */
StructuredMessage& StructuredMessage::operator=( StructuredMessage& sMsg )
{
	Serializer = sMsg.Serializer;

	return *this;
}

bool StructuredMessage::IsAnObject() const
{
	return (Serializer.type() == json_spirit::obj_type);
}

bool StructuredMessage::IsASimpleValue() const
{
	return (Serializer.type() != json_spirit::obj_type);
}

bool StructuredMessage::IsNullValue() const
{
	return (Serializer.type() == json_spirit::null_type);
}

  /** \find Find an element identified by Key
  * @param Key [in] the key to identifies the pair.
  * @return true if found, false otherwise
  */
SerializeObjectConstIterator StructuredMessage::Find( const SimpleString& Key ) const
{
	if ( IsASimpleValue() )
	{
		throw SimpleException( "Could not find a key : not an object" );
	}

	SerializeObjectConstIterator it;
	const SerializeObject & Parser = Serializer.get_obj();

	for (it = Parser.begin(); it!=Parser.end(); ++it)
	{
		if ( same_name( *it, Key ) == true )
		{
			return it;
		}
	}

	throw SimpleException( "Key not found" );

	// Will be Parser.end(), just to make compiler happy
	return it;
}

 /** \find Find an element identified by Key
  * @param Key [in] the key to identifies the pair.
  * @return true if found, false otherwise
  */
SerializeObjectIterator StructuredMessage::Find( const SimpleString& Key )
{
	if ( IsASimpleValue() )
	{
		throw SimpleException( "Could not find a key : not an object" );
	}

	SerializeObjectIterator it;
	SerializeObject & Parser = (SerializeObject &)Serializer.get_obj();

	for (it = Parser.begin(); it!=Parser.end(); ++it)
	{
		if ( same_name( *it, Key ) == true )
		{
			return it;
		}
	}

	throw SimpleException( "Key not found" );

	// Will be Parser.end(), just to make compiler happy
	return it;
}

 /** \find Find an element value hashed by Key
  * @param Key [in] the key to identifies the pair.
  * @return a value
  */
SerializeValue StructuredMessage::FindAndGetValue( const SimpleString& Key ) const
{
	return (*Find(Key)).value_;
}

void StructuredMessage::Put( const SimpleString Key, const SerializeValue& Val )
{
	if ( IsNullValue() ||  IsAnObject() == false )
	{
		SerializeObject EmptyObject;
		Serializer = SerializeValue( EmptyObject );
	}
	Serializer.get_obj().push_back( SerializePair( Key.GetStr(), Val ) );
}

void StructuredMessage::Put( const SimpleString Key, const StructuredMessage& Val )
{
	Put( Key, (SerializeValue)Val );
}

void StructuredMessage::Put( const SimpleString Key, const SerializeArray& Val )
{
	Put( Key, SerializeValue(Val) );
}

void StructuredMessage::Put( const SimpleString Key, const SerializeObject& Val )
{
	Put( Key, SerializeValue(Val) );
}
