#ifndef JASON_SPIRIT_READER
#define JASON_SPIRIT_READER

/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

#pragma once

#include <string>
#include <iostream>

namespace json_spirit
{
    class Value;

    // converts text to a JSON value that will be either a JSON object or array
    //
    bool read( const std::string& s, Value& value );
    bool read( std::istream& is,     Value& value );
}

#endif
