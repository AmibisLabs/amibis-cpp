

#ifndef __OMISCID_VARIABLE_CHANGE_LISTENER_H__
#define __OMISCID_VARIABLE_CHANGE_LISTENER_H__

#include <System/Config.h>

#include <System/SimpleString.h>
#include <Com/Message.h>

namespace Omiscid {

class OmiscidService;

/**
 * Defines the listener interface for all Omiscid variable of a service
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class OmiscidAllVariablesListener
{
	friend class OmiscidService;

protected:
    /**
     * Processes a received Omiscid message. As a given message could be processed
     * by several others listeners, the message must not be modified by its
     * processing.
     *
     * @param message
     *            the Omiscid message to process
     */
    virtual void VariableChanged(OmiscidService& Service, SimpleString VarName, SimpleString NewValue) = 0;
};

/**
 * Defines the listener interface for all Omiscid variable of a service
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class OmiscidVariableListener
{
	friend class OmiscidService;

protected:
    /**
     * Processes a received Omiscid message. As a given message could be processed
     * by several others listeners, the message must not be modified by its
     * processing.
     *
     * @param message
     *            the Omiscid message to process
     */
    virtual void VariableChanged(OmiscidService& Service, SimpleString NewValue) = 0;
};


} // namespace Omiscid

#endif	// __OMISCID_VARIABLE_CHANGE_LISTENER_H__
