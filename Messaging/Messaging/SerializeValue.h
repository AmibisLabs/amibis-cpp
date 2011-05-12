#ifndef __SERIALIZE_VALUE_H__
#define __SERIALIZE_VALUE_H__

#include <Messaging/SerializeManager.h>

#include <vector>
#include <list>

namespace Omiscid {

class SerializeValue : public json_spirit::Value
{
public:
	SerializeValue();
	SerializeValue( const SerializeValue& Val );
	SerializeValue( const json_spirit::Value& Val );
	SerializeValue( const int Val );
	SerializeValue( const unsigned int Val );
	SerializeValue( const bool Val );
	SerializeValue( const double Val );
	SerializeValue( const float Val );
	SerializeValue( const SimpleString& Val );
	SerializeValue( const char * Val );

	operator json_spirit::Value();
	operator int();
	operator unsigned int();
	operator bool();
	operator double();
	operator float();
	operator SimpleString();
	operator char*();

	SerializeValue& operator=( const SerializeValue& Val );
 	SerializeValue& operator=( const json_spirit::Value& Val );
	SerializeValue& operator=( const int Val );
	SerializeValue& operator=( const unsigned int Val );
	SerializeValue& operator=( const bool Val );
	SerializeValue& operator=( const double Val );
	SerializeValue& operator=( const float Val );
	SerializeValue& operator=( const SimpleString& Val );
	SerializeValue& operator=( const char* Val );

	bool IsAnObject() const;
	bool IsASimpleValue() const;
	bool IsNullValue() const;
	bool IsAnArray() const;
};

// int management
	// Encoding functions
	SerializeValue SerializeInt( int Data );
	SerializeValue SerializeIntFromAddress( void * pData );
	// Decoding functions
	int UnserializeInt( const SerializeValue& Val );
	void UnserializeIntFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( int Data ) { return SerializeInt(Data); }
	inline void Unserialize( const SerializeValue& Val, int * pData ) { UnserializeIntFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, int& Data ) { Data = UnserializeInt(Val); };

// short int management
	// Encoding functions
	SerializeValue SerializeShortInt( short int Data );
	SerializeValue SerializeShortIntFromAddress( void * pData );
	// Decoding functions
	short int UnserializeShortInt( const SerializeValue& Val );
	void UnserializeShortIntFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( short int Data ) { return SerializeShortInt(Data); }
	inline void Unserialize( const SerializeValue& Val, short int * pData ) { UnserializeShortIntFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, short int& Data ) { Data = UnserializeShortInt(Val); };

// unsigned int management
// unsigned are not supported due to incompatibility among programming language
	// Encoding functions
	SerializeValue SerializeUnsignedInt( unsigned int Data );
	SerializeValue SerializeUnsignedIntFromAddress( void * pData );
	// Decoding functions
	unsigned int UnserializeUnsignedInt( const SerializeValue& Val );
	void UnserializeUnsignedIntFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( unsigned int Data ) { return SerializeUnsignedInt(Data); }
	inline void Unserialize( const SerializeValue& Val, unsigned int * pData ) { UnserializeUnsignedIntFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, unsigned int& Data ) { Data = UnserializeUnsignedInt(Val); };

// unsigned short management
	// Encoding functions
	SerializeValue SerializeUnsignedShort( unsigned short Data );
	SerializeValue SerializeUnsignedShortFromAdress( void * pData );
	// Decoding functions
	unsigned short UnserializeUnsignedShort( const SerializeValue& Val );
	void UnserializeUnsignedShortFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( unsigned short Data ) { return SerializeUnsignedShort(Data); }
	inline void Unserialize( const SerializeValue& Val, unsigned short * pData ) { UnserializeUnsignedShortFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, unsigned short& Data ) { Data = UnserializeUnsignedShort(Val); };

// char management
	// Encoding functions
	SerializeValue SerializeChar( char Data );
	SerializeValue SerializeCharFromAddress( void * pData );
	// Decoding functions
	char UnserializeChar( const SerializeValue& Val );
	void UnserializeCharFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( char Data ) { return SerializeChar(Data); }
	inline void Unserialize( const SerializeValue& Val, char * pData ) { UnserializeCharFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, char& Data ) { Data = UnserializeChar(Val); };

// unsigned char management
	// Encoding functions
	SerializeValue SerializeUnsignedChar( unsigned char Data );
	SerializeValue SerializeUnsignedChar( void * pData );
	// Decoding functions
	unsigned char UnserializeUnsignedChar( const SerializeValue& Val );
	void UnserializeUnsignedCharFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( unsigned char Data ) { return SerializeUnsignedChar(Data); }
	inline void Unserialize( const SerializeValue& Val, unsigned char * pData ) { UnserializeUnsignedCharFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, unsigned char& Data ) { Data = UnserializeUnsignedChar(Val); };

// double management
	// Encoding functions
	SerializeValue SerializeDouble( double Data );
	SerializeValue SerializeDoubleFromAddress( void * pData );
	// Decoding functions
	double UnserializeDouble( const SerializeValue& Val );
	void UnserializeDoubleFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( double Data ) { return SerializeDouble(Data); }
	inline void Unserialize( const SerializeValue& Val, double * pData ) { UnserializeDoubleFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, double& Data ) { Data = UnserializeDouble(Val); };

// float management
	// Encoding functions
	SerializeValue SerializeFloat( float Data );
	SerializeValue SerializeFloatFromAddress( void * pData );
	// Decoding functions
	float UnserializeFloat( const SerializeValue& Val );
	void UnserializeFloatFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( float Data ) { return SerializeFloat(Data); }
	inline void Unserialize( const SerializeValue& Val, float * pData ) { UnserializeFloatFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, float& Data ) { Data = UnserializeFloat(Val); };

// bool management
	// Encoding functions
	SerializeValue SerializeBool( bool Data );
	SerializeValue SerializeBoolFromAddress( void * pData );
	// Decoding functions
	bool UnserializeBool( const SerializeValue& Val );
	void UnserializeBoolFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( bool Data ) { return SerializeBool(Data); }
	inline void Unserialize( const SerializeValue& Val, bool * pData ) { UnserializeBoolFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, bool& Data ) { Data = UnserializeBool(Val); };

// SimpleString management
	// Encoding functions
	SerializeValue SerializeSimpleString( SimpleString& Data );
	SerializeValue SerializeSimpleStringFromAddress( void * pData );
	// Decoding functions
	SimpleString UnserializeSimpleString( const SerializeValue& Val );
	void UnserializeSimpleStringFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( SimpleString& Data ) { return SerializeSimpleString(Data); }
	inline void Unserialize( const SerializeValue& Val, SimpleString * pData ) { UnserializeSimpleStringFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, SimpleString& Data ) { Data = UnserializeSimpleString(Val); };

// char * management
	// Encoding functions
	SerializeValue SerializeCharStar( char * Data );
	SerializeValue SerializeCharStarFromAddress( void * pData );
	// Decoding functions
	char * UnserializeCharStar( const SerializeValue& Val );
	void UnserializeCharStarFromAddress( const SerializeValue& Val, void * pData );
	// Generic versions
	inline SerializeValue Serialize( char * Data ) { return SerializeCharStar(Data); }
	inline void Unserialize( const SerializeValue& Val, char ** pData ) { UnserializeCharStarFromAddress(Val,(void*)pData); };
	inline void Unserialize( const SerializeValue& Val, char *& Data ) { Data = UnserializeCharStar(Val); };

// SimleList management
	// Encoding functions
	template <typename TYPE_NAME> SerializeValue SerializeSimpleList( SimpleList<TYPE_NAME>& Data )
	{
		SerializeArray ValArray;
		for( Data.First(); Data.NotAtEnd(); Data.Next() )
		{
			ValArray.push_back( Serialize(Data.GetCurrent()) );
		}
		return SerializeValue(ValArray);
	}

	template <typename TYPE_NAME> SerializeValue SerializeSimpleListFromAddress( SimpleList<TYPE_NAME> * pData )
	{
		SerializeArray ValArray;
		for( pData->First(); pData->NotAtEnd(); pData->Next() )
		{
			ValArray.push_back( Serialize(pData->GetCurrent()) );
		}
		return SerializeValue(ValArray);
	}
	// Decoding functions
	template <typename TYPE_NAME> SimpleList<TYPE_NAME> UnserializeSimpleList( const SerializeValue& Val )
	{
		if ( Val.type() != json_spirit::array_type )
		{
			throw SimpleException( "Mauvais type de paramètre" );
		}
		const SerializeArray& ValArray = Val.get_array();
		SerializeArrayConstIterator it;
		SimpleList<TYPE_NAME> ResultList;
		TYPE_NAME Listelement;

		for( it = ValArray.begin(); it != ValArray.end();++it )
		{
			Unserialize( *it, &Listelement );
			ResultList.AddTail( Listelement );
		}

		return ResultList;
	}
	template <typename TYPE_NAME> void UnserializeSimpleListFromAddress( const SerializeValue& Val, SimpleList<TYPE_NAME> * pData )
	{
		if ( Val.type() != json_spirit::array_type )
		{
			throw SimpleException( "Mauvais type de paramètre" );
		}
		const SerializeArray& ValArray = Val.get_array();
		SerializeArrayConstIterator it;
		TYPE_NAME Listelement;

		pData->Empty();
		for( it = ValArray.begin(); it != ValArray.end(); ++it )
		{
			Unserialize( *it, &Listelement );
			pData->AddTail( Listelement );
		}
	}
	// Generic versions
	template <typename TYPE_NAME> inline SerializeValue Serialize( SimpleList<TYPE_NAME>& Data ) { return SerializeSimpleListFromAddress<TYPE_NAME>(&Data); }
	template <typename TYPE_NAME> inline void Unserialize( const SerializeValue& Val, SimpleList<TYPE_NAME> * pData ) { UnserializeSimpleListFromAddress<TYPE_NAME>(Val,(void*)pData); };
	template <typename TYPE_NAME> inline void Unserialize( const SerializeValue& Val, SimpleList<TYPE_NAME>& Data ) { UnserializeSimpleListFromAddress<TYPE_NAME>(Val,&Data); };

// std::vector management
	// Encoding functions
	template <typename TYPE_NAME> SerializeValue SerializeStdVector( std::vector<TYPE_NAME>& Data )
	{
		SerializeArray ValArray;
		typename std::vector<TYPE_NAME>::const_iterator it;
		for( it = Data.begin(); it != Data.end(); ++it )
		{
			ValArray.push_back( Serialize(*it) );
		}
		return SerializeValue(ValArray);
	}

	template <typename TYPE_NAME> SerializeValue SerializeStdVectorFromAddress( std::vector<TYPE_NAME> * pData )
	{
		SerializeArray ValArray;
		typename std::vector<TYPE_NAME>::const_iterator it;
		for( it = pData->begin(); it != pData->end(); ++it )
		{
			ValArray.push_back( Serialize(*it) );
		}
		return SerializeValue(ValArray);
	}
	// Decoding functions
	template <typename TYPE_NAME> std::vector<TYPE_NAME> UnserializeStdVector( const SerializeValue& Val )
	{
		if ( Val.type() != json_spirit::array_type )
		{
			throw SimpleException( "Mauvais type de paramètre" );
		}
		const SerializeArray& ValArray = Val.get_array();
		SerializeArrayConstIterator it;
		typename std::vector<TYPE_NAME> ResultVector;
		TYPE_NAME Listelement;

		for( it = ValArray.begin(); it != ValArray.end();++it )
		{
			Unserialize( *it, &Listelement );
			ResultVector.push_back( Listelement );
		}

		return ResultVector;
	}
	template <typename TYPE_NAME> void UnserializeStdVectorFromAddress( const SerializeValue& Val, std::vector<TYPE_NAME> * pData )
	{
		if ( Val.type() != json_spirit::array_type )
		{
			throw SimpleException( "Mauvais type de paramètre" );
		}
		const SerializeArray& ValArray = Val.get_array();
		SerializeArrayConstIterator it;
		TYPE_NAME Listelement;

		pData->clear();
		for( it = ValArray.begin(); it != ValArray.end(); ++it )
		{
			Unserialize( *it, &Listelement );
			pData->push_back( Listelement );
		}
	}
	// Generic versions
	template <typename TYPE_NAME> inline SerializeValue Serialize( std::vector<TYPE_NAME>& Data ) { return SerializeStdVectorFromAddress<TYPE_NAME>(&Data); }
	template <typename TYPE_NAME> inline void Unserialize( const SerializeValue& Val, std::vector<TYPE_NAME> * pData ) { UnserializeStdVectorFromAddress<TYPE_NAME>(Val,(void*)pData); };
	template <typename TYPE_NAME> inline void Unserialize( const SerializeValue& Val, std::vector<TYPE_NAME>& Data ) { UnserializeStdVectorFromAddress<TYPE_NAME>(Val,&Data); };

// std::list management
	// Encoding functions
	template <typename TYPE_NAME> SerializeValue SerializeStdList( std::list<TYPE_NAME>& Data )
	{
		SerializeArray ValArray;
		typename std::list<TYPE_NAME>::const_iterator it;
		for( it = Data.begin(); it != Data.end(); ++it )
		{
			ValArray.push_back( Serialize(*it) );
		}
		return SerializeValue(ValArray);
	}

	template <typename TYPE_NAME> SerializeValue SerializeStdListFromAddress( std::list<TYPE_NAME> * pData )
	{
		SerializeArray ValArray;
		typename std::list<TYPE_NAME>::const_iterator it;
		for( it = pData->begin(); it != pData->end(); ++it )
		{
			ValArray.push_back( Serialize(*it) );
		}
		return SerializeValue(ValArray);
	}
	// Decoding functions
	template <typename TYPE_NAME> std::list<TYPE_NAME> UnserializeStdList( const SerializeValue& Val )
	{
		if ( Val.type() != json_spirit::array_type )
		{
			throw SimpleException( "Mauvais type de paramètre" );
		}
		const SerializeArray& ValArray = Val.get_array();
		SerializeArrayConstIterator it;
		typename std::list<TYPE_NAME> ResultList;
		TYPE_NAME Listelement;

		for( it = ValArray.begin(); it != ValArray.end();++it )
		{
			Unserialize( *it, &Listelement );
			ResultList.push_back( Listelement );
		}

		return ResultList;
	}
	template <typename TYPE_NAME> void UnserializeStdListFromAddress( const SerializeValue& Val, std::list<TYPE_NAME> * pData )
	{
		if ( Val.type() != json_spirit::array_type )
		{
			throw SimpleException( "Mauvais type de paramètre" );
		}
		const SerializeArray& ValArray = Val.get_array();
		SerializeArrayConstIterator it;
		TYPE_NAME Listelement;

		pData->clear();
		for( it = ValArray.begin(); it != ValArray.end(); ++it )
		{
			Unserialize( *it, &Listelement );
			pData->push_back( Listelement );
		}
	}
	// Generic versions
	template <typename TYPE_NAME> inline SerializeValue Serialize( std::list<TYPE_NAME>& Data ) { return SerializeStdListFromAddress<TYPE_NAME>(&Data); }
	template <typename TYPE_NAME> inline void Unserialize( const SerializeValue& Val, std::list<TYPE_NAME> * pData ) { UnserializeStdListFromAddress<TYPE_NAME>(Val,(void*)pData); };
	template <typename TYPE_NAME> inline void Unserialize( const SerializeValue& Val, std::list<TYPE_NAME>& Data ) { UnserializeStdListFromAddress<TYPE_NAME>(Val,&Data); };

}; // Omiscid

#endif // __SERIALIZE_VALUE_H__

