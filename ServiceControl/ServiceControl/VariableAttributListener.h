#ifndef __SERVICE_CONTROL_VARIABLE_ATTRIBUT_LISTENER_H__
#define __SERVICE_CONTROL_VARIABLE_ATTRIBUT_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/VariableAttribut.h>

namespace Omiscid {

class VariableAttributListener
{
public:
	VariableAttributListener();

private:
	// To prevent usuage of SetUSerData from the user side
	friend class VariableAttribut;
	friend class ControlServer;
	friend class Service;
	friend class LocalVariableListener;
	friend class RemoteVariableListener;

	// Get and Set user data
	void SetUserData( void * eUserData );
	void * GetUserData() const;

protected:
	virtual void VariableChanged( VariableAttribut * ChangedVariable ) = 0;
	virtual bool IsValid( VariableAttribut * ChangedVariable, SimpleString newValue ) = 0;

private:
	void * UserData;
};

} // namespace Omiscid

#endif	// __SERVICE_CONTROL_VARIABLE_ATTRIBUT_LISTENER_H__
