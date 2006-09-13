
#include <ServiceControl/IntVariableAttribut.h>

#include <System/SimpleString.h>
#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

IntVariableAttribut::IntVariableAttribut(VariableAttribut* va, int value)
{
	VariableAtt = va;
	
	// Just to be sure modification will be made in SetValue, force value !
	SetValue(value, true);
}

IntVariableAttribut::~IntVariableAttribut()
{
}

void IntVariableAttribut::SetValue( int value, bool ForceChange )
{
  if ( ForceChange == true || IntegerValue != value )
  {
	  SimpleString TmpValue;
	
	  IntegerValue = value;
	  TmpValue += IntegerValue;
	  VariableAtt->SetValueFromControl(TmpValue);
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
