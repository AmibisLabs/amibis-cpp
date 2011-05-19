#include <algorithm>
// #include <boost/bind.hpp>

#include <System/SimpleString.h>
#include <Messaging/SerializeManager.h>
#include <Messaging/StructuredMessage.h>
#include <Messaging/StructuredParameters.h>
#include <Messaging/StructuredMethodCall.h>
#include <Messaging/StructuredMessageException.h>

using namespace std;
using namespace Omiscid;

StructuredMethodCall::StructuredMethodCall()
: StructuredMessage()
{
}

StructuredMethodCall::StructuredMethodCall( const StructuredMessage& Obj) throw( StructuredMessageException )
{
  BuildFromStructuredMessage(Obj);
}

StructuredMethodCall::StructuredMethodCall( const StructuredMethodCall& Obj)
{
  Init( Obj.GetMethodName(), Obj.GetParameters() );
}

StructuredMethodCall::StructuredMethodCall( const SimpleString& MethodName, const StructuredParameters& Params)
{
  Init( MethodName, Params );
}

StructuredMethodCall::~StructuredMethodCall()
{
}

void StructuredMethodCall::Init( const SimpleString& MethodName, const StructuredParameters& Params )
{
  this->MethodName = MethodName;
  this->Params = Params;
  Put( "method", MethodName.GetStr() );
  Put( "params", Params.GetArray() );
}

const StructuredMessage& StructuredMethodCall::operator=( const StructuredMethodCall& Msg )
{
  JsonParser = Msg.GetObject();
  fprintf(stderr, "toto\n");
  return *this;
}

const SimpleString& StructuredMethodCall::GetMethodName() const
{
  return this->MethodName;
}

const StructuredParameters& StructuredMethodCall::GetParameters() const
{
  return this->Params;
}

void StructuredMethodCall::SetMethodName( const SimpleString& MethodName )
{
  this->MethodName = MethodName;
  Set( "method", MethodName.GetStr() );
}

void StructuredMethodCall::SetParameters( const StructuredParameters& Params )
{
  this->Params = Params;
  Set( "params", Params.GetArray() );
}

bool StructuredMethodCall::IsMethodCall( const StructuredMessage& Msg)
{
  json_spirit::Value method = Msg.Get("method");
  json_spirit::Value params = Msg.Get("params");
  if( method.type() == json_spirit::str_type && params.type() == json_spirit::array_type ) {
	return true;
  } else {
	return false;
  }
}

void StructuredMethodCall::BuildFromStructuredMessage( const StructuredMessage& Msg ) throw( StructuredMessageException )
{
  if( IsMethodCall(Msg) ) {
	json_spirit::Value method = Msg.Get("method");
	json_spirit::Value params = Msg.Get("params");
	Init( method.get_str().c_str(), params.get_array() );
  } else {
	throw StructuredMessageException("Bad StructuredMethodCall format.", StructuredMessageException::InvalidFormat);
  }
}
