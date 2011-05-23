/* @file LocalVariableListener.h
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Class definition for variable listenning (in a service)
 * @date 2004-2006
 */

#ifndef __LOCAL_VARIABLE_LISTENER_H__
#define __LOCAL_VARIABLE_LISTENER_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/SimpleString.h>
#include <ServiceControl/VariableAttribute.h>
#include <ServiceControl/VariableAttributeListener.h>

namespace Omiscid {

class Service;

/**
 * Defines the listener interface for all Omiscid variables of a service.
 * This interface must be implemented in order to receive
 * notifications.
 *
 * @author Dominique Vaufreydaz
 */
class LocalVariableListener : public VariableAttributeListener
{
public:
	// Virtual destructor always
	virtual ~LocalVariableListener();

	/**
	 * This method is called when the value of a variable
	 * changes
	 *
	 * @param ServiceRef the service owning the variable
	 * @param VarName the name of the variable which value has changed
	 * @param NewValue the new value of the variable
	 */
	virtual void VariableChanged(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue) = 0;

	/**
	 * This method is called when a value modification is requested on a variable. This method must
	 * check that this new value is a valid value for us.
	 * @param ServiceRef the service owning the variable
	 * @param VarName the current value of the variable
	 * @param NewValue the new requested value
	 * @return true if the new value is accepted, false if rejected.
	 */
	virtual bool IsValid(Service& ServiceRef, const SimpleString VarName, const SimpleString NewValue) = 0;

private:
	void VariableChanged( VariableAttribute * ChangedVariable );
	bool IsValid( VariableAttribute * ChangedVariable, SimpleString newValue );
};


} // namespace Omiscid

#endif	// __LOCAL_VARIABLE_LISTENER_H__
