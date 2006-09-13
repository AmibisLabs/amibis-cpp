/* @file
 * @ingroup ServiceControl
 * @brief Defines a way to listen to local variable (internal version)
 * @author Dominique Vaufreydaz
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __VARIABLE_ATTRIBUT_LISTENER_H__
#define __VARIABLE_ATTRIBUT_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/VariableAttribut.h>

namespace Omiscid {

class VariableAttribut;

class VariableAttributListener
{
public:
	VariableAttributListener();

  /** @brief Destructor */
	virtual ~VariableAttributListener();

private:
	// To prevent usuage of SetUSerData from the user side
	friend class VariableAttribut;
	friend class ControlServer;
	friend class Service;
	friend class ServiceProxy;
	friend class LocalVariableListener;
	friend class RemoteVariableChangeListener;

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

#endif	// __VARIABLE_ATTRIBUT_LISTENER_H__
