/* Copyright (c) 2007 John W Wilkinson

   This source code can be used for any purpose as long as
   this comment is retained. */

#include <Json/json_spirit_reader.h>
#include <Json/json_spirit_value.h>
#include <cassert>
#include <iostream>
#include <iterator>


#ifdef USE_BOOST_SPIRIT

#include <boost/bind.hpp>
#include <boost/function.hpp>
#include <boost/spirit/include/classic_core.hpp>
#include <boost/spirit/include/classic_confix.hpp>
#include <boost/spirit/include/classic_escape_char.hpp>
#include <boost/spirit/include/classic_lists.hpp>
using namespace boost;
using namespace boost::spirit;
using namespace boost::spirit::classic;
#endif //USE_BOOST_SPIRIT

using namespace json_spirit;
using namespace std;

namespace
{
	string remove_trailing_quote( const string& s )
	{
		assert( s[ s.length() - 1 ] == '"' );

		return s.substr( 0, s.length() - 1 );
	}

	// this class's methods get called by the spirit parse resulting
	// in the creation of a JSON object or array
	//
	class Semantic_actions
	{
	public:

		Semantic_actions( Value& value );

		void begin_obj   ( char c );
		void end_obj     ( char c );
		void begin_array ( char c );
		void end_array   ( char c );
		void new_c_esc_ch( char c );
		void new_name ( const char* str, const char* end );
		void new_str  ( const char* str, const char* end );
		void new_true ( const char* str, const char* end );
		void new_false( const char* str, const char* end );
		void new_null ( const char* str, const char* end );
		void new_int ( int i );
		void new_real( double d );
		void set_current_str( const string& str);

	private:

		template< class Array_or_obj > void begin_compound();
		void end_compound();
		void add_to_current( const Value& value );
		string get_current_str();

		Value& value_;              // this is the object ro array that is being created
		Value* current_p;           // the child object or array that is currently being constructed

		vector< Value* > stack_;    // previous child objects and arrays

		string name_;               // of current name/value pair
		string current_str_;        // current name or string value
	};

	Semantic_actions::Semantic_actions( Value& value )
	:   value_( value )
	,   current_p( 0 )
	{
	}

	void Semantic_actions::set_current_str( const string& str)
	{
		current_str_ = str;
	}

	void Semantic_actions::begin_obj( char c )
	{
		assert( c == '{' );

		begin_compound< Object >();
	}

	void Semantic_actions::end_obj( char c )
	{
		assert( c == '}' );

		end_compound();
	}

	void Semantic_actions::begin_array( char c )
	{
		assert( c == '[' );

		begin_compound< Array >();
   }

	void Semantic_actions::end_array( char c )
	{
		assert( c == ']' );

		end_compound();
	}

	void Semantic_actions::new_c_esc_ch( char c )  // work around for c_escape_ch_p parser bug where
												   // esc chars are not converted unless the semantic action
												   // is attached directlry to the parser
	{
		current_str_ += c;
	}

	void Semantic_actions::new_name( const char* str, const char* end )
	{
		assert( current_p->type() == obj_type );

		name_ = get_current_str();
	}

	void Semantic_actions::new_str( const char* str, const char* end )
	{
		add_to_current( get_current_str() );
	}

	void Semantic_actions::new_true( const char* str, const char* end )
	{
		assert( string( str, end ) == "true" );

		add_to_current( true );
	}

	void Semantic_actions::new_false( const char* str, const char* end )
	{
		assert( string( str, end ) == "false" );

		add_to_current( false );
	}

	void Semantic_actions::new_null( const char* str, const char* end )
	{
		assert( string( str, end ) == "null" );

		add_to_current( Value() );
	}

	void Semantic_actions::new_int( int i )
	{
		add_to_current( i );
	}

	void Semantic_actions::new_real( double d )
	{
		add_to_current( d );
	}

	void Semantic_actions::add_to_current( const Value& value )
	{
		if( current_p->type() == array_type )
		{
			current_p->get_array().push_back( value );
		}
		else
		{
			current_p->get_obj().push_back( Pair( name_, value ) );
		}
	}

	template< class Array_or_obj >
	void Semantic_actions::begin_compound()
	{
		if( current_p == 0 )
		{
			value_ = Array_or_obj();

			current_p = &value_;
		}
		else
		{
			stack_.push_back( current_p );

			Array_or_obj new_array_or_obj;   // avoid copy by building new array or object in place

			add_to_current( new_array_or_obj );

			if( current_p->type() == array_type )
			{
				current_p = &current_p->get_array().back();
			}
			else
			{
				current_p = &current_p->get_obj().back().value_;
			}
		}
	}

	void Semantic_actions::end_compound()
	{
		if( current_p != &value_ )
		{
			current_p = stack_.back();

			stack_.pop_back();
		}
	}

	string Semantic_actions::get_current_str()
	{
#ifdef USE_BOOST_SPIRIT
		string result = remove_trailing_quote( current_str_ );
#else
		string result = current_str_;
#endif
		current_str_ = "";
		return result;
	}

#ifdef USE_BOOST_SPIRIT

	// the spirit grammer
	//
	class Json_grammer : public grammar< Json_grammer >
	{
	public:

		Json_grammer( Semantic_actions& semantic_actions )
		:   actions_( semantic_actions )
		{
		}

		template< typename ScannerT >
		struct definition
		{
			definition( const Json_grammer& self )
			{
				// need to convert the semantic action class methods to functors with the
				// paramater signature expected by spirit

				typedef function< void( char )                     > Char_action;
				typedef function< void( const char*, const char* ) > Str_action;
				typedef function< void( double )                   > Real_action;
				typedef function< void( int )                      > Int_action;

				Char_action begin_obj     ( bind( &Semantic_actions::begin_obj,    &self.actions_, _1 ) );
				Char_action end_obj       ( bind( &Semantic_actions::end_obj,      &self.actions_, _1 ) );
				Char_action begin_array   ( bind( &Semantic_actions::begin_array,  &self.actions_, _1 ) );
				Char_action end_array     ( bind( &Semantic_actions::end_array,    &self.actions_, _1 ) );
				Char_action new_c_esc_ch  ( bind( &Semantic_actions::new_c_esc_ch, &self.actions_, _1 ) );
				Str_action  new_name      ( bind( &Semantic_actions::new_name,     &self.actions_, _1, _2 ) );
				Str_action  new_str       ( bind( &Semantic_actions::new_str,      &self.actions_, _1, _2 ) );
				Str_action  new_true      ( bind( &Semantic_actions::new_true,     &self.actions_, _1, _2 ) );
				Str_action  new_false     ( bind( &Semantic_actions::new_false,    &self.actions_, _1, _2 ) );
				Str_action  new_null      ( bind( &Semantic_actions::new_null,     &self.actions_, _1, _2 ) );
				Real_action new_real      ( bind( &Semantic_actions::new_real,     &self.actions_, _1 ) );
				Int_action  new_int       ( bind( &Semantic_actions::new_int,      &self.actions_, _1 ) );

				json_
					= ( object_ | array_ ) >> end_p
					;

				object_
					= confix_p
					  (
						  ch_p('{')[ begin_obj ],
						  *members_,
						  ch_p('}')[ end_obj ]
					  )
					;

				members_
					= pair_ >> *( ',' >> pair_ )
					;

				pair_
					= string_[ new_name ]
					>> ':'
					>> value_
					;

				value_
					= string_[ new_str ]
					| number_
					| object_
					| array_
					| str_p( "true" ) [ new_true  ]
					| str_p( "false" )[ new_false ]
					| str_p( "null" ) [ new_null  ]
					;

				array_
					= confix_p
					  (
						  ch_p('[')[ begin_array ],
						  *list_p( elements_, ',' ),
						  ch_p(']')[ end_array ]
					  )
					;

				elements_
					= value_ >> *( ',' >> value_ )
					;

				string_
					= lexeme_d // this causes white space inside a string to be retained
					  [
						  confix_p
						  (
							  '"',
							  *c_escape_ch_p[ new_c_esc_ch ],  // work around for c_escape_ch_p parser bug
							  '"'
						  )
					  ]
					;

				number_
					= strict_real_p[ new_real ]
					| int_p        [ new_int  ]
					;
			}

			rule< ScannerT > json_, object_, members_, pair_, array_, elements_, value_, string_, number_;

			const rule< ScannerT >& start() const { return json_; }
		};

		Semantic_actions& actions_;
	};

#endif // USE_BOOST_SPIRIT

	void stream_to_string( std::istream& is, string& s )
	{
		is.unsetf( ios::skipws );

		copy( istream_iterator< char>( is ), istream_iterator< char >(), back_inserter( s ) );
	}
}

#ifdef USE_BOOST_SPIRIT
bool json_spirit::read( const std::string& s, Value& value )
{
	Semantic_actions semantic_actions( value );

	parse_info<> info = parse( s.c_str(), Json_grammer( semantic_actions ), space_p );

	return info.full;
}

bool json_spirit::read( std::istream& is, Value& value )
{
	std::string s;

	stream_to_string( is, s );

	return read( s, value );
}

#else // USE_BOOST_SPIRIT

#include <Json/JSON_parser.h>
#include <sstream>
using namespace std;
using namespace json_spirit;

static int json_calback(void* ctx, int type, const JSON_value* value);

bool json_spirit::read( std::istream& is, Value& value )
{
  bool result = true;

  Semantic_actions semantic_actions( value );

  JSON_config config;
  struct JSON_parser_struct* jc = NULL;
  init_JSON_config(&config);
  config.callback = &json_calback;
  config.callback_ctx = static_cast<void*>(&semantic_actions);
  jc = new_JSON_parser(&config);

  while(is.good()) {
	int next_char = is.get();
	if (next_char <= 0) {
	  break;
	}
	if (!JSON_parser_char(jc, next_char)) {
	  result = false;
	  goto done;
	}
  }

  if (!JSON_parser_done(jc)) {
	result = false;
	goto done;
  }

  done:
	delete_JSON_parser(jc);
	return result;
}

bool json_spirit::read( const std::string& s, Value& value )
{
  istringstream ss(s);
  return read(ss, value);
}

int json_calback(void* ctx, int type, const JSON_value* value)
{
  Semantic_actions * semantic_actions = static_cast<Semantic_actions *>(ctx);

  switch(type) {
  case JSON_T_ARRAY_BEGIN:
	semantic_actions->begin_array('[');
	break;
  case JSON_T_ARRAY_END:
	semantic_actions->end_array(']');
	break;
  case JSON_T_OBJECT_BEGIN:
	semantic_actions->begin_obj('{');
	break;
  case JSON_T_OBJECT_END:
	semantic_actions->end_obj('}');
	break;
  case JSON_T_INTEGER:
	semantic_actions->new_int(value->vu.integer_value);
	break;
  case JSON_T_FLOAT:
	semantic_actions->new_real(value->vu.float_value);
	break;
  case JSON_T_NULL:
  {
	char snull[] = "null";
	semantic_actions->new_null(snull, snull+4);
  }
  break;
  case JSON_T_TRUE:
  {
	char strue[] = "true";
	semantic_actions->new_true(strue, strue+4);
  }
  break;
  case JSON_T_FALSE:
  {
	char sfalse[] = "false";
	semantic_actions->new_false(sfalse, sfalse+5);
  }
  break;
  case JSON_T_KEY:
  {
	string str(value->vu.str.value, value->vu.str.length);
	semantic_actions->set_current_str(str);
	semantic_actions->new_name(value->vu.str.value, value->vu.str.value+value->vu.str.length);
	break;
  }
  case JSON_T_STRING:
  {
	string str(value->vu.str.value, value->vu.str.length);
	semantic_actions->set_current_str(str);
	semantic_actions->new_str(value->vu.str.value, value->vu.str.value+value->vu.str.length);
	break;
  }
  default:
	break;
  }
  return 1;
}


#endif // USE_BOOST_SPIRIT

