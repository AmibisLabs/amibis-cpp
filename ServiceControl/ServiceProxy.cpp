

#include <ServiceControl/ServiceProxy.h>

#include <Com/Connector.h>
#include <ServiceControl/InOutputAttribut.h>
#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

ServiceProxy::ServiceProxy( unsigned int PeerId, SimpleString eHostName, int eControlPort )
	: ControlClient(PeerId)
{
	HostName	= eHostName;
	ControlPort	= eControlPort;
	FullDescription = false;

	if ( ConnectToCtrlServer(HostName, ControlPort) == true )
	{
		UpdateDescription();
	}
	else
	{
		// throw  SimpleException("ServiceProxy failed");
	}
}

ServiceProxy::ServiceProxy( unsigned int PeerId, SimpleString eHostName, int eControlPort, ServiceProperties& ServiceProps )
	: ControlClient(PeerId)
{
	SimpleString TmpString;
	VariableAttribut * VarAtt;
	InOutputAttribut * IOAtt;
	int Pos;
	unsigned int Port;

	HostName	= eHostName;
	ControlPort	= eControlPort;
	FullDescription = false;

	// Is the description
	TmpString = ServiceProps["desc"].GetValue();
	//if ( ServiceProps.IsDefined( "name" ) )
	//{
	//	OmiscidError( "Warning, forced fallback for '%s' (%s).\n",
	//		ServiceProps[NameString].GetValue().GetStr(), ServiceProps["id"].GetValue().GetStr() );
	//}
	//else
	//{
	//	OmiscidError( "Warning, forced fallback for %8.8x.\n", PeerId );
	//}
	//if ( TmpString == "forced fallback" )
	// OmiscidTrace( "Working on'%s' (%s).\n",
	// 	ServiceProps[ControlServer::NameString].GetValue().GetStr(), ServiceProps["id"].GetValue().GetStr() );

	if ( TmpString == "full" )
	{
		FullDescription = true;

		for( Pos = 0; FullDescription == true && Pos <  ServiceProps.GetNumberOfProperties(); Pos++ )
		{
			ServiceProperty& LocalProp = ServiceProps.GetProperty(Pos);

			if ( LocalProp.GetName() == "desc" )
			{
				continue;
			}

			// SimpleString TmpName = LocalProp.GetName();

			// Parse property
			TmpString = LocalProp.GetValue();
			char * TmpChar = (char*)TmpString.GetStr();
			switch( TmpString[0] )
			{
				// Work on Variables
				case 'c':
					if ( TmpString.GetLength() <= 2 || TmpString[1] != '/' )
					{
						FullDescription = false;
						continue;
					}

					// Ok, something like c/...
					VarAtt = new VariableAttribut( LocalProp.GetName() );
					if ( VarAtt == NULL )
					{
						FullDescription = false;
						continue;
					}
					VarAtt->SetValue( TmpChar+2 );
					VarAtt->SetAccessConstant();
					listVariableName.Add( LocalProp.GetName() );
					listVariableAttr.Add( VarAtt );
					break;

				case 'r':
					if ( TmpString.GetLength() != 1 )
					{
						FullDescription = false;
						continue;
					}

					// Ok, something like r/...
					VarAtt = new VariableAttribut( LocalProp.GetName() );
					if ( VarAtt == NULL )
					{
						FullDescription = false;
						continue;
					}
					VarAtt->SetAccessRead();
					listVariableName.Add( LocalProp.GetName() );
					listVariableAttr.Add( VarAtt );
					break;

				case 'w':
					if ( TmpString.GetLength() != 1 )
					{
						FullDescription = false;
						continue;
					}

					// Ok, something like w/...
					VarAtt = new VariableAttribut( LocalProp.GetName() );
					if ( VarAtt == NULL )
					{
						FullDescription = false;
						continue;
					}
					VarAtt->SetAccessReadWrite();
					listVariableName.Add( LocalProp.GetName() );
					listVariableAttr.Add( VarAtt );
					break;

				// Work on Connectors
				case 'i':
					if ( TmpString[1] != '/' || TmpString[2] < '0' || TmpString[2] > '9' )
					{
						FullDescription = false;
						continue;
					}

					// Check if we've got a correct port
					Port = 0;
					Port = atoi(TmpChar+2);
					if ( Port == 0 || Port >= 0x0000ffff )
					{
						FullDescription = false;
						continue;
					}

					IOAtt = new InOutputAttribut( LocalProp.GetName(), NULL, AnInput );
					if ( IOAtt == NULL )
					{
						FullDescription = false;
						continue;
					}
					IOAtt->SetTcpPort( (unsigned short)Port );
					listInputName.Add( LocalProp.GetName() );
					listInputAttr.Add( IOAtt );
					break;

				case 'o':
					if ( TmpString[1] != '/' || TmpString[2] < '0' || TmpString[2] > '9' )
					{
						FullDescription = false;
						continue;
					}

					// Check if we've got a correct port
					Port = 0;
					Port = atoi(TmpChar+2);
					if ( Port == 0 || Port >= 0x0000ffff )
					{
						FullDescription = false;
						continue;
					}

					IOAtt = new InOutputAttribut( LocalProp.GetName(), NULL, AnOutput );
					if ( IOAtt == NULL )
					{
						FullDescription = false;
						continue;
					}
					IOAtt->SetTcpPort( (unsigned short)Port );
					listOutputName.Add( LocalProp.GetName() );
					listOutputAttr.Add( IOAtt );
					break;

				case 'd':
					if ( TmpString[1] != '/' || TmpString[2] < '0' || TmpString[2] > '9' )
					{
						FullDescription = false;
						continue;
					}

					// Check if we've got a correct port
					Port = 0;
					Port = atoi(TmpChar+2);
					if ( Port == 0 || Port >= 0x0000ffff )
					{
						FullDescription = false;
						continue;
					}

					IOAtt = new InOutputAttribut( LocalProp.GetName(), NULL, AnInOutput );
					if ( IOAtt == NULL )
					{
						FullDescription = false;
						continue;
					}
					IOAtt->SetTcpPort( (unsigned short)Port );
					listInOutputName.Add( LocalProp.GetName() );
					listInOutputAttr.Add( IOAtt );
					break;

				default:
					FullDescription = false;
					continue;
			}
		}
	}

	if ( FullDescription == false )
	{
		UpdateDescription();
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
	// We got everything
	if ( FullDescription )
	{
		return GetInputNameList();
	}

	// In other case, update the description
	UpdateDescription();

	return GetInputNameList();
}

/**
    * Returns the list of connectors (output type)
    * @return the list of connectors
    */
SimpleList<SimpleString>& ServiceProxy::GetOutputConnectors()
{
	// We got everything
	if ( FullDescription )
	{
		return GetOutputNameList();
	}

	// In other case, update the description
	UpdateDescription();

	return GetOutputNameList();
}

    /**
     * Returns the list of connectors (input-output type)
     * @return the list of connectors
     */
SimpleList<SimpleString>& ServiceProxy::GetInputOutputConnectors()
{
	// We got everything
	if ( FullDescription )
	{
		return GetInOutputNameList();
	}

	// In other case, update the description
	UpdateDescription();

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
	if ( IsConnected() == false )
	{
		ConnectToCtrlServer( HostName, ControlPort );
	}

	if ( QueryDetailedDescription() )
	{
		FullDescription = true;
	}
	// else the value of FullDescription remain the same
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
	return GetVariableValue( NameString );
}

/**
* The Peer Id of the remote bip service
* @return the peer id
*/
unsigned int ServiceProxy::GetPeerId()
{
	unsigned int tmpPeerId = 0;
	SimpleString lPeerId = GetVariableValue( PeerIdString );

	if ( sscanf( lPeerId.GetStr(), "%x", &tmpPeerId ) == 1 )
	{
		return tmpPeerId;
	}

	return 0;
}

    /**
     * Sets the new value of a remote variable
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool ServiceProxy::SetVariableValue(const SimpleString VarName, const SimpleString Value)
{
	// Update description if needed
	if ( FullDescription == false )
	{
		UpdateDescription();
	}

	if ( QueryVariableModif(VarName, Value) == NULL )
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
		throw  SimpleException("Unknown variable. Call HasVariableFirst.");
	}

	// If the variable is initialised
	if ( pVar->IsInitialised() )
	{
		// If we can get the current value
		if ( pVar->GetAccess() == ConstantAccess && FullDescription == true )
		{
			return pVar->GetValue();
		}
	}

	// Il all other cases
	pVar = QueryVariableDescription( VarName );
	if ( pVar == NULL )
	{
		// This should not appear
		throw SimpleException("Unknown variable. Call HasVariableFirst.");
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

	// If it is the first listener, subscribe to the variable changes
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
     * Check availability of a variable
     * @param VarName the name of the remote variable
     */
bool ServiceProxy::HasVariable(const SimpleString VarName)
{
	return (FindVariable(VarName) != NULL);
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
	// Update description if needed
	if ( FullDescription == false )
	{
		UpdateDescription();
	}

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
		OmiscidError( "Could not find connector nammed '%s'\n", Connector.GetStr() );
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
	// Update description if needed
	if ( FullDescription == false )
	{
		UpdateDescription();
	}

	VariableAttribut * pVar = ControlClient::FindVariable(VarName);
	// if ( pVar == NULL )
	// {
	// 	pVar = ControlClient::QueryVariableDescription( VarName );
	// }
	if ( pVar == NULL )
	{
		if ( FullDescription == true )
		{
			// Not found
			OmiscidError( "Variable '%s' not found\n", VarName.GetStr() );
			return NULL;
		}

		// Try to get the variable if we do not get the full description
		pVar = QueryVariableDescription(VarName);
	}
	return pVar;
}

InOutputAttribut * ServiceProxy::FindConnector( SimpleString ConnectortName )
{
	InOutputAttribut * pAtt;

	// Update description if needed
	if ( FullDescription == false )
	{
		UpdateDescription();
	}
	
	if (   (pAtt = FindInput(ConnectortName)) != NULL					// Is is an Input ?
		|| (pAtt = FindOutput(ConnectortName)) != NULL					// Is it an Output
		|| (pAtt = FindInOutput(ConnectortName)) != NULL				// Is is an InOutput ?
		)
	{
		// We found it
		return pAtt;
	}

	// Here we did not find the connector
	OmiscidError( "Connector '%s' not found\n", ConnectortName.GetStr() );
	return NULL;
}

