
#include <ServiceControl/StringVariableAttribut.h>

#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

StringVariableAttribut::~StringVariableAttribut()
{
}

StringVariableAttribut::StringVariableAttribut(VariableAttribut* va, SimpleString value)
{
  VariableAtt = va;
  SetValue(value);
}

void StringVariableAttribut::SetValue( SimpleString value )
{
	StringValue = value;
    VariableAtt->SetValueFromControl( StringValue ); 
}

SimpleString StringVariableAttribut::GetValue() const
{
	return StringValue;
}
