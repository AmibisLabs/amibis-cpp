/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

#include <Json/json_spirit_value.h>

#include <cassert>

using namespace json_spirit;

const Value Value::null;

Value::Value()
:   type_( null_type )
{
}

Value::Value( const Value &val)
  : type_(val.type_)
  , str_(val.str_)
  , obj_(val.obj_)
  , array_(val.array_)
  , bool_(val.bool_)
  , i_(val.i_)
  , d_(val.d_)
{
}

Value::Value( const char* value )
:   type_( str_type )
,   str_( value )
{
}

Value::Value( const std::string& value )
:   type_( str_type )
,   str_( value )
{
}

Value::Value( const Object& value )
:   type_( obj_type )
,   obj_( value )
{
}

Value::Value( const Array& value )
:   type_( array_type )
,   array_( value )
{
}

Value::Value( bool value )
:   type_( bool_type )
,   bool_( value )
{
}

Value::Value( int value )
:   type_( int_type )
,   i_( value )
{
}

Value::Value( double value )
:   type_( real_type )
,   d_( value )
{
}

bool Value::operator==( const Value& lhs ) const
{
	if( this == &lhs ) return true;

	if( type() != lhs.type() ) return false;

	switch( type_ )
	{
		case str_type:   return get_str()   == lhs.get_str();
		case obj_type:   return get_obj()   == lhs.get_obj();
		case array_type: return get_array() == lhs.get_array();
		case bool_type:  return get_bool()  == lhs.get_bool();
		case int_type:   return get_int()   == lhs.get_int();
		case real_type:  return get_real()  == lhs.get_real();
		case null_type:  return true;
	};

	assert( false );

	return false;
}

Value_type Value::type() const
{
	return type_;
}

const std::string& Value::get_str() const
{
	assert( type() == str_type );

	return str_;
}

const Object& Value::get_obj() const
{
	assert( type() == obj_type );

	return obj_;
}

const Array& Value::get_array() const
{
	assert( type() == array_type );

	return array_;
}

bool Value::get_bool() const
{
	assert( type() == bool_type );

	return bool_;
}

int Value::get_int() const
{
	assert( type() == int_type );

	return i_;
}

double Value::get_real() const
{
	assert( type() == real_type || type() == int_type);

	if(type() == real_type)
		return d_;
	else
		return i_;
}

Object& Value::get_obj()
{
	assert( type() == obj_type );

	return obj_;
}

Array& Value::get_array()
{
	assert( type() == array_type );

	return array_;
}

Pair::Pair( const std::string& name, const Value& value )
:   name_( name )
,   value_( value )
{
}

bool Pair::operator==( const Pair& lhs ) const
{
	if( this == &lhs ) return true;

	return ( name_ == lhs.name_ ) && ( value_ == lhs.value_ );
}

