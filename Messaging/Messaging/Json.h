#ifndef __JSON_H__
#define __JSON_H__

#include <Json/json_spirit.h>

/**
 * Return true if the pair as the name "name", false otherwise
 */
bool same_name( const json_spirit::Pair& pair, const std::string& name );

#endif

