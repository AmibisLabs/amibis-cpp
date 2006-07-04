

#include <ServiceControl/OmiscidServiceProxy.h>

#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

OmiscidServiceProxy::OmiscidServiceProxy( SimpleString eHostName, int eControlPort )
	: ControlClient(ComTools::GeneratePeerId())
{
	HostName	= eHostName;
	ControlPort	= eControlPort;
	if ( ConnectToCtrlServer(HostName.GetStr(), ControlPort) == false )
	{
		throw "OmiscidServiceProxy failed";
	}
	// UpdateDescription();
}

OmiscidServiceProxy::~OmiscidServiceProxy()
{
}

   /**
	 * Returns the list of variables
	 * @return the list of variables
	 */
SimpleList<SimpleString>& OmiscidServiceProxy::GetVariables()
{
	return GetVariableNameList();
}

/**
	* Returns the list of connectors (input type)
	* @return the list of connectors
	*/
SimpleList<SimpleString>& OmiscidServiceProxy::GetInputConnectors()
{
	return GetInputNameList();
}

/**
    * Returns the list of connectors (output type)
    * @return the list of connectors
    */
SimpleList<SimpleString>& OmiscidServiceProxy::GetOutputConnectors()
{
	return GetOutputNameList();
}

    /**
     * Returns the list of connectors (input-output type)
     * @return the list of connectors
     */
SimpleList<SimpleString>& OmiscidServiceProxy::GetInputOutputConnectors()
{
	return GetInOutputNameList();
}

    /**
     * Updates the local view of a remote bip service :
     * <ul>
     * <li> the list of variables
     * <li> the list of connectors
     * </ul>
     */
void OmiscidServiceProxy::UpdateDescription()
{
	QueryDetailedDescription();
}

    /**
     * Host name where the remote service is located
     * @return the host name
     */
SimpleString OmiscidServiceProxy::GetHostName()
{
	return HostName;
}

unsigned int OmiscidServiceProxy::GetControlPort()
{
	return ControlPort;
}

SimpleString OmiscidServiceProxy::GetName()
{
	SimpleString ServiceName;

	GetVariableValue( "name", ServiceName );

	return ServiceName;
}

/**
* The Peer Id of the remote bip service
* @return the peer id
*/
unsigned int OmiscidServiceProxy::GetPeerId()
{
	return ControlClient::GetPeerId();
}
    /**
     * Sets the new value of a remote variable
     * @param varName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool OmiscidServiceProxy::SetVariableValue(const SimpleString VarName, const SimpleString Value)
{
	VariableAttribut * pVar = FindVariable(VarName.GetStr());
	if ( pVar == NULL )
	{
		return false;
	}

	pVar->SetValue( Value );
	return true;
}

	/**
     * Gets the value of a remote variable
     * @param varName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool OmiscidServiceProxy::GetVariableValue(const SimpleString VarName, SimpleString& Value)
{
	VariableAttribut * pVar = FindVariable(VarName.GetStr());
	if ( pVar == NULL )
	{
		return false;
	}

	Value = pVar->GetValue();
	return true;
}

	/**
     * search for a connector on the remote Omiscid Service
     * @param ConnectorName the name of the remote variable
     * @return true or false
     */
bool OmiscidServiceProxy::HasConnector(const SimpleString ConnectorName )
{
	return (FindConnector(ConnectorName) != NULL);
}

		/**
     * search for a connector on the remote Omiscid Service
     * @param ConnectorName the name of the remote variable
     * @return true or false
     */
ConnectorKind OmiscidServiceProxy::GetConnectorKind(const SimpleString ConnectorName )
{
	InOutputAttribut * pAtt = FindConnector(ConnectorName);
	if ( pAtt == NULL )
	{
		return UnkownConnectorKind;
	}
	return pAtt->GetType();
}

bool OmiscidServiceProxy::GetConnectionInfos( const SimpleString Connector, ConnectionInfos& Connection )
{
	InOutputAttribut * pAtt = FindConnector( Connector );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find connector nammed '%s'\n", Connector.GetStr() );
		return false;
	}

	Connection.TcpPort = pAtt->GetTcpPort();
	Connection.UdpPort = pAtt->GetUdpPort();
	Connection.Type	   = pAtt->GetType();

	return true;
}

// Utility functions
VariableAttribut * OmiscidServiceProxy::FindVariable( SimpleString VarName )
{
	VariableAttribut * pVar = ControlClient::FindVariable(VarName.GetStr());
	if ( pVar == NULL )
	{
		pVar = ControlClient::QueryVariableDescription( VarName.GetStr() );
	}
	if ( pVar == NULL )
	{
		// Not found
		TraceError( "Variable '%s' not found\n", VarName.GetStr() );
		return NULL;
	}
	return pVar;
}

InOutputAttribut * OmiscidServiceProxy::FindConnector( SimpleString ConnectortName )
{
	InOutputAttribut * pAtt;

	
	if (   (pAtt = FindInput(ConnectortName.GetStr())) != NULL					// Is is an Input ?
		|| (pAtt = QueryInputDescription(ConnectortName.GetStr())) != NULL		// again
		|| (pAtt = FindOutput(ConnectortName.GetStr())) != NULL					// Is it an Output
		|| (pAtt = QueryOutputDescription(ConnectortName.GetStr())) != NULL		// again
		|| (pAtt = FindInOutput(ConnectortName.GetStr())) != NULL				// Is is an InOutput ?
		|| (pAtt = QueryInOutputDescription(ConnectortName.GetStr())) != NULL	// again
		)
	{
		// We found it
		return pAtt;
	}

	// Here we did not find the connector
	TraceError( "Connector '%s' not found\n", ConnectortName.GetStr() );
	return NULL;
}

