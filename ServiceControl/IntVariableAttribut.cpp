
#include <ServiceControl/IntVariableAttribut.h>

#include <System/SimpleString.h>
#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

IntVariableAttribut::IntVariableAttribut(VariableAttribut* va, int value)
{
  VariableAtt = va;
  SetValue(value);
}

void IntVariableAttribut::SetValue( int value )
{
  if(IntegerValue != value)
  {
	  SimpleString TmpValue;
	
	  IntegerValue = value;
	  TmpValue += IntegerValue;
	  VariableAtt->SetValue(TmpValue); 
  }
}

int IntVariableAttribut::GetValue() const
{
	return IntegerValue;
}

void IntVariableAttribut::Incr()
{
	SetValue(IntegerValue + 1); 
}

void IntVariableAttribut::Decr()
{
	SetValue(IntegerValue - 1);
}
