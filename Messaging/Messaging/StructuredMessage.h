/**
 * \file Messaging/StructuredMessage.h
 * \ingroup Messaging
 * \brief Definition of Structured Message type and function
 */
#ifndef __STRUCTURED_MESSAGE_H__
#define __STRUCTURED_MESSAGE_H__

// Desable Warning C4290: Visual C++ does not implement checked exceptions, 
// C4290 is just informing you that other exceptions may still be throw from 
// these functions
#ifdef WIN32
#pragma warning(disable: 4290)
#endif

#include <System/SimpleString.h>

#include <Messaging/Json.h>
#include <Messaging/StructuredMessageException.h>
#include <Messaging/Access.h>

#include <vector>

namespace Omiscid { namespace Messaging {

/**
 * \class StructuredMessage StructuredMessage.h Messaging/StructuredMessage.h
 * \ingroup Messaging
 * \brief Group Data about a structured message
 *
 * contain the data about a structured message :
 * 
 * - to be detailed :)
 *
 * \author Remi Barraquand
 * \author Amaury Negre
 */
class StructuredMessage {
public:
  /** \brief Constructor
  */
  StructuredMessage();

  /** \brief Constructor
  *
  * \param msg [in] Copy constructor
  */
  StructuredMessage( const StructuredMessage& Msg);

  /** \brief Constructor
  *
  * \param val [in] Copy constructor
  */
  StructuredMessage( const json_spirit::Object& Obj);

  /** \brief Constructor
  *
  * \param str [in] Construct a Json object from str
  */
  StructuredMessage( const SimpleString Str) throw( StructuredMessageException );

  /** \brief Desctuctor
  */
  ~StructuredMessage();

  /** \brief Overload of the = operator
  */
  const StructuredMessage& operator= ( const StructuredMessage& Msg);

  /** \brief Put a new element in the object
  * \param Key [in] the key that identifies the value
  * \param Val [in] the value to insert.
  */
  virtual void Put( const SimpleString Key, const json_spirit::Value Val );

  /** \brief Put a new element described by a StructuredMessage
   * \param Key [in] the key that identifies the value
   * \param msg [in] the StructuredMessage to insert.
   */
  void Put( const SimpleString Key, StructuredMessage& Msg );

  /** \brief Put a new element of basic_type
   * \param Key [in] the key that identifies the value
   * \param Val [in] the value to insert.
   */
  void Put( const SimpleString Key, const char* Val );
  void Put( const SimpleString Key, const std::string& Val );
  void Put( const SimpleString Key, const json_spirit::Array& Val );
  void Put( const SimpleString Key, const json_spirit::Object& Val );
  void Put( const SimpleString Key, bool Val );
  void Put( const SimpleString Key, unsigned int Val );
  void Put( const SimpleString Key, int Val );
  void Put( const SimpleString Key, unsigned short Val );
  void Put( const SimpleString Key, short Val );
  void Put( const SimpleString Key, unsigned char Val );
  void Put( const SimpleString Key, char Val );
  void Put( const SimpleString Key, double Val );
  void Put( const SimpleString Key, float Val );

  void Put( const SimpleString Key, const std::vector<std::string> & Vec );
  void Put( const SimpleString Key, const std::vector<unsigned int> & Vec );
  void Put( const SimpleString Key, const std::vector<int> & Vec );
  void Put( const SimpleString Key, const std::vector<unsigned short> & Vec );
  void Put( const SimpleString Key, const std::vector<short> & Vec );
  void Put( const SimpleString Key, const std::vector<unsigned char> & Vec );
  void Put( const SimpleString Key, const std::vector<char> & Vec );
  void Put( const SimpleString Key, const std::vector<bool> & Vec );
  void Put( const SimpleString Key, const std::vector<double> & Vec );
  void Put( const SimpleString Key, const std::vector<float> & Vec );
  
  /** \brief Put an object of any type in the object
   * \param Key [in] the key that identifies the value
   * \param msg [in] the StructuredMessage to insert.
   */
  template <class C>
  void Put( const Omiscid::SimpleString Key, const C& obj);
  
  /** \brief Put a vector of object of any type in the object
   * \param Key [in] the key that identifies the value
   * \param msg [in] the StructuredMessage to insert.
   */
  template <class C>
    void Put( const Omiscid::SimpleString Key, const std::vector<C>& Vec);

  /** \brief Remove an element from the object
  * \param Key [in] the key that identifies the value to remove
  * \return true if removed one, false otherwise.
  */
  bool Pop( const SimpleString Key );

  /** \brief Set the value of an element, identified by Key, in the object
  * \param Key [in] the key that identifies the value to remove
  * \param Val [in] the new value.
  * \remark If no element identified by Key exist, this one will be created. However to create
  * a new element we will prefer to use Put method (Fastest)
  */
  void Set( const SimpleString Key, json_spirit::Value Val );

  template <typename T>
    void Set( const SimpleString Key, const T & Val );
  
  /** \brief Check if an element is identified by Key
  * \param Key [in] the key to identifies the value we are looking for.
  * \return true if found, false otherwise
  */
  bool Has( const SimpleString Key ) const;

  /** \brief Check the object contain a Pair(Key, Val)
  * \param Key [in] the key to identifies the pair.
  * \param Val [in] the value of the pair.
  * \return true if found, false otherwise
  */
  bool Has( const SimpleString Key, json_spirit::Value Val ) const;
  
  /** \brief Clear the Structured message
  */
  virtual void Clear();
  
  /** \brief Get the first value identifies by Key
  * \param Key [in] the key to identifies the value we are looking for.
  * \return the value if found, json_spirit::Null otherwise
  */
  json_spirit::Value Get( const SimpleString Key ) const;
  
  void Get( const SimpleString Key, std::string& Val ) const;
  void Get( const SimpleString Key, bool & Val ) const;
  void Get( const SimpleString Key, unsigned int & Val ) const;
  void Get( const SimpleString Key, int & Val ) const;
  void Get( const SimpleString Key, unsigned short & Val ) const;
  void Get( const SimpleString Key, short & Val ) const;
  void Get( const SimpleString Key, unsigned char & Val ) const;
  void Get( const SimpleString Key, char & Val ) const;
  void Get( const SimpleString Key, double & Val ) const;
  void Get( const SimpleString Key, float & Val ) const;

  void Get( const SimpleString Key, std::vector<std::string> & Vec ) const;
  void Get( const SimpleString Key, std::vector<unsigned int> & Vec ) const;
  void Get( const SimpleString Key, std::vector<int> & Vec ) const;
  void Get( const SimpleString Key, std::vector<unsigned short> & Vec ) const;
  void Get( const SimpleString Key, std::vector<short> & Vec ) const;
  void Get( const SimpleString Key, std::vector<unsigned char> & Vec ) const;
  void Get( const SimpleString Key, std::vector<char> & Vec ) const;
  void Get( const SimpleString Key, std::vector<bool> & Vec ) const;
  void Get( const SimpleString Key, std::vector<double> & Vec ) const;
  void Get( const SimpleString Key, std::vector<float> & Vec ) const;

  /** \brief Get an object of any type
   * \param Key [in] the key that identifies the value
   * \param obj [out] the object to write hte value.
   */
  template <class C>
  void Get( const Omiscid::SimpleString Key, C& obj) const;

  /** \brief Get a vector of object of any type from the object
   * \param Key [in] the key that identifies the value
   * \param msg [out] the vector to write the value.
   */
  template <class C>
  void Get( const Omiscid::SimpleString Key, std::vector<C>& Vec) const;

  /** \brief Encode the structured message to a string
  *
  * \return Structured Object encoded into a simple string
  */
  SimpleString Encode() const throw( StructuredMessageException );

  /** \brief Get a reference of the internel object
  *
  * \return return a reference of the internal object
  */
  const json_spirit::Object& GetObject() const;

  /** \brief Function to encode a Json object in a structured message
   *
   * \param Obj [in] the object to encode
   * \return Json object serialized into a string
   */
  static SimpleString EncodeStructuredMessage( const json_spirit::Object& Obj);

  /** \brief Function to decode a structured message to a Json object
   *
   * \param Msg [in] the msg to decode
   * \param Obj [out] the Json object output
   */
  static void DecodeStructuredMessage( Omiscid::SimpleString Msg, json_spirit::Object& Obj ) throw( StructuredMessageException );

  /** \brief Function to decode a structured message to a Json object
   *
   * \param Msg [in] the msg to decode
   * \param Obj [out] the StructuredMessage object output
   */
  static void DecodeStructuredMessage( Omiscid::SimpleString Msg, StructuredMessage& StrMsg ) throw( StructuredMessageException );

  /** \brief Function to decode a Json object from a <code>std::istream</code>
  *
  * \param Stream [in] the Json object stream
  * \param Obj [out] the Json object output
  */
  static void DecodeStructuredMessage( std::istream& Stream, json_spirit::Object& Obj ) throw( StructuredMessageException );
  
  /** \brief Function to decode a Json object from a <code>std::istream</code>
  *
  * \param Stream [in] the Json object stream
  * \param StrMsg [out] the StructuredMessage output
  */
  static void DecodeStructuredMessage( std::istream& Stream, StructuredMessage& StrMsg ) throw( StructuredMessageException );

protected:
  json_spirit::Object o;
};


template<class T>
  inline void Save( StructuredMessage &Msg, const T& t);

template<class T>
  inline void Load( StructuredMessage &Msg, T& t);

/* template implementation */

template <class C>
void StructuredMessage::Put( const Omiscid::SimpleString Key, const C& obj)
{
  StructuredMessage msg;
  Save(msg, obj);
  Put( Key, msg );
}

template <class C>
  void StructuredMessage::Put( const Omiscid::SimpleString Key, const std::vector<C>& Vec)
{
  json_spirit::Array Val;
  
  for( class std::vector<C>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    StructuredMessage msg;
    Save(msg, *it);
    Val.push_back( json_spirit::Value(msg.GetObject()) );
  }  
  Put(Key, Val);
}

template <class C>
void StructuredMessage::Get( const Omiscid::SimpleString Key, C& obj) const
{
  StructuredMessage msg(Get(Key).get_obj());
  Load(msg, obj);
}

template <class C>
void StructuredMessage::Get( const Omiscid::SimpleString Key, std::vector<C>& Vec) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    StructuredMessage msg((*it).get_obj());
    C obj;
    Load(msg, obj);
    Vec.push_back( obj );
  }
}

template <typename T>
  void StructuredMessage::Set( const SimpleString Key, const T & Val )
{
  if( Has( Key ) ) {
    // If exist then change it
    Pop( Key );
    Put( Key, Val );
  } else {
    // Add a new element
    Put( Key, Val);
  }
}

template<class T>
inline void Save( StructuredMessage &Msg, const T& t){
    Access::Save(Msg, t);
}

template<class T>
inline void Load( StructuredMessage &Msg, T& t){
    Access::Load(Msg, t);
}


}} // Omiscid::Messaging

#endif //__STRUCTURED_MESSAGE_H__

