

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
	TraceError( "A new peer %.8x dicconnects from %s:%s.\n", PeerId, TheService.GetVariableValue("name").GetStr(), LocalConnectorName.GetStr() );
}

void ConnectorListener::MessageReceived(Service& TheService, const SimpleString LocalConnectorName, const Message& Msg)
{
}

void ConnectorListener::Receive(MsgSocketCallBackData& CallbackData)
{
	// Call the User Friendly interface
	MessageReceived( *ServiceOfTheConnector, ConnectorName, CallbackData.Msg );
}

void ConnectorListener::Connected(unsigned int PeerId)
{
	Connected( *ServiceOfTheConnector, ConnectorName, PeerId );
}

void ConnectorListener::Disconnected(unsigned int PeerId)
{
	Disconnected( *ServiceOfTheConnector, ConnectorName, PeerId );
}
