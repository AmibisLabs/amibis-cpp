/**
 * \file Messaging/Serializable.h
 * \ingroup Messaging
 * \brief Definition of Serializable class
 * \author Dominique Vaufreydaz
 */
#ifndef __SERIALIZABLE_H__
#define __SERIALIZABLE_H__

// Desable Warning C4290: Visual C++ does not implement checked exceptions, 
// C4290 is just informing you that other exceptions may still be throw from 
// these functions
#ifdef WIN32
#pragma warning(disable: 4290)
#endif

#include <System/ReentrantMutex.h>
#include <System/SimpleList.h>
#include <System/SimpleString.h>

#include <Messaging/SerializeValue.h>
#include <Messaging/StructuredMessage.h>

#include <vector>
#include <list>

namespace Omiscid {

class Serializable : protected ReentrantMutex {
public:

	Serializable();

	virtual ~Serializable();

	virtual void DeclareSerializeMapping() = 0;
	
	void AddToSerialization( const SimpleString& Key, int& Val );
	void AddToSerialization( const SimpleString& Key, unsigned int& Val );
	void AddToSerialization( const SimpleString& Key, short int& Val );
	void AddToSerialization( const SimpleString& Key, unsigned short& Val );
	void AddToSerialization( const SimpleString& Key, double& Val );
	void AddToSerialization( const SimpleString& Key, float& Val );
	void AddToSerialization( const SimpleString& Key, bool& Val );
	void AddToSerialization( const SimpleString& Key, SimpleString& Val );
	void AddToSerialization( const SimpleString& Key, char*& Val );

	// For recursive purpose
	void AddToSerialization( const SimpleString& Key, Serializable& Val );

	template <typename CurrentType>
	void AddToSerialization( const SimpleString& Key, SimpleList<CurrentType>& Val );

	template <typename CurrentType>
	void AddToSerialization( const SimpleString& Key, std::vector<CurrentType>& Val );

	template <typename CurrentType>
	void AddToSerialization( const SimpleString& Key, std::list<CurrentType>& Val );

	SerializeValue Serialize();
	void Unserialize( const SerializeValue& Val );

protected:
	/** \brief Callback for the encoding function */
    typedef SerializeValue (*SerializeFunction)(void *);

	/** \brief Callback for the decoding function */
	typedef void (*UnserializeFunction)(const SerializeValue&, void *);

	class EncodeMapping
	{
	public:
		SimpleString Key;
		SerializeFunction FunctionToEncode;
		UnserializeFunction FunctionToDecode;
		void * AddressOfObject;

		inline const char * GetKey()
		{
			return Key.GetStr();
		}

		inline SerializeValue Encode()
		{
			return FunctionToEncode(AddressOfObject);
		}

		inline void Decode( const SerializeValue &Val )
		{
			FunctionToDecode( Val, AddressOfObject );
		}
	};

	SimpleList<EncodeMapping*> SerialiseMapping;

	// Find in local mapping
	EncodeMapping * Find( const SimpleString& Key );

	// Create in local mapping
	EncodeMapping * Create( const SimpleString& Key ) throw (SimpleException);

	bool SerializationDeclared;
	inline void CallDeclareSerializeMappingIfNeeded()
	{
		if ( SerializationDeclared == true )
		{
			return;
		}

		SerializationDeclared = true;
		DeclareSerializeMapping();
	}
};


template <typename CurrentType>
void Serializable::AddToSerialization( const SimpleString& Key, SimpleList<CurrentType>& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = (SerializeFunction)SerializeSimpleListFromAddress<CurrentType>;
	tmpMapping->FunctionToDecode = (UnserializeFunction)UnserializeSimpleListFromAddress<CurrentType>;
}

template <typename CurrentType>
void Serializable::AddToSerialization( const SimpleString& Key, std::vector<CurrentType>& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = (SerializeFunction)SerializeStdVectorFromAddress<CurrentType>;
	tmpMapping->FunctionToDecode = (UnserializeFunction)UnserializeStdVectorFromAddress<CurrentType>;
}

template <typename CurrentType>
void Serializable::AddToSerialization( const SimpleString& Key, std::list<CurrentType>& Val )
{
	SmartLocker SL_this((const LockableObject&)*this);

	// Check if SerializeMappingIsDone
	CallDeclareSerializeMappingIfNeeded();

	Serializable::EncodeMapping * tmpMapping = Create( Key );

	// Fill (new) structure
	tmpMapping->AddressOfObject = (void*)&Val;
	tmpMapping->FunctionToEncode = (SerializeFunction)SerializeStdListFromAddress<CurrentType>;
	tmpMapping->FunctionToDecode = (UnserializeFunction)UnserializeStdListFromAddress<CurrentType>;
}

} // Omiscid 

#endif //__SERIALIZABLE_H__

