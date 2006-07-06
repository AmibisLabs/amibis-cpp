#ifndef __SERVICE_CONTROL_VARIABLE_ATTRIBUT_LISTENER_H__
#define __SERVICE_CONTROL_VARIABLE_ATTRIBUT_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/Service.h>
#include <ServiceControl/VariableAttribut.h>

namespace Omiscid {

class VariableAttribut;
class Service;

class VariableAttributListener
{
	friend class VariableAttribut;
	friend class Service;

public:
	VariableAttributListener();

protected:
	virtual void VariableChanged( VariableAttribut * ChangedVariable ) = 0;
	virtual bool IsValid( VariableAttribut * ChangedVariable, SimpleString newValue ) = 0;

	// Get and Set user data
	void SetUserData( void * eUserData );
	void * GetUserData() const;

protected:
	void * UserData;
};

} // namespace Omiscid

#endif	// __SERVICE_CONTROL_VARIABLE_ATTRIBUT_LISTENER_H__
