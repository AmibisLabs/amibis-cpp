#include <algorithm>
#include <boost/bind.hpp>

#include <Messaging/StructuredMessage.h>

using namespace std;
using namespace Omiscid;
using namespace Messaging;
using namespace boost;

StructuredMessage::StructuredMessage() 
{
}

StructuredMessage::StructuredMessage( const StructuredMessage& Msg)
:o(Msg.GetObject())
{
}

StructuredMessage::StructuredMessage( const json_spirit::Object& Obj)
: o(Obj) 
{
}

StructuredMessage::StructuredMessage( const SimpleString Str) throw( StructuredMessageException )
{
  StructuredMessage::DecodeStructuredMessage( Str, this->o );
}


StructuredMessage::~StructuredMessage() 
{
}

const StructuredMessage& StructuredMessage::operator=( const StructuredMessage& Msg)
{
  this->o = Msg.GetObject();
  return *this;
}

const json_spirit::Object& StructuredMessage::GetObject() const
{
  return (this->o);
}

void StructuredMessage::Put( const SimpleString Key, const json_spirit::Value Val )
{
  o.push_back( json_spirit::Pair(Key.GetStr(), Val) );
}

void StructuredMessage::Put( const SimpleString Key, StructuredMessage& msg )
{
  Put(Key, json_spirit::Value(msg.o));
}

void StructuredMessage::Put( const SimpleString Key, const char* Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, const std::string& Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, const json_spirit::Array& Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, const json_spirit::Object& Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, bool Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, unsigned int Val )
{
  Put(Key, json_spirit::Value(static_cast<int>(Val)));
}

void StructuredMessage::Put( const SimpleString Key, int Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, unsigned short Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, short Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, unsigned char Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, char Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, double Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, float Val )
{
  Put(Key, json_spirit::Value(Val));
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<std::string> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<std::string>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<unsigned int> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<unsigned int>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(static_cast<int>(*it)) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<int> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<int>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<unsigned short> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<unsigned short>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(static_cast<int>(*it)) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<short> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<short>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<unsigned char> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<unsigned char>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(static_cast<int>(*it)) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<char> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<char>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<float> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<float>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::vector<double> & Vec )
{
  json_spirit::Array Val;
  
  for( std::vector<double>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

/** std::list serialization */

void StructuredMessage::Put( const SimpleString Key, const std::list<std::string> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<std::string>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<unsigned int> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<unsigned int>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(static_cast<int>(*it)) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<int> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<int>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<unsigned short> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<unsigned short>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(static_cast<int>(*it)) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<short> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<short>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<unsigned char> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<unsigned char>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(static_cast<int>(*it)) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<char> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<char>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<float> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<float>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}

void StructuredMessage::Put( const SimpleString Key, const std::list<double> & Vec )
{
  json_spirit::Array Val;
  
  for( std::list<double>::const_iterator it = Vec.begin();
       it != Vec.end();
       ++it )
  {
    Val.push_back( json_spirit::Value(*it) );
  }
  
  Put(Key, Val);
}


bool StructuredMessage::Pop( const SimpleString Key )
{
  json_spirit::Object::iterator it;

  it = find_if( o.begin(), o.end(), bind( same_name, _1, Key.GetStr()) );
  if( it != o.end() )
  {
    o.erase(it);
    return true;
  } else {
    return false;
  }
}

void StructuredMessage::Set( const SimpleString Key, json_spirit::Value Val )
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

bool StructuredMessage::Has( const SimpleString Key ) const
{
  json_spirit::Object::const_iterator it;

  it = std::find_if( o.begin(), o.end(), bind( same_name, _1, Key.GetStr()) );
  if( it != o.end() ) {
    return true;
  } else {
    return false;
  }
}

bool StructuredMessage::Has( const SimpleString Key, json_spirit::Value Val ) const
{
  json_spirit::Object::const_iterator it;
  
  it = find(o.begin(), o.end(), json_spirit::Pair(Key.GetStr(), Val));
  if( it != o.end() )
  {
    return true;
  } else {
    return false;
  }
}

json_spirit::Value StructuredMessage::Get( const SimpleString Key ) const
{
  json_spirit::Object::const_iterator it;

  it = find_if( o.begin(), o.end(), bind( same_name, _1, Key.GetStr()) );
  if( it != o.end() )
  {
    return it->value_;
  } else {
    return json_spirit::Value();
  }
}

void StructuredMessage::Get( const SimpleString Key, std::string & Val ) const
{
  Val = Get(Key).get_str();
}

void StructuredMessage::Get( const SimpleString Key, bool & Val ) const
{
  Val = Get(Key).get_bool();
}

void StructuredMessage::Get( const SimpleString Key, unsigned int & Val ) const
{
  Val = static_cast<unsigned int>(Get(Key).get_int());
}

void StructuredMessage::Get( const SimpleString Key, int & Val ) const
{
  Val = Get(Key).get_int();
}

void StructuredMessage::Get( const SimpleString Key, unsigned short & Val ) const
{
  Val = static_cast<unsigned short>(Get(Key).get_int());
}

void StructuredMessage::Get( const SimpleString Key, short & Val ) const
{
  Val = static_cast<short>(Get(Key).get_int());
}

void StructuredMessage::Get( const SimpleString Key, unsigned char & Val ) const
{
  Val = static_cast<unsigned char>(Get(Key).get_int());
}

void StructuredMessage::Get( const SimpleString Key, char & Val ) const
{
  Val = static_cast<char>(Get(Key).get_int());
}

void StructuredMessage::Get( const SimpleString Key, double & Val ) const
{
  Val = Get(Key).get_real();
}

void StructuredMessage::Get( const SimpleString Key, float & Val ) const
{
  Val = static_cast<float>(Get(Key).get_real());
}

/* std::vector deserialization */
void StructuredMessage::Get( const SimpleString Key, std::vector<std::string> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_str() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<unsigned int> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<int> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<unsigned short> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<short> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<unsigned char> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<char> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<bool> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_bool() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<double> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_real() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::vector<float> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_real() );
  }
}

/* std::list deserialization */
void StructuredMessage::Get( const SimpleString Key, std::list<std::string> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_str() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<unsigned int> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<int> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<unsigned short> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<short> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<unsigned char> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<char> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_int() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<bool> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_bool() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<double> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_real() );
  }
}

void StructuredMessage::Get( const SimpleString Key, std::list<float> & Vec ) const
{
  Vec.clear();
  json_spirit::Value Val = Get(Key);
  const json_spirit::Array & arr = Val.get_array();
  for(json_spirit::Array::const_iterator it = arr.begin();
      it != arr.end();
      ++it)
  {
    Vec.push_back( (*it).get_real() );
  }
}

void StructuredMessage::Clear()
{
  this->o.clear();
}

Omiscid::SimpleString StructuredMessage::Encode() const throw( StructuredMessageException )
{
  return StructuredMessage::EncodeStructuredMessage( GetObject() );
}

SimpleString StructuredMessage::EncodeStructuredMessage( const json_spirit::Object& Obj) 
{
  return json_spirit::write_formatted(Obj).c_str();
}

void StructuredMessage::DecodeStructuredMessage( SimpleString Msg, json_spirit::Object& Obj ) throw( StructuredMessageException ) 
{
  json_spirit::Value v;
  string s(Msg.GetStr());
  if( !json_spirit::read(s, v) && (v.type() != json_spirit::obj_type) ) 
  {
    throw StructuredMessageException("Argument is not a valid Json stream", StructuredMessageException::MalformedJSONStream);
  } else {
    Obj = v.get_obj();
  }
}

void StructuredMessage::DecodeStructuredMessage( Omiscid::SimpleString Msg, StructuredMessage& val ) throw( StructuredMessageException )
{
  StructuredMessage::DecodeStructuredMessage( Msg, val.o );
}

void StructuredMessage::DecodeStructuredMessage( std::istream& Stream, json_spirit::Object& Obj ) throw( StructuredMessageException )
{
  json_spirit::Value v;
  if( !json_spirit::read(Stream, v) && (v.type() != json_spirit::obj_type) ) 
  {
    throw StructuredMessageException("Argument is not a valid Json stream", StructuredMessageException::MalformedJSONStream);
  } else {
    Obj = v.get_obj();
  }
}


void StructuredMessage::DecodeStructuredMessage( std::istream& Stream, StructuredMessage& val ) throw( StructuredMessageException )
{
  StructuredMessage::DecodeStructuredMessage( Stream, val.o );
}

