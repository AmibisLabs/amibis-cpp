

#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/Service.h>

using namespace Omiscid;

ConnectorListener::ExtendedMessageForService::ExtendedMessageForService(Message& ToCopy)
{
	buffer			= ToCopy.GetBuffer();
	len				= ToCopy.GetLength();
	mid				= ToCopy.GetMsgId();
	OriginalSize	= 0;
	origine			= ToCopy.GetOrigine();
	pid				= ToCopy.GetPeerId();
	realBuffer		= (char*)NULL;
}

ConnectorListener::ExtendedMessageForService::~ExtendedMessageForService()
{
	// To prevent multiple free of buffer, enpty myself
	buffer			= (char*)NULL;
	len				= 0;
	mid				= 0;
	origine			= UnknownOrigine;
	pid				= 0;

	// Done in constructor
	OriginalSize	= 0;
	realBuffer		= (char*)NULL;
}

ConnectorListener::~ConnectorListener()
{
}

	/**
	 * Called when the connexion between the local service and the remote
	 * service is open.
	 * @param TheService A reference to the service which have a new connected Peer
	 * @param LocalConnectorName The name of the connector handling the new link
	 * @param PeerId the connected remote service PeerId
	 */
void ConnectorListener::Connected(Service& TheService, const SimpleString LocalConnectorName, unsigned int PeerId)
{
	OmiscidTrace( "A new peer %8.8x connects to %s:%s.\n", PeerId, TheService.GetName().GetStr(), LocalConnectorName.GetStr() );
}

	/**
	 * Called when the connexion between the local service and the remote
	 * service is broken.
	 * @param TheService A reference to the service which losts a connection
	 * @param LocalConnectorName the name of the connector handling the broken link
	 * @param PeerId the disconnected remote service PeerId
	 */
void ConnectorListener::Disconnected(Service& TheService, const SimpleString LocalConnectorName, unsigned int PeerId)
{
	OmiscidTrace( "The peer %8.8x disconnects from %s:%s.\n", PeerId, TheService.GetName().GetStr(), LocalConnectorName.GetStr() );
}

	/**
	 * Processes a received Omiscid message. As a given message could be processed
	 * by several others listeners, the message must not be modified by its
	 * processing.
	 * @param service the service receiving the message
	 * @param localConnectorName the name of the connector that has received the message
	 * @param Msg the message to process
	 */
void ConnectorListener::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
}

void ConnectorListener::Receive(MsgSocket& ConnectionPoint, MsgSocketCallBackData& CallbackData)
{
	ExtendedMessageForService ExtMsg(CallbackData.Msg);
	ExtMsg.ReceivedFromConnector = ConnectionPoint.GetName();

	// Call the User Friendly interface
	MessageReceived( *ServiceOfTheConnector, ConnectionPoint.GetName(), ExtMsg );
}

void ConnectorListener::Connected(MsgSocket& ConnectionPoint, unsigned int PeerId)
{
	Connected( *ServiceOfTheConnector, ConnectionPoint.GetName(), PeerId );
}

void ConnectorListener::Disconnected(MsgSocket& ConnectionPoint, unsigned int PeerId)
{
	Disconnected( *ServiceOfTheConnector, ConnectionPoint.GetName(), PeerId );
}
