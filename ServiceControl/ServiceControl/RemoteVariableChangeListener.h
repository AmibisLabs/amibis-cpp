

#ifndef __REMOTE_VARIABLE_CHANGE_LISTENER_H__
#define __REMOTE_VARIABLE_CHANGE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/ServiceProxy.h>

namespace Omiscid {

/**
 * Defines the listener interface for all Omiscid variable of a service
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class RemoteVariableChangeListener
{
	friend class Service;

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
};


} // namespace Omiscid

#endif	// __REMOTE_VARIABLE_CHANGE_LISTENER_H__
