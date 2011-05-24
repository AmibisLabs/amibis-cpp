
#include <ServiceControl/IntVariableAttribute.h>

#include <System/SimpleString.h>
#include <ServiceControl/VariableAttribute.h>

using namespace Omiscid;

IntVariableAttribute::IntVariableAttribute(VariableAttribute* va, int value)
{
	VariableAtt = va;

	// Just to be sure modification will be made in SetValue, force value !
	SetValue(value, true);
}

IntVariableAttribute::~IntVariableAttribute()
{
}

void IntVariableAttribute::SetValue( int value, bool ForceChange )
{
  if ( ForceChange == true || IntegerValue != value )
  {
	  SimpleString TmpValue;

	  IntegerValue = value;
	  TmpValue += IntegerValue;
	  VariableAtt->SetValueFromControl(TmpValue);
  }
}

int IntVariableAttribute::GetValue() const
{
	return IntegerValue;
}

void IntVariableAttribute::Incr()
{
	SetValue(IntegerValue + 1);
}

void IntVariableAttribute::Decr()
{
	SetValue(IntegerValue - 1);
}

