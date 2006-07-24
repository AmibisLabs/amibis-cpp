

#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/Service.h>

using namespace Omiscid;

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
	TraceError( "A new peer %.8x connects to %s:%s.\n", PeerId, TheService.GetVariableValue("name").GetStr(), LocalConnectorName.GetStr() );
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
	TraceError( "The peer %.8x disconnects from %s:%s.\n", PeerId, TheService.GetVariableValue("name").GetStr(), LocalConnectorName.GetStr() );
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
	// Call the User Friendly interface
	MessageReceived( *ServiceOfTheConnector, ConnectionPoint.GetName(), CallbackData.Msg );
}

void ConnectorListener::Connected(MsgSocket& ConnectionPoint, unsigned int PeerId)
{
	Connected( *ServiceOfTheConnector, ConnectionPoint.GetName(), PeerId );
}

void ConnectorListener::Disconnected(MsgSocket& ConnectionPoint, unsigned int PeerId)
{
	Disconnected( *ServiceOfTheConnector, ConnectionPoint.GetName(), PeerId );
}
