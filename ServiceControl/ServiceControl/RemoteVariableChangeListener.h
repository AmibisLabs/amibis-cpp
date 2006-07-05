

#ifndef __SERVICE_CONTROL_REMOTE_VARIABLE_CHANGE_LISTENER_H__
#define __SERVICE_CONTROL_REMOTE_VARIABLE_CHANGE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/Variable.h>
#include <ServiceControl/Service.h>

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
    virtual void VariableChanged(Service& Service, Variable ChangedVariable) = 0;
};


} // namespace Omiscid

#endif	// __SERVICE_CONTROL_REMOTE_VARIABLE_CHANGE_LISTENER_H__
