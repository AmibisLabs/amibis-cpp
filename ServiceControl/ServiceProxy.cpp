

#include <ServiceControl/ServiceProxy.h>

#include <Com/Connector.h>
#include <ServiceControl/InOutputAttribute.h>
#include <ServiceControl/VariableAttribute.h>

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
	VariableAttribute * VarAtt;
	InOutputAttribute * IOAtt;
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
	//	 ServiceProps[ControlServer::NameString].GetValue().GetStr(), ServiceProps["id"].GetValue().GetStr() );

	if ( TmpString == "full" )
	{
		FullDescription = true;
	}

	for( Pos = 0; Pos <  ServiceProps.GetNumberOfProperties(); Pos++ )
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
				VarAtt = new OMISCID_TLM VariableAttribute( LocalProp.GetName() );
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
				VarAtt = new OMISCID_TLM VariableAttribute( LocalProp.GetName() );
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
				VarAtt = new OMISCID_TLM VariableAttribute( LocalProp.GetName() );
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

				IOAtt = new OMISCID_TLM InOutputAttribute( LocalProp.GetName(), NULL, AnInput );
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

				IOAtt = new OMISCID_TLM InOutputAttribute( LocalProp.GetName(), NULL, AnOutput );
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

				IOAtt = new OMISCID_TLM InOutputAttribute( LocalProp.GetName(), NULL, AnInOutput );
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

	//if ( FullDescription == false )
	//{
	//	UpdateDescription();
	//}
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
	if ( FullDescription == true )
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
	if ( FullDescription == true )
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
	if ( FullDescription == true )
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
	if ( FullDescription == true )
	{
		return;
	}

	if ( IsConnected() == false )
	{
		ConnectToCtrlServer( HostName, ControlPort );
	}

	if ( QueryDetailedDescription() == true )
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
	return GetVariableValue( CommonServiceValues::GetNameForNameString() );
}

/**
* The Peer Id of the remote bip service
* @return the peer id
*/
unsigned int ServiceProxy::GetPeerId()
{
	try
	{
		SimpleString lPeerId = GetVariableValue( CommonServiceValues::GetNameForPeerIdString() );

		return ComTools::PeerIdFromString(lPeerId);
	}
	catch( SimpleException & )
	{
	}

	return 0;
}

/**
* The string containing the Peer Id of the remote bip service
* @return the peer id as a string
*/
SimpleString ServiceProxy::GetPeerIdAsString()
{
	try
	{
		return GetVariableValue( CommonServiceValues::GetNameForPeerIdString() );
	}
	catch( SimpleException& )
	{
	}

	return SimpleString::EmptyString;
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

	if ( IsConnected() == false )
	{
		// Update description if needed
		if ( ConnectToCtrlServer(HostName, ControlPort) == false )
		{
			throw SimpleException("Can not connect to the Service.");
		}
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
	VariableAttribute * pVar = FindVariable(VarName);
	if ( pVar == NULL )
	{
		SimpleString ErrMesg;
		ErrMesg = "Unknown variable '";
		ErrMesg += VarName;
		ErrMesg += "'. Call HasVariableFirst.";
		throw  SimpleException( ErrMesg );
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

	if ( IsConnected() == false )
	{
		// Update description if needed
		if ( ConnectToCtrlServer(HostName, ControlPort) == false )
		{
			throw SimpleException("Can not connect to the Service.");
		}
	}

	// Il all other cases
	pVar = QueryVariableDescription( VarName );
	if ( pVar == NULL )
	{
		// This one *here* should not appear
		throw SimpleException( "Could not retrieve variable '" + VarName + "'." );
	}
	return pVar->GetValue();
}

	/**
	 * Gets the type of a remote variable
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
SimpleString ServiceProxy::GetVariableType(const SimpleString VarName)
{
	VariableAttribute * pVar = FindVariable(VarName);
	if ( pVar == NULL )
	{
		SimpleString ErrMesg;
		ErrMesg = "Unknown variable '";
		ErrMesg += VarName;
		ErrMesg += "'. Call HasVariableFirst.";
		throw  SimpleException( ErrMesg );
	}

	// If the variable is initialised
	if ( pVar->IsInitialised() )
	{
		// return the current type
		return pVar->GetType();
	}

	if ( IsConnected() == false )
	{
		// Update description if needed
		if ( ConnectToCtrlServer(HostName, ControlPort) == false )
		{
			throw SimpleException("Can not connect to the Service.");
		}
	}

	// Il all other cases
	pVar = QueryVariableDescription( VarName );
	if ( pVar == NULL )
	{
		// This one *here* should not appear
		throw SimpleException( "Could not retrieve variable '" + VarName + "'." );
	}

	return pVar->GetType();
}

	/**
	 * Gets the description of a remote variable
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
SimpleString ServiceProxy::GetVariableDescription(const SimpleString VarName)
{
	VariableAttribute * pVar = FindVariable(VarName);
	if ( pVar == NULL )
	{
		SimpleString ErrMesg;
		ErrMesg = "Unknown variable '";
		ErrMesg += VarName;
		ErrMesg += "'. Call HasVariableFirst.";
		throw  SimpleException( ErrMesg );
	}

	// If the variable is initialised
	if ( pVar->IsInitialised() )
	{
		// return the current description
		return pVar->GetDescription();
	}

	if ( IsConnected() == false )
	{
		// Update description if needed
		if ( ConnectToCtrlServer(HostName, ControlPort) == false )
		{
			throw SimpleException("Can not connect to the Service.");
		}
	}

	// Il all other cases
	pVar = QueryVariableDescription( VarName );
	if ( pVar == NULL )
	{
		// This one *here* should not appear
		throw SimpleException( "Could not retrieve variable '" + VarName + "'." );
	}

	return pVar->GetDescription();
}

	/**
	 * Add a listener to monitor variable changes
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
bool ServiceProxy::AddRemoteVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener )
{
	// Serach the variable
	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		// not found
		return false;
	}

	if ( IsConnected() == false )
	{
		// Update description if needed
		if ( ConnectToCtrlServer(HostName, ControlPort) == false )
		{
			throw SimpleException("Can not connect to the Service.");
		}
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
	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		// Not found
		return false;
	}

	if ( IsConnected() == false )
	{
		// Update description if needed
		if ( ConnectToCtrlServer(HostName, ControlPort) == false )
		{
			throw SimpleException("Can not connect to the Service.");
		}
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
	InOutputAttribute * pAtt = FindConnector(ConnectorName);
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

	/**
	 * Retrieve the description of a remote controler
	 * @param ConnectorName the name of the remote connector
	 * @return the description in a SimpleString
	 */
SimpleString ServiceProxy::GetConnectorDescription(const SimpleString ConnectorName)
{
	InOutputAttribute * pAtt;
	pAtt = FindConnector( ConnectorName );
	if ( pAtt == (InOutputAttribute *)NULL )
	{
		SimpleString ErrMesg;
		ErrMesg = "Unknown Connector '";
		ErrMesg += ConnectorName;
		ErrMesg += "'. Call HasConnector First.";
		throw  SimpleException( ErrMesg );
	}

	return pAtt->GetDescription();
}

bool ServiceProxy::GetConnectionInfos( const SimpleString Connector, ConnectionInfos& Connection )
{
	InOutputAttribute * pAtt = FindConnector( Connector );
	if ( pAtt == (InOutputAttribute *)NULL )
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
VariableAttribute * ServiceProxy::FindVariable( SimpleString VarName )
{
	// Update description if needed
	if ( FullDescription == false )
	{
		UpdateDescription();
	}

	VariableAttribute * pVar = ControlClient::FindVariable(VarName);
	// if ( pVar == NULL )
	// {
	//	 pVar = ControlClient::QueryVariableDescription( VarName );
	// }
	if ( pVar == NULL )
	{
		if ( FullDescription == true )
		{
			// Not found
			OmiscidError( "Variable '%s' not found\n", VarName.GetStr() );
			return NULL;
		}

		if ( IsConnected() == false )
		{
			// Update description if needed
			if ( ConnectToCtrlServer(HostName, ControlPort) == false )
			{
				throw SimpleException("Can not connect to the Service.");
			}
		}

		// Try to get the variable if we do not get the full description
		pVar = QueryVariableDescription(VarName);
	}
	return pVar;
}

InOutputAttribute * ServiceProxy::FindConnector( SimpleString ConnectortName )
{
	InOutputAttribute * pAtt;

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
