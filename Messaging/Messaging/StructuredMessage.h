/**
 * @file Messaging/StructuredMessage.h
 * \ingroup Messaging
 * @brief Definition of Structured Message type and function
 */
#ifndef __STRUCTURED_MESSAGE_H__
#define __STRUCTURED_MESSAGE_H__

// Desable Warning C4290: Visual C++ does not implement checked exceptions,
// C4290 is just informing you that other exceptions may still be throw from
// these functions
#ifdef WIN32
#pragma warning(disable: 4290)
// Warning azbout multiple copy constructors (C4521)
#pragma warning(disable: 4521)
#endif

#include <System/SimpleString.h>

#include <Messaging/SerializeValue.h>
#include <Messaging/SerializeException.h>

#include <Com/Message.h>

namespace Omiscid {

/**
 * @class StructuredMessage StructuredMessage.h Messaging/StructuredMessage.h
 *
 * @author Dominique Vaufreydaz
 */
class StructuredMessage {
public:
  /** @brief Constructor
  */
  StructuredMessage();

 /** @brief Constructor
  */
  StructuredMessage( const SerializeValue& SerValue );
 /** @brief Constructor
  */
  StructuredMessage( SerializeValue& SerValue );

 /** @brief Constructor from a Message received
  */
  StructuredMessage( const Message& Msg );
 /** @brief Constructor from a Message received
  */
  StructuredMessage( Message& Msg );

 /** @brief Copy Constructor
  */
  StructuredMessage( StructuredMessage& SMsg );
 /** @brief Copy Constructor
  */
  StructuredMessage( const StructuredMessage& SMsg );

 /** @brief Copy Constructor
  */
  StructuredMessage( SimpleString& SMsg );
 /** @brief Copy Constructor
  */
  StructuredMessage( const SimpleString& SMsg );

  /** @brief Desctuctor
  */
  ~StructuredMessage();

  operator SerializeValue() const
  {
	  return Serializer;
  }

 /** \find Find an element value hashed by Key
  * @param Key [in] the key to identifies the pair.
  * @return a value
  */
  operator SerializeValue()
  {
	  return Serializer;
  }

  bool IsAnObject() const;
  bool IsASimpleValue() const;
  bool IsNullValue() const;
  bool IsAnArray() const;

  void Put( const SimpleString Key, const SerializeArray& Val );
  void Put( const SimpleString Key, const StructuredMessage& Val );
  void Put( const SimpleString Key, const SerializeValue& Val );
  void Put( const SimpleString Key, const SerializeObject& Val );

  operator SimpleString()
  {
	  return SimpleString(json_spirit::write_formatted(Serializer).c_str());
  }

 /** \find Find an element value hashed by Key
  * @param Key [in] the key to identifies the pair.
  * @return a value
  */
  SerializeValue FindAndGetValue( const SimpleString& Key ) const;

 /** operator=
  */
  StructuredMessage& operator=( SerializeValue& SerValue );

 /** operator=
  */
  StructuredMessage& operator=( const SerializeValue& SerValue );

 /** operator=
  */
  StructuredMessage& operator=( StructuredMessage& sMsg );

protected:
 /** \find Find an element identified by Key
  * @param Key [in] the key to identifies the pair.
  * @return an iterator
  */
  SerializeObjectConstIterator Find( const SimpleString& Key ) const;

 /** \find Find an element identified by Key
  * @param Key [in] the key to identifies the pair.
  * @return an iterator
  */
  SerializeObjectIterator Find( const SimpleString& Key );

  SerializeValue Serializer;
};

} // Omiscid

#endif //__STRUCTURED_MESSAGE_H__
