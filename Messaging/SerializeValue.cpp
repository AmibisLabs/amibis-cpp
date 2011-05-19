#include <Messaging/SerializeValue.h>
#include <Messaging/SerializeException.h>

using namespace Omiscid;

SerializeValue::SerializeValue()
{
}

SerializeValue::SerializeValue( const SerializeValue& Val )
{
	*this = Val; // .InternalValue;
}

SerializeValue::SerializeValue(const json_spirit::Value& Val )
{
	operator=( Val );
}

SerializeValue::SerializeValue( const int Val )
{
	operator=( Val );
}

SerializeValue::SerializeValue( const unsigned int Val )
{
	throw SerializeException("unsigned int not supported, please send as a string", SerializeException::IllegalTypeConversion );
}

SerializeValue::SerializeValue( const bool Val )
{
	operator=( Val );
}

SerializeValue::SerializeValue( const double Val )
{
	operator=( Val );
}

SerializeValue::SerializeValue( const float Val )
{
	operator=( Val );
}

SerializeValue::SerializeValue( const SimpleString& Val )
{
	operator=( Val );
}

SerializeValue::SerializeValue( const char * Val )
{
	operator=( Val );
}

SerializeValue::operator json_spirit::Value()
{
	return *this;
}

SerializeValue::operator int()
{
	if ( type() != json_spirit::int_type )
	{
		throw SerializeException("Object is not an int", SerializeException::IllegalTypeConversion );
	}

	return get_int();
}

SerializeValue::operator unsigned int()
{
	throw SerializeException("unsigned int not supported, please send as a string", SerializeException::UnsupportedType );
	return 0;
}

SerializeValue::operator bool()
{
	if ( type() != json_spirit::bool_type )
	{
		throw SerializeException("Value is not a bool", SerializeException::IllegalTypeConversion );
	}

	return get_bool();
}

SerializeValue::operator double()
{
	if ( type() != json_spirit::real_type )
	{
		throw SerializeException("Value is not a double", SerializeException::IllegalTypeConversion );
	}

	return get_real();
}

SerializeValue::operator float()
{
	if ( type() != json_spirit::real_type )
	{
		throw SerializeException("Value is not a float", SerializeException::IllegalTypeConversion );
	}

	return (float)get_real();
}

SerializeValue::operator SimpleString()
{
	if ( type() != json_spirit::str_type )
	{
		throw SerializeException("Value is not a string", SerializeException::IllegalTypeConversion );
	}

	return SimpleString( get_str().c_str() );
}

SerializeValue::operator char*()
{
	if ( type() != json_spirit::str_type )
	{
		throw SerializeException("Value is not a string", SerializeException::IllegalTypeConversion );
	}

	return strdup((char*)get_str().c_str());
}

SerializeValue& SerializeValue::operator=( const SerializeValue& Val )
{
	*((json_spirit::Value*)this) = *((json_spirit::Value*)&Val);
	return *this;
}

SerializeValue& SerializeValue::operator=( const json_spirit::Value& Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( Val );
	return *this;
}

SerializeValue& SerializeValue::operator=( const int Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( Val );
	return *this;
}

SerializeValue& SerializeValue::operator=( const unsigned int Val )
{
	throw SerializeException("unsigned int not supported, please send as a string", SerializeException::UnsupportedType );
}

SerializeValue& SerializeValue::operator=( const bool Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( Val );
	return *this;
}

SerializeValue& SerializeValue::operator=( const double Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( Val );
	return *this;
}

SerializeValue& SerializeValue::operator=( const float Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( Val );
	return *this;
}

SerializeValue& SerializeValue::operator=( const SimpleString& Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( Val.GetStr() );
	return *this;
}

SerializeValue& SerializeValue::operator=( const char* Val )
{
	*((json_spirit::Value*)this) = json_spirit::Value( (char*)Val );
	return *this;
}

bool SerializeValue::IsAnObject() const
{
	return (type() == json_spirit::obj_type);
}

bool SerializeValue::IsASimpleValue() const
{
	return (type() != json_spirit::obj_type);
}

bool SerializeValue::IsNullValue() const
{
	return (type() == json_spirit::null_type);
}

bool SerializeValue::IsAnArray() const
{
	return (type() == json_spirit::array_type);
}

// int management
	// Encoding functions
	SerializeValue Omiscid::SerializeInt( int Data )
	{
		return SerializeValue( Data );
	}
	SerializeValue Omiscid::SerializeIntFromAddress( void * pTmpData )
	{
		return SerializeValue( *(static_cast<int*>(pTmpData)) );
	}
	// Decoding functions
	int Omiscid::UnserializeInt( const SerializeValue& Val )
	{
		return Val.get_int();
	}
	void Omiscid::UnserializeIntFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<int*>(pTmpData)) = UnserializeInt(Val);
	}

// short int management
	// Encoding functions
	SerializeValue Omiscid::SerializeShortInt( short int Data )
	{
		return SerializeValue( (int)Data );
	}
	SerializeValue Omiscid::SerializeShortIntFromAddress( void * pTmpData )
	{
		return SerializeValue( (int)(*(static_cast<short int*>(pTmpData))) );
	}
	// Decoding functions
	short int Omiscid::UnserializeShortInt( const SerializeValue& Val )
	{
		return (short int)Val.get_int();
	}
	void Omiscid::UnserializeShortIntFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<short int*>(pTmpData)) = UnserializeShortInt( Val );
	}

// unsigned int management
	// Encoding functions
	SerializeValue Omiscid::SerializeUnsignedInt( unsigned int Data )
	{
		throw SerializeException("unsigned int not supported, please send as a string", SerializeException::UnsupportedType );
		return SerializeValue();
	}
	SerializeValue Omiscid::SerializeUnsignedIntFromAddress( void * pTmpData )
	{
		throw SerializeException("unsigned int not supported, please send as a string", SerializeException::UnsupportedType );
		return SerializeValue();
	}
	// Decoding functions
	unsigned int Omiscid::UnserializeUnsignedInt( const SerializeValue& Val )
	{
		throw SerializeException("unsigned int not supported, please send as a string", SerializeException::UnsupportedType );
		return 0;
	}
	void Omiscid::UnserializeUnsignedIntFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		throw SerializeException("unsigned int not supported, please send as a string", SerializeException::UnsupportedType );
	}

// unsigned short management
	// Encoding functions
	SerializeValue Omiscid::SerializeUnsignedShort( unsigned short Data )
	{
		return SerializeValue( (int)Data );
	}
	SerializeValue Omiscid::SerializeUnsignedShortFromAddress( void * pTmpData )
	{
		return SerializeValue( (int)(*(static_cast<unsigned short*>(pTmpData))) );
	}
	// Decoding functions
	unsigned short Omiscid::UnserializeUnsignedShort( const SerializeValue& Val )
	{
		return (unsigned short)Val.get_int();
	}
	void Omiscid::UnserializeUnsignedShortFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<unsigned short*>(pTmpData)) = UnserializeUnsignedShort( Val );
	}

// char management
	// Encoding functions
	SerializeValue Omiscid::SerializeChar( char Data )
	{
		return SerializeValue( (int)Data );
	}
	SerializeValue Omiscid::SerializeCharFromAddress( void * pTmpData )
	{
		return SerializeValue( (int)(*(static_cast<char*>(pTmpData))) );
	}
	// Decoding functions
	char Omiscid::UnserializeChar( const SerializeValue& Val )
	{
		return (char)Val.get_int();
	}
	void Omiscid::UnserializeCharFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<char*>(pTmpData)) = UnserializeChar(Val);
	}

// unsigned char management
	// Encoding functions
	SerializeValue Omiscid::SerializeUnsignedChar( unsigned char Data )
	{
		return SerializeValue( (int)Data );
	}
	SerializeValue Omiscid::SerializeUnsignedChar( void * pTmpData )
	{
		return SerializeValue( (int)(*(static_cast<unsigned char*>(pTmpData))) );
	}
	// Decoding functions
	unsigned char Omiscid::UnserializeUnsignedChar( const SerializeValue& Val )
	{
		return (unsigned char)Val.get_int();
	}
	void Omiscid::UnserializeUnsignedCharFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<unsigned char*>(pTmpData)) = UnserializeUnsignedChar( Val );
	}

// double management
	// Encoding functions
	SerializeValue Omiscid::SerializeDouble( double Data )
	{
		return SerializeValue( Data );
	}
	SerializeValue Omiscid::SerializeDoubleFromAddress( void * pTmpData )
	{
		return SerializeValue( (double)(*(static_cast<double*>(pTmpData))) );
	}
	// Decoding functions
	double Omiscid::UnserializeDouble( const SerializeValue& Val )
	{
		return (double)Val.get_real();
	}
	void Omiscid::UnserializeDoubleFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<double*>(pTmpData)) = UnserializeDouble( Val );
	}

// float management
	// Encoding functions
	SerializeValue Omiscid::SerializeFloat( float Data )
	{
		return SerializeValue( (double)Data );
	}
	SerializeValue Omiscid::SerializeFloatFromAddress( void * pTmpData )
	{
		return SerializeValue( (double)(*(static_cast<float*>(pTmpData))) );
	}
	// Decoding functions
	float Omiscid::UnserializeFloat( const SerializeValue& Val )
	{
		return (float)Val.get_real();
	}
	void Omiscid::UnserializeFloatFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<float*>(pTmpData)) = UnserializeFloat( Val );
	}

// bool management
	// Encoding functions
	SerializeValue Omiscid::SerializeBool( bool Data )
	{
		return SerializeValue( Data );
	}
	SerializeValue Omiscid::SerializeBoolFromAddress( void * pTmpData )
	{
		return SerializeValue( *(static_cast<bool*>(pTmpData)) );
	}
	// Decoding functions
	bool Omiscid::UnserializeBool( const SerializeValue& Val )
	{
		return (bool)Val.get_bool();
	}
	void Omiscid::UnserializeBoolFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<bool*>(pTmpData)) = UnserializeBool( Val );
	}

// SimpleString management
	// Encoding functions
	SerializeValue Omiscid::SerializeSimpleString( SimpleString& Data )
	{
		return SerializeValue( Data.GetStr() );
	}
	SerializeValue Omiscid::SerializeSimpleStringFromAddress( void * pTmpData )
	{
		return SerializeValue( (*(static_cast<SimpleString*>(pTmpData))).GetStr() );
	}
	// Decoding functions
	SimpleString Omiscid::UnserializeSimpleString( const SerializeValue& Val )
	{
		return SimpleString( Val.get_str().c_str() );
	}
	void Omiscid::UnserializeSimpleStringFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<SimpleString*>(pTmpData)) = Val.get_str().c_str();
	}

// char * management
	// Encoding functions
	SerializeValue Omiscid::SerializeCharStar( char * Data )
	{
		return SerializeValue( Data );
	}
	SerializeValue Omiscid::SerializeCharStarFromAddress( void * pTmpData )
	{
		// here the address is the adress of the (char *) pointer NOT the char pointer
		return SerializeValue( *(static_cast<char**>(pTmpData)) );
	}
	// Decoding functions
	char * Omiscid::UnserializeCharStar( const SerializeValue& Val )
	{
		return (char*)strdup(Val.get_str().c_str());
	}
	void Omiscid::UnserializeCharStarFromAddress( const SerializeValue& Val, void * pTmpData )
	{
		*(static_cast<char**>(pTmpData)) = UnserializeCharStar( Val );
	}
