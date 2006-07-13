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
#include <ServiceControl/VariableAttributListener.h>

namespace Omiscid {

class ServiceProxy;

/**
 * Defines the listener interface for all Omiscid variable of a service
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class RemoteVariableChangeListener : public VariableAttributListener
{
	friend class ServiceProxy;

public:
	// Virtual destructor always
	virtual ~RemoteVariableChangeListener();

protected:
    /**
     * Processes a received Omiscid message. As a given message could be processed
     * by several others listeners, the message must not be modified by its
     * processing.
     *
     * @param message
     *            the Omiscid message to process
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
    bool IsValid( VariableAttribut * ChangedVariable, SimpleString newValue );

	/**
    * This method is called when the value has change is request on a variable. This method must
     * check that this new value is a valid value. As we are a remote componant, we
	 * do not care about change. So we always say yes...
     * @param service the service owning the variable
     * @param currentVariable the current value of the variable
     * @param newValue the new requested value
     * @return true if the new value is accepted, false if rejected.
     */
	void VariableChanged( VariableAttribut * ChangedVariable );
};

} // namespace Omiscid

#endif	// __REMOTE_VARIABLE_CHANGE_LISTENER_H__
