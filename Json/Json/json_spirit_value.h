#ifndef JASON_SPIRIT_VALUE
#define JASON_SPIRIT_VALUE

/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

#pragma once

#include <vector>
#include <string>

namespace json_spirit
{
	enum Value_type{ obj_type, array_type, str_type, bool_type, int_type, real_type, null_type };

	class Value;
	struct Pair;

	typedef std::vector< Pair > Object;

	typedef std::vector< Value > Array;

	class Value
	{
	public:

		Value();  // creates null value
		Value( const char*        value );
		Value( const std::string& value );
		Value( const Object&      value );
		Value( const Array&       value );
		Value( bool               value );
		Value( int                value );
		Value( double             value );

		Value( const Value &val); // copy constructor

		bool operator==( const Value& lhs ) const;

		Value_type type() const;

		const std::string& get_str()   const;
		const Object&      get_obj()   const;
		const Array&       get_array() const;
		bool               get_bool()  const;
		int                get_int()   const;
		double             get_real()  const;

		inline void get_val(std::string& s) const {s = get_str();}
		inline void get_val(Object& o) const {o = get_obj();}
		inline void get_val(Array& a) const {a = get_array();}
		inline void get_val(bool& b) const {b = get_bool();}
		inline void get_val(int& i) const {i = get_int();}
		inline void get_val(double& d) const {d = get_real();}
		inline void get_val(float& f) const {f = (float)get_real();}

		Object& get_obj();
		Array&  get_array();

		static const Value null;

	private:

		Value_type type_;

		std::string str_;
		Object obj_;
		Array array_;
		bool bool_;
		int i_;
		double d_;
	};

	struct Pair
	{
		Pair( const std::string& name, const Value& value );

		bool operator==( const Pair& lhs ) const;

		std::string name_;
		Value value_;
	};

}

#endif

