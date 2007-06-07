
#include <ServiceControl/StringVariableAttribute.h>

#include <ServiceControl/VariableAttribute.h>

using namespace Omiscid;

StringVariableAttribute::~StringVariableAttribute()
{
}

StringVariableAttribute::StringVariableAttribute(VariableAttribute* va, SimpleString value)
{
  VariableAtt = va;
  SetValue(value);
}

void StringVariableAttribute::SetValue( SimpleString value )
{
    StringValue = value;
    VariableAtt->SetValueFromControl( StringValue );
}

SimpleString StringVariableAttribute::GetValue() const
{
    return StringValue;
}































































