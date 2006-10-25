/* @file ServiceControl/ServiceControl/VariableAttributeListener.h
 * @ingroup ServiceControl
 * @brief Defines a way to listen to local variable (internal version)
 * @author Dominique Vaufreydaz
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __VARIABLE_ATTRIBUTE_LISTENER_H__
#define __VARIABLE_ATTRIBUTE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/VariableAttribute.h>

namespace Omiscid {

class VariableAttribute;

class VariableAttributeListener
{
public:
	VariableAttributeListener();

  /** @brief Destructor */
	virtual ~VariableAttributeListener();

private:
	// To prevent usuage of SetUSerData from the user side
	friend class VariableAttribute;
	friend class ControlServer;
	friend class Service;
	friend class ServiceProxy;
	friend class LocalVariableListener;
	friend class RemoteVariableChangeListener;

	// Get and Set user data
	void SetUserData( void * eUserData );
	void * GetUserData() const;

protected:
	virtual void VariableChanged( VariableAttribute * ChangedVariable ) = 0;
	virtual bool IsValid( VariableAttribute * ChangedVariable, SimpleString newValue ) = 0;

private:
	void * UserData;
};

} // namespace Omiscid

#endif	// __VARIABLE_ATTRIBUTE_LISTENER_H__
