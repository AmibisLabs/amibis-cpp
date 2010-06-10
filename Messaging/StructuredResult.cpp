#include <Messaging/StructuredResult.h>

using namespace std;
using namespace Omiscid;
using namespace Messaging;

StructuredResult::StructuredResult()
{
  Set( "result", json_spirit::Value() );
  Set( "error", json_spirit::Value() );
}

StructuredResult::StructuredResult( const StructuredMessage& Msg) throw( Omiscid::Messaging::StructuredMessageException )
{
  BuildFromStructuredMessage(Msg);
}

StructuredResult::~StructuredResult()
{
}

// void StructuredResult::SetResult( const SimpleString& Val )
// {
//   this->Result = json_spirit::Value(Val.GetStr());
//   Set( "result", this->Result );
// }

// void StructuredResult::SetResult( const json_spirit::Value& Val )
// {
//   this->Result = Val;
//   Set( "result", this->Result );
// }
  
// void StructuredResult::SetResult( const json_spirit::Object& Val )
// {
//   this->Result = Val;
//   Set( "result", this->Result );
// }
  
// void StructuredResult::SetResult( const json_spirit::Array& Val )
// {
//   this->Result = Val;
//   Set( "result", this->Result );
// }

// void StructuredResult::SetResult( bool Val )
// {
//   this->Result = json_spirit::Value(Val);
//   Set( "result", this->Result );
// }
  
// void StructuredResult::SetResult( int Val )
// {
//   this->Result = json_spirit::Value(Val);
//   Set( "result", this->Result );
// }
  
// void StructuredResult::SetResult( double Val )
// {
//   this->Result = json_spirit::Value(Val);
//   Set( "result", this->Result );
// }

// void StructuredResult::SetResult( float Val )
// {
//   this->Result = json_spirit::Value(Val);
//   Set( "result", this->Result );
// }

void StructuredResult::SetError( const SimpleString& Error )
{
  this->Error = Error;
  Set( "error", this->Error.GetStr() );
}

// void StructuredResult::GetResult( SimpleString& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::str_type ) {
//     Val = this->Result.get_str().c_str();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }

// void StructuredResult::GetResult( json_spirit::Value& Val ) const
// {
//   Val = this->Result;
// }

// void StructuredResult::GetResult( json_spirit::Object& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::obj_type ) {
//     Val = this->Result.get_obj();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }

// void StructuredResult::GetResult( json_spirit::Array& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::array_type ) {
//     Val = this->Result.get_array();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }

// void StructuredResult::GetResult( bool& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::bool_type ) {
//     Val = this->Result.get_bool();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }

// void StructuredResult::GetResult( int& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::int_type ) {
//     Val = this->Result.get_int();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }
  
// void StructuredResult::GetResult( double& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::real_type ) {
//     Val = this->Result.get_real();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }

// void StructuredResult::GetResult( float& Val ) const  throw( StructuredMessageException )
// {
//   if( this->Result.type() == json_spirit::real_type ) {
//     Val = this->Result.get_real();
//   } else {
//     throw StructuredMessageException("Bad Type Request.", StructuredMessageException::IllegalTypeConversion);
//   }
// }

const SimpleString& StructuredResult::GetError() const
{
  return this->Error;
}

bool StructuredResult::HasError() const
{
  return Has("error");
}

bool StructuredResult::IsResult( const StructuredMessage& Msg)
{
  return Msg.Has("result") && Msg.Has("error");
  
//   json_spirit::Value error = Msg.Get("error");
//   json_spirit::Value result = Msg.Get("result");
//   if( error.type() == json_spirit::str_type || result.type() != json_spirit::null_type ) {
//     return true;
//   } else {
//     return false;
//   }
}

void StructuredResult::BuildFromStructuredMessage( const StructuredMessage& Msg ) throw( Omiscid::Messaging::StructuredMessageException )
{
  if( IsResult(Msg) ) {
    json_spirit::Value error = Msg.Get("error");
    json_spirit::Value result = Msg.Get("result");
    if( error.type() != json_spirit::null_type ) SetError(error.get_str().c_str());
    if( result.type() != json_spirit::null_type ) SetResult( result );
  } else {
    throw StructuredMessageException("Bad StructuredMethodCall format.", StructuredMessageException::InvalidFormat);
  }
}
