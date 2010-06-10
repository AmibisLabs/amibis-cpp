#include <Messaging/Json.h>

bool same_name( const json_spirit::Pair& pair, const std::string& name )
{
  return pair.name_ == name;
}
