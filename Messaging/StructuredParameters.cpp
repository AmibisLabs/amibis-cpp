#include <Messaging/StructuredParameters.h>

using namespace std;
using namespace Omiscid;
using namespace Messaging;

#define BETWEEN(x,a,b) ((a) <= (x)) && ((x) < (b))

StructuredParameters::StructuredParameters()
{
}

StructuredParameters::StructuredParameters(const json_spirit::Array& Obj)
: Params(Obj)
{
}

StructuredParameters::~StructuredParameters()
{
}

const json_spirit::Array& StructuredParameters::GetArray() const
{
  return this->Params;
}

void StructuredParameters::AddParameter( json_spirit::Value Val )
{
  this->Params.push_back(Val);
}

int StructuredParameters::GetNumberOfParameters() const
{
  return (int) this->Params.size();
}

void StructuredParameters::GetParameter( int Index, int& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
    if( this->Params[Index].type() == json_spirit::int_type ) {
      Val = Params[Index].get_int();
    } else {
      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
    }
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  }
}

void StructuredParameters::GetParameter( int Index, SimpleString& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
    if( this->Params[Index].type() == json_spirit::str_type ) {
      Val = Params[Index].get_str().c_str();
    } else {
      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
    }
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  }
}
  
void StructuredParameters::GetParameter( int Index, bool& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
    if( this->Params[Index].type() == json_spirit::bool_type ) {
      Val = Params[Index].get_bool();
    } else {
      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
    }
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  }
}

void StructuredParameters::GetParameter( int Index, double& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
	  json_spirit::Value_type param_type = this->Params[Index].type();
    if( param_type == json_spirit::real_type ) {
      Val = Params[Index].get_real();
    } else if(param_type == json_spirit::int_type)
    {
    	Val = Params[Index].get_int();
    }else{
      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
    }
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  }
}

void StructuredParameters::GetParameter( int Index, float& Val ) const throw( StructuredMessageException )
{
	double val_d;
	GetParameter(Index, val_d);
	Val = static_cast<float>(val_d);
}


void StructuredParameters::GetParameter( int Index, json_spirit::Value& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
    Val = Params[Index];
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  } 
}

void StructuredParameters::GetParameter( int Index, json_spirit::Object& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
    if( this->Params[Index].type() == json_spirit::obj_type ) {
      Val = Params[Index].get_obj();
    } else {
      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
    }
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  }
}

void StructuredParameters::GetParameter( int Index, json_spirit::Array& Val ) const throw( StructuredMessageException )
{
  if( BETWEEN(Index, 0,GetNumberOfParameters()) ) {
    if( this->Params[Index].type() == json_spirit::array_type ) {
      Val = Params[Index].get_array();
    } else {
      throw StructuredMessageException( "Bad argument's type.", StructuredMessageException::IllegalTypeConversion );
    }
  } else {
    throw StructuredMessageException( "Index out of bound.", StructuredMessageException::Exception );
  }
}

SimpleString StructuredParameters::ToString()
{
  return json_spirit::write_formatted(Params).c_str();
}

