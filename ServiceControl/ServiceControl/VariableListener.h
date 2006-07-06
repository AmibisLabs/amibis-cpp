#ifndef __SERVICE_CONTROL_VARIABLE_LISTENER_H__
#define __SERVICE_CONTROL_VARIABLE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/VariableAttribut.h>

namespace Omiscid {

class VariableAttribut;

class VariableAttributListener
{
public:
	virtual void VariableChanged( VariableAttribut * ChangedVariable, void * UserData ) = 0;
	virtual bool IsAValidChange( VariableAttribut * ChangedVariable, SimpleString newValue ) = 0;
};

} // namespace Omiscid

#endif	// __SERVICE_CONTROL_VARIABLE_LISTENER_H__
