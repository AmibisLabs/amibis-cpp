

#include <ServiceControl/ConnectorListener.h>

using namespace Omiscid;

void ConnectorListener::Receive(MsgSocketCallBackData& CallbackData)
{
	// Call the User Friendly interface
	MessageReceived( *ServiceOfTheConnector, ConnectorName, CallbackData.Msg );
}
