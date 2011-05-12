#include <Messaging/Serializable.h>

using namespace Omiscid;


Serializable::Serializable()
	: SerializationDeclared(false)
{
}

Serializable::~Serializable()
{
	SmartLocker SL_this((const LockableObject&)*this);

	while( SerialiseMapping.IsNotEmpty() )
	{
		delete SerialiseMapping.ExtractFirst();
	}
}

#if 0

 /** \find Find an element identified by Key
  * \param Key [in] the key to identifies the pair.
  * \return true if found, false otherwise
  */
SerializeObjectIterator Serializable::Find( const SimpleString& Key, SerializeObject& SerializeManager )
{
	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	SerializeObjectIterator it;

	for (it = SerializeManager.begin(); it!=SerializeManager.end(); ++it)
	{
		if ( same_name( *it, Key ) == true )
		{
			break;
		}
	}

	// Will be JSonManager.end()
	return it;
}

#endif

Serializable::EncodeMapping * Serializable::Find( const SimpleString& Key )
{
	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	for( SerialiseMapping.First(); SerialiseMapping.NotAtEnd(); SerialiseMapping.Next() )
	{
		EncodeMapping * tmpMapping = SerialiseMapping.GetCurrent();
		if ( tmpMapping->Key.EqualsCaseInsensitive( Key ) )
		{
			return tmpMapping;
		}
	}

	return (Serializable::EncodeMapping*)NULL;
}

Serializable::EncodeMapping * Serializable::Create( const SimpleString& Key )
{
	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Find( Key );

	if ( tmpMapping == (Serializable::EncodeMapping*)NULL )
	{
		// Create and add structure to the list
		tmpMapping = new EncodeMapping;
		tmpMapping->Key = Key;
		SerialiseMapping.AddTail(tmpMapping);
	}
	else
	{
		SimpleString Msg = "Replacing serialise mapping for " + Key;
		throw SimpleException( Msg );
	}

	return tmpMapping;
}

void Serializable::AddToSerialization( const SimpleString& Key, int& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = SerializeIntFromAddress;
	tmpMapping->FunctionToDecode = UnserializeIntFromAddress;
}

void Serializable::AddToSerialization( const SimpleString& Key, bool& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = SerializeBoolFromAddress;
	tmpMapping->FunctionToDecode = UnserializeBoolFromAddress;
}

void Serializable::AddToSerialization( const SimpleString& Key, SimpleString& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = SerializeSimpleStringFromAddress;
	tmpMapping->FunctionToDecode = UnserializeSimpleStringFromAddress;
}

void Serializable::AddToSerialization( const SimpleString& Key, char *& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = SerializeCharStarFromAddress;
	tmpMapping->FunctionToDecode = UnserializeCharStarFromAddress;
}

SerializeValue Serializable::Serialize()
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	StructuredMessage MySMsg;

	for( SerialiseMapping.First(); SerialiseMapping.NotAtEnd(); SerialiseMapping.Next() )
	{
		Serializable::EncodeMapping * tmpMapping = SerialiseMapping.GetCurrent();

		MySMsg.Put( tmpMapping->GetKey(), tmpMapping->Encode() );
	}

	return (SerializeValue)MySMsg;
}

void Serializable::Unserialize( const SerializeValue& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

/*
	json_spirit::Value JSonMsg;
	string sJSONMsg(SerializationMessage.GetStr());

	if( !json_spirit::read(sJSONMsg, JSonMsg) && (JSonMsg.type() != json_spirit::obj_type) ) 
	{
		throw SimpleException("Argument is not a valid serialization stream", 0 );
	}


	// Parse serialising objet
	for( SerialiseMapping.First(); SerialiseMapping.NotAtEnd(); SerialiseMapping.Next() )
	{
		Serializable::EncodeMapping * tmpMapping = SerialiseMapping.GetCurrent();

		tmpMapping->Decode( SMsg.FindAndGetValue( tmpMapping->Key ) );
	}*/
}

