/* @file LocalVariableListener.h
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Class definition for variable listenning (in a remote way using ServiceProxy#)
 * @date 2004-2006
 */

#ifndef __REMOTE_VARIABLE_CHANGE_LISTENER_H__
#define __REMOTE_VARIABLE_CHANGE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/ServiceProxy.h>
#include <ServiceControl/VariableAttributeListener.h>

namespace Omiscid {

class ServiceProxy;

/**
 * Defines the listener interface for all Omiscid variable of a ServiceProxy
 * This interface must be implemented in order to receive
 * notifications.
 *
 * @author Dominique Vaufreydaz
 */
class RemoteVariableChangeListener : public VariableAttributeListener
{
	friend class ServiceProxy;

public:
	// Virtual destructor always
	virtual ~RemoteVariableChangeListener();

protected:
    /**
     * Notification of a value change on a variable.
     *
     * @param SP the ServiceProxy containing the variable
     * @param VarName the name of the variable
     * @param NewValue the new value of the variable
     */
    virtual void VariableChanged(ServiceProxy& SP, const SimpleString VarName, const SimpleString NewValue ) = 0;

private:
	/**
    * This method is called when a new value is request on a variable. This method must
     * check that this new value is a valid value. As we are a remote componant, we
	 * do not care about change. So we always say yes...
     * @param service the service owning the variable
     * @param currentVariable the current value of the variable
     * @param newValue the new requested value
     * @return true if the new value is accepted, false if rejected.
     */
    bool IsValid( VariableAttribute * ChangedVariable, SimpleString newValue );

	/**
    * This method is called when the value has change is request on a variable. This method must
     * check that this new value is a valid value. As we are a remote componant, we
	 * do not care about change. So we always say yes...
     * @param service the service owning the variable
     * @param currentVariable the current value of the variable
     * @param newValue the new requested value
     * @return true if the new value is accepted, false if rejected.
     */
	void VariableChanged( VariableAttribute * ChangedVariable );
};

} // namespace Omiscid

#endif	// __REMOTE_VARIABLE_CHANGE_LISTENER_H__
