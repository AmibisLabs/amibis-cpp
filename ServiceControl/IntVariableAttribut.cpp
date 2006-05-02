
#include <ServiceControl/IntVariableAttribut.h>

#include <ServiceControl/VariableAttribut.h>

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
