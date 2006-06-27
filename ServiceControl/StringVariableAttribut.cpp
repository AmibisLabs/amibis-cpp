
#include <ServiceControl/StringVariableAttribut.h>

#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

StringVariableAttribut::StringVariableAttribut(VariableAttribut* va, SimpleString value)
{
  VariableAtt = va;
  SetValue(value);
}

void StringVariableAttribut::SetValue( SimpleString value )
{
	StringValue = value;
    VariableAtt->SetValueStr( StringValue ); 
}

SimpleString StringVariableAttribut::GetValue() const
{
	return StringValue;
}
