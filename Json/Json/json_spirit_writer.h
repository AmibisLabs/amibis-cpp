#ifndef JASON_SPIRIT_WRITER
#define JASON_SPIRIT_WRITER

/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

#pragma once

#include <iostream>

namespace json_spirit
{
    class Value;

    // converts a JSON object or array to text, 
    // the "formatted" versions add whitespace to format the output nicely
    //
    void        write          ( const Value& value, std::ostream& os );
    void        write_formatted( const Value& value, std::ostream& os );
    std::string write          ( const Value& value );
    std::string write_formatted( const Value& value );
}

#endif
