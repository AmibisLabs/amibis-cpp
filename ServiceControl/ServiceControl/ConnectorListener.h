/* @file ConnectorListener.h
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Class definition for connector listenning
 * @date 2004-2006
 */

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
	// Virtual destructor always
	virtual ~ConnectorListener();

	/**
     * Processes a received Omiscid message. As a given message could be processed
     * by several others listeners, the message must not be modified by its
     * processing.
     * @param TheService the service which received the message
     * @param LocalConnectorName the name of the connector that has received the message
     * @param Msg the message to process
     */
    virtual void MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg);

    /**
     * Called when the connexion between the local service and the remote
     * service is open.
     * @param TheService A reference to the service which have a new connected Peer
     * @param LocalConnectorName The name of the connector handling the new link
     * @param PeerId the connected remote service PeerId
     */
    virtual void Connected(Service& TheService, const SimpleString LocalConnectorName, unsigned int PeerId);

    /**
     * Called when the connexion between the local service and the remote
     * service is broken.
     * @param TheService A reference to the service which losts a connection
     * @param LocalConnectorName the name of the connector handling the broken link
     * @param PeerId the disconnected remote service PeerId
     */
    virtual void Disconnected(Service& TheService, const SimpleString LocalConnectorName, unsigned int PeerId);

private:
	// Atributs
	Service    * ServiceOfTheConnector;

	void Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& CallbackData);
	void Connected(MsgSocket& ConnectionPoint, unsigned int PeerId);
	void Disconnected(MsgSocket& ConnectionPoint, unsigned int PeerId);
};

} // namespace Omiscid

#endif	// __CONNECTOR_LISTENER_H__
