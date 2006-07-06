

#ifndef __MESSAGE_LISTENER_H__
#define __MESSAGE_LISTENER_H__

#include <ServiceControl/Config.h>

#include <System/Thread.h>
#include <Com/MsgManager.h>

namespace Omiscid {

class Service;

/**
 * Defines the listener interface for Omiscid
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class MessageListener : protected MsgManager, public Thread
{
	friend class Service;

public:
    /**
     * Processes a received Omiscid message. As a given message could be processed
     * by several others listeners, the message must not be modified by its
     * processing.
     *
     * @param message
     *            the Omiscid message to process
     */
    virtual void ReceivedOmiscidMessage(const Message& message) = 0;

protected:
	virtual void Run();
	virtual void ProcessAMessage( Message * Msg );
};


} // namespace Omiscid

#endif	// __MESSAGE_LISTENER_H__
