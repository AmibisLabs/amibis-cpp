

#ifndef __CONNECTOR_LISTENER_H__
#define __CONNECTOR_LISTENER_H__

#include <ServiceControl/Config.h>

#include <System/Thread.h>
#include <Com/MsgSocket.h>
#include <Com/Message.h>

namespace Omiscid {

class Service;

/**
 * Defines the listener interface for Omiscid
 * messages. This interface must be implemented in order to receive
 * notifications on Omiscid messages receptions.
 *
 * @author Dominique Vaufreydaz
 */
class ConnectorListener : public MsgSocketCallbackObject
{
	friend class Service;

public:
	/**
     * Processes a received Omiscid message. As a given message could be processed
     * by several others listeners, the message must not be modified by its
     * processing.
     * @param service the service receiving the message
     * @param localConnectorName the name of the connector that has received the message
     * @param message the message to process
     */
    virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& message) = 0;

    // void messageReceived(Service service, String localConnectorName, Message message);

    /**
     * Called when the connexion between the local service and the remote
     * service is broken.
     * @param localConnectorName the name of the connector handling the broken link
     * @param peerId the disconnected remote service
     */
    // virtual void Connected(Service& TheService, const SimpleString LocalConnectorName, unsigned int PeerId) = 0;

    /**
     * Called when the connexion between the local service and the remote
     * service is broken.
     * @param localConnectorName the name of the connector handling the broken link
     * @param peerId the disconnected remote service
     */
    // virtual void Disconnected(Service& TheService, const SimpleString LocalConnectorName, unsigned int PeerId) = 0;

private:
	// Atributs
	SimpleString ConnectorName;
	Service    * ServiceOfTheConnector;

	void Receive(MsgSocketCallBackData& CallbackData);
};

} // namespace Omiscid

#endif	// __CONNECTOR_LISTENER_H__
