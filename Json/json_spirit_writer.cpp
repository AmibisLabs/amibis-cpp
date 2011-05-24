/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

#include <Json/json_spirit_writer.h>
#include <Json/json_spirit_value.h>

#include <cassert>
#include <sstream>

using namespace json_spirit;
using namespace std;

namespace
{

	// does the actual formatting,
	// it keeps track of the indentation level etc.
	//
	class Generator
	{
	public:

		Generator( const Value& value, ostream& os, bool pretty )
		:   os_( os )
		,   indentation_level_( 0 )
		,   pretty_( pretty )
		{
			output( value );
		}

	private:

		void output( const Value& value )
		{
			switch( value.type() )
			{
				case obj_type:   output( value.get_obj() );   break;
				case array_type: output( value.get_array() ); break;
				case str_type:   output( value.get_str() );   break;
				case bool_type:  output( value.get_bool() );  break;
				case int_type:   os_ <<  value.get_int();     break;
				case real_type:  os_ <<  value.get_real();    break;
				case null_type:  os_ << "null";               break;
				default: assert( false );
			}
		}

		void output( const Object& obj )
		{
			output_array_or_obj( obj, '{', '}' );
		}

		void output( const Array& arr )
		{
			output_array_or_obj( arr, '[', ']' );
		}

		void output( const Pair& pair )
		{
			output( pair.name_ ); space(); os_ << ':'; space(); output( pair.value_ );
		}

		void output( const string& s )
		{
			os_ << '"' << s << '"';
		}

		void output( bool b )
		{
			const string s = b ? "true" : "false";
			os_ << s;
		}

		template< class T >
		void output_array_or_obj( const T& t, char start_char, char end_char )
		{
			os_ << start_char; new_line();

			++indentation_level_;

			for( typename T::const_iterator i = t.begin(); i != t.end(); ++i )
			{
				indent(); output( *i );

				if( i != t.end() - 1 )
				{
					os_ << ',';
				}

				new_line();
			}

			--indentation_level_;

			indent(); os_ << end_char;
		}

		void indent()
		{
			if( !pretty_ ) return;

			for( int i = 0; i < indentation_level_; ++i )
			{
				os_ << "    ";
			}
		}

		void space()
		{
			if( pretty_ ) os_ << ' ';
		}

		void new_line()
		{
			if( pretty_ ) os_ << '\n';
		}

		ostream& os_;
		int indentation_level_;
		bool pretty_;
	};
}

void json_spirit::write( const Value& value, std::ostream& os )
{
	Generator( value, os, false );
}

void json_spirit::write_formatted( const Value& value, std::ostream& os )
{
	Generator( value, os, true );
}

std::string json_spirit::write( const Value& value )
{
	ostringstream os;

	write( value, os );

	return os.str();
}

std::string json_spirit::write_formatted( const Value& value )
{
	ostringstream os;

	write_formatted( value, os );

	return os.str();
}

