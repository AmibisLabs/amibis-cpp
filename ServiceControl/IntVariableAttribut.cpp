
#include <ServiceControl/IntVariableAttribut.h>

#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

IntVariableAttribut::IntVariableAttribut(VariableAttribut* va,
					 int value)
{
  variableAttribut = va;
  integerValue = value + 1;
  SetIntValue(value);
}

void IntVariableAttribut::SetIntValue( int value )
{
  if(integerValue != value){
    integerValue = value;
    ControlUtils::IntToStr(integerValue, variableAttribut->GetValueStr()); 
  }
}

int IntVariableAttribut::GetIntValue() const
{
	return integerValue;
}

void IntVariableAttribut::Incr()
{
	SetIntValue(integerValue + 1); 
}

void IntVariableAttribut::Decr()
{
	SetIntValue(integerValue - 1);
}
