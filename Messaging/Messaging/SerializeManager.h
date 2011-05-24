#ifndef __SERIALIZE_MANAGER_H__
#define __SERIALIZE_MANAGER_H__

#include <Messaging/ConfigMessaging.h>

#include <System/SimpleString.h>
#include <Json/json_spirit.h>

namespace Omiscid {

// Defining type of JSON data
typedef json_spirit::Pair SerializePair;
typedef json_spirit::Object SerializeObject;
typedef json_spirit::Object::iterator SerializeObjectIterator;
typedef json_spirit::Object::const_iterator SerializeObjectConstIterator;
typedef json_spirit::Array SerializeArray;
typedef json_spirit::Array::iterator SerializeArrayIterator;
typedef json_spirit::Array::const_iterator SerializeArrayConstIterator;
// typedef json_spirit::Value SerializeValue;

/**
 * Return true if the pair as the name "name", false otherwise
 */
inline bool same_name( const json_spirit::Pair& pair, const std::string& name )
{
	return pair.name_ == name;
}

inline bool same_name( const json_spirit::Pair& pair, const Omiscid::SimpleString& name )
{
	return pair.name_ == name.GetStr();
}


} // Omiscid

#endif // __SERIALIZE_MANAGER_H__

