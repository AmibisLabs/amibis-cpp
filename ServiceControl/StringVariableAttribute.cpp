
#include <ServiceControl/StringVariableAttribute.h>

#include <ServiceControl/VariableAttribute.h>

using namespace Omiscid;

StringVariableAttribute::~StringVariableAttribute()
{
}

StringVariableAttribute::StringVariableAttribute(VariableAttribute* va, SimpleString value)
{
	VariableAtt = va;
	SetValue( value );
}

void StringVariableAttribute::SetValue( SimpleString value )
{
	// First set value to the reference VariableAttribute
	VariableAtt->SetValueFromControl( value );
	StringValue = value;
}

SimpleString StringVariableAttribute::GetValue() const
{
	return StringValue;
}
