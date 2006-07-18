

#include <ServiceControl/ServiceProxy.h>

#include <Com/TcpUdpClientServer.h>
#include <ServiceControl/InOutputAttribut.h>
#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

ServiceProxy::ServiceProxy( unsigned int PeerId, SimpleString eHostName, int eControlPort )
	: ControlClient(PeerId)
{
	HostName	= eHostName;
	ControlPort	= eControlPort;
	if ( ConnectToCtrlServer(HostName, ControlPort) == false )
	{
		throw "ServiceProxy failed";
	}
}

ServiceProxy::~ServiceProxy()
{
}

   /**
	 * Returns the list of variables
	 * @return the list of variables
	 */
SimpleList<SimpleString>& ServiceProxy::GetVariables()
{
	return GetVariableNameList();
}

/**
	* Returns the list of connectors (input type)
	* @return the list of connectors
	*/
SimpleList<SimpleString>& ServiceProxy::GetInputConnectors()
{
	return GetInputNameList();
}

/**
    * Returns the list of connectors (output type)
    * @return the list of connectors
    */
SimpleList<SimpleString>& ServiceProxy::GetOutputConnectors()
{
	return GetOutputNameList();
}

    /**
     * Returns the list of connectors (input-output type)
     * @return the list of connectors
     */
SimpleList<SimpleString>& ServiceProxy::GetInputOutputConnectors()
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
void ServiceProxy::UpdateDescription()
{
	QueryDetailedDescription();
}

    /**
     * Host name where the remote service is located
     * @return the host name
     */
SimpleString ServiceProxy::GetHostName()
{
	return HostName;
}

SimpleString ServiceProxy::GetName()
{
	return GetVariableValue( "name" );
}

/**
* The Peer Id of the remote bip service
* @return the peer id
*/
unsigned int ServiceProxy::GetPeerId()
{
	return ControlClient::GetPeerId();
}

    /**
     * Sets the new value of a remote variable
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool ServiceProxy::SetVariableValue(const SimpleString VarName, const SimpleString Value)
{
	VariableAttribut * pVar = QueryVariableModif(VarName, Value);
	if ( pVar == NULL )
	{
		return false;
	}

	return true;
}

	/**
     * Gets the value of a remote variable
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
SimpleString ServiceProxy::GetVariableValue(const SimpleString VarName)
{
	VariableAttribut * pVar = FindVariable(VarName);
	if ( pVar == NULL )
	{
		return SimpleString::EmptyString;
	}

	return pVar->GetValue();
}

	/**
     * Add a listener to monitor variable changes
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool ServiceProxy::AddRemoteVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener )
{
	// Serach the variable
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		// not found
		return false;
	}

	// Add information about me
	Listener->SetUserData( this );

	// If we can not add the listener
	if ( pVar->AddListener( Listener ) == false )
	{
		// Add information about me
		Listener->SetUserData( NULL );

		// Say there was a problem
		return false;
	}

	// If it is the first listenner, subscribe to the variable changes
	if ( pVar->GetNumberOfListeners() == 1 )
	{
		// first listener
		// Subscribe to the variable changes
		Subscribe( VarName );
	}

	// ok
	return true;
}

	/**
     * Remove a listener to monitor variable changes
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool ServiceProxy::RemoveRemoteVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener )
{
	bool ret;

	// Serach for the variable
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		// Not found	
		return false;
	}

	// Aswk to remove the listener
	ret = pVar->RemoveListener( Listener );
	if ( ret == false )
	{
		// something goes wrong
		return false;
	}

	// Do we have any listener left ?
	if ( pVar->GetNumberOfListeners() == 0 )
	{
		// If no, unsubscribe to the variable changes
		Unsubscribe( VarName );
	}

	// say, it was ok
	return true;
}



	/**
     * search for a connector on the remote Omiscid service
     * @param ConnectorName the name of the remote variable
     * @return true or false
     */
bool ServiceProxy::HasConnector( const SimpleString ConnectorName, ConnectorKind ItsKind /* UnkownConnectorKind */ )
{
	InOutputAttribut * pAtt = FindConnector(ConnectorName);
	if ( pAtt == NULL )
	{
		return false;
	}
	if ( ItsKind != UnkownConnectorKind && pAtt->GetType() != ItsKind )
	{
		return false;
	}
	return true;
}

	/**
     * search for a connector on the remote Omiscid service
     * @param ConnectorName the name of the remote variable
     * @return true or false
     */
SimpleString ServiceProxy::FindConnector( unsigned int PeerId )
{
	// Serach for the connector
	for( listInputAttr.First(); listInputAttr.NotAtEnd(); listInputAttr.Next() )
	{
		if ( listInputAttr.GetCurrent()->GetPeerId() == PeerId )
		{
			return listInputAttr.GetCurrent()->GetName();
		}
	}

	for( listOutputAttr.First(); listOutputAttr.NotAtEnd(); listOutputAttr.Next() )
	{
		if ( listOutputAttr.GetCurrent()->GetPeerId() == PeerId )
		{
			return listOutputAttr.GetCurrent()->GetName();
		}
	}

	for( listInOutputAttr.First(); listInOutputAttr.NotAtEnd(); listInOutputAttr.Next() )
	{
		if ( listInOutputAttr.GetCurrent()->GetPeerId() == PeerId )
		{
			return listInOutputAttr.GetCurrent()->GetName();
		}
	}

	return SimpleString::EmptyString;
}

bool ServiceProxy::GetConnectionInfos( const SimpleString Connector, ConnectionInfos& Connection )
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
VariableAttribut * ServiceProxy::FindVariable( SimpleString VarName )
{
	VariableAttribut * pVar = ControlClient::FindVariable(VarName);
	if ( pVar == NULL )
	{
		pVar = ControlClient::QueryVariableDescription( VarName );
	}
	if ( pVar == NULL )
	{
		// Not found
		TraceError( "Variable '%s' not found\n", VarName.GetStr() );
		return NULL;
	}
	return pVar;
}

InOutputAttribut * ServiceProxy::FindConnector( SimpleString ConnectortName )
{
	InOutputAttribut * pAtt;

	
	if (   (pAtt = FindInput(ConnectortName)) != NULL					// Is is an Input ?
		|| (pAtt = QueryInputDescription(ConnectortName)) != NULL		// again
		|| (pAtt = FindOutput(ConnectortName)) != NULL					// Is it an Output
		|| (pAtt = QueryOutputDescription(ConnectortName)) != NULL		// again
		|| (pAtt = FindInOutput(ConnectortName)) != NULL				// Is is an InOutput ?
		|| (pAtt = QueryInOutputDescription(ConnectortName)) != NULL	// again
		)
	{
		// We found it
		return pAtt;
	}

	// Here we did not find the connector
	TraceError( "Connector '%s' not found\n", ConnectortName.GetStr() );
	return NULL;
}

