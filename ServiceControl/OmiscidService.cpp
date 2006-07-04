

#include <ServiceControl/OmiscidService.h>

#include <Com/MsgManager.h>
#include <Com/TcpUdpClientServer.h>
#include <ServiceControl/WaitForDnsSdServices.h>

using namespace Omiscid;
// using namespace Omiscid::OmiscidCascadeServiceFilters;

namespace Omiscid {

class OmiscidServiceSearchData
{
public:
	OmiscidServiceSearchData();
	~OmiscidServiceSearchData();

	OmiscidCascadeServiceFilters FilterList;
	OmiscidServiceProxy * Proxy;
};

bool FUNCTION_CALL_TYPE WaitForOmiscidServiceCallback(const char * fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void * UserData)
{
	OmiscidServiceSearchData * MyData = (OmiscidServiceSearchData *)UserData;

	SimpleString Host(hosttarget);

	OmiscidServiceProxy * Proxy = new OmiscidServiceProxy( Host, port );
	if ( Proxy == NULL )
	{
		return false;
	}

	for( MyData->FilterList.First(); MyData->FilterList.NotAtEnd(); MyData->FilterList.Next() )
	{
		if ( MyData->FilterList.GetCurrent()->IsAGoodService( *Proxy ) == false )
		{
			delete Proxy;
			return false;
		}
	}

	// Return the found service
	MyData->Proxy = Proxy;
	return true;
}

} // namespace Omiscid

OmiscidServiceSearchData::OmiscidServiceSearchData()
{
	Proxy = NULL;
}

OmiscidServiceSearchData::~OmiscidServiceSearchData()
{
}

OmiscidService::OmiscidService(const SimpleString& ServiceName)
	: ControlServer( ServiceName )
{

}

OmiscidService::~OmiscidService()
{

}

void OmiscidService::Start()
{
	StartServer();
	StartThreadProcessMsg();
}

/**
	* Adds a new connector to the Bip service.
	* @param connectorName the name of the connector
	* @param connectorDescription the description of the connector
	* @param connectorKind connector type. This can be input, output or input-output
	* @throws ConnectorAlreadyExisting thrown if we try to recreate an already existing connector
	* @throws IOException thrown if there is an error in the tcp socket creation
	*/
bool OmiscidService::AddConnector(SimpleString ConnectorName, SimpleString ConnectorDescription, ConnectorKind ConnectorKind)
{
	InOutputAttribut * pAtt = FindInOutput( ConnectorName );
	if ( pAtt != NULL )
	{
		TraceError( "A connector nammed '%s' is already defined.\n", ConnectorName.GetStr() );
		return false;
	}

	TcpUdpClientServer * pConnector = new TcpUdpClientServer();
	if ( pConnector == NULL )
	{
		TraceError( "Could not allocate a new connector\n" );
		return false;
	}

	// Create TCP and UDP ports
	pConnector->Create();
	pAtt = AddInOutput( ConnectorName.GetStr(), pConnector, ConnectorKind );
	pAtt->SetDescription( ConnectorDescription );

	return true;
}

	/**
	 * Sends a message to all the clients connected to the service I
	 * @param connectorName the name of the connector sending the message
	 * @param msg the message to send
	 * @throws UnknownBipService thrown if serviceId is not a declared service
	 * @throws UnknownBipConnector thrown if the service has not declared this connector
	 */
bool OmiscidService::SendToAllClients(SimpleString ConnectorName, char * Buffer, int BufferLen, bool FastSend )
{
	if ( Buffer == NULL && BufferLen != 0 )
	{
		TraceError( "Could not send (null) buffer\n" );
		return false;
	}
	InOutputAttribut * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}
	if ( pAtt->IsAnInput() )
	{
		TraceError( "Could not send data on an input connector.\n" );
		return false;
	}

	TcpUdpClientServer * pConnector = dynamic_cast<TcpUdpClientServer *>(pAtt->GetComTool());

	// if ReliableSend == true, we should send over udp so the last parameter (udp send)
	// must be true
	pConnector->SendToAll( BufferLen, Buffer, FastSend );

	return true;
}

	/**
	 * Sends a message to a particular client. This client is identified by its Peer id (pid).
	 * This method is usually used to answer a request coming from another service that
	 * has requested a connexion with us. We know this service from its pid inside its request message. We
	 * do not have a bipServiceProxy for it because we have not found this service to initiate the connexion.
	 * @param connectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param PeerId : the identification of the client that must receive the message
	 */
bool OmiscidService::SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, int PeerId, bool FastSend )
{
	if ( Buffer == NULL && BufferLen != 0 )
	{
		TraceError( "Could not send (null) buffer\n" );
		return false;
	}
	InOutputAttribut * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}
	if ( pAtt->IsAnInput() )
	{
		TraceError( "Could not send data on an input connector.\n" );
		return false;
	}

	TcpUdpClientServer * pConnector = dynamic_cast<TcpUdpClientServer *>(pAtt->GetComTool());

	// if ReliableSend == true, we should send over udp so the last parameter (udp send)
	// must be true
	if ( pConnector->SendToPeer( BufferLen, Buffer, PeerId, FastSend ) <= 0 )
	{
		return false;
	}

	return true;
}

   /**
	* Sends a message to a particular client. This client is identified by BipServiceProxy because
	* we have been looking for it to create the connexion.
	* @param connectorName the name of the connector that will send the message
	* @param msg the message to send
	*/
bool OmiscidService::SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, OmiscidServiceProxy& ServiceProxy, bool FastSend )
{
	return SendToOneClient(ConnectorName, Buffer, BufferLen, ServiceProxy.GetPeerId(), FastSend );
}

   /**
	* Creates a new Omiscid Variable
	* @param varName the variable name
	* @param type the variable type (or null if no type is associated)
	* @param accessType the access type of the variable
	* name has already been declated
	*/
bool OmiscidService::AddVariable(SimpleString VarName, SimpleString Type, SimpleString VarDescription, VariableAccess AccessType)
{
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar != NULL )
	{
		TraceError( "A variable nammed '%s' is already defined.\n", VarName.GetStr() );
		return false;
	}

	pVar = ControlServer::AddVariable( VarName.GetStr() );
	if ( pVar == NULL )
	{
		TraceError( "Could not allocate a new variable\n" );
		return false;
	}

	// Set access type
	pVar->SetType( Type );
	pVar->SetDescription( VarDescription );
	pVar->SetAccess( AccessType );

	return true;
}

   /**
	* Change a description to an existing variable
	* @param varName the var name
	* @param varDescription the description
	* @throws UnknownBipVariable thrown if the variable has not been created
	*/
bool OmiscidService::SetVariableDescription(SimpleString VarName, SimpleString VarDescription)
{
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		TraceError( "Could not find variable nammed '%s'.\n", VarName.GetStr() );
		return false;
	}

	pVar->SetDescription( VarDescription );
	return true;
}

	/**
	 * Returns the description associated to a variable
	 * @param varName the variable name
	 * @return the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
SimpleString OmiscidService::GetVariableDescription(SimpleString VarName)
{
	SimpleString Empty("");

	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		TraceError( "Could not find variable nammed '%s'.\n", VarName.GetStr() );
		return Empty;
	}

	return pVar->GetDescription();
}

	/**
	 * Sets the value of a service variable
	 * @param varName the variable name
	 * @param varValue the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
bool OmiscidService::SetVariableValue(SimpleString VarName, SimpleString VarValue)
{
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		TraceError( "Could not find variable nammed '%s'.\n", VarName.GetStr() );
		return false;
	}

	pVar->SetValue( VarValue );
	return true;
}

	/**
	 * Returns the variable value
	 * @param varName the variable name
	 * @return the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
SimpleString OmiscidService::GetVariableValue(SimpleString VarName)
{
	SimpleString Empty("");

	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		TraceError( "Could not find variable nammed '%s'.\n", VarName.GetStr() );
		return Empty;
	}

	return pVar->GetValue();
}

	/**
	 * Returns the variable access type
	 * @param varName the variable name
	 * @return the access type (SimpleString version)
	 * @throws UnknownBipVariable thrown if the variable has not been decladed
	 * @see BipService#addVariable
	 */
SimpleString OmiscidService::GetVariableAccessType(SimpleString VarName)
{
	SimpleString Empty("");

	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		TraceError( "Could not find variable nammed '%s'.\n", VarName.GetStr() );
		return Empty;
	}

	return VariableAttribut::AccessToStr(pVar->GetAccess());
}

	/**
	 * Returns the string version of the variable type
	 * @param varName the variable name
	 * @return the variable type
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
SimpleString OmiscidService::GetVariableType(SimpleString VarName)
{
	SimpleString Empty("");

	return Empty;
}

    /**
     * Connects a local connector to a remote connector of a remote service
     * @param localConnector
     * @param proxy the proxy of the remote service
     * @param remoteConnector the name of the remote connector on the remote service
     * @throws UnknownBipConnector thrown if one of the connector does not exist
     * @throws IncorrectConnectorType thrown if the coonnectors cannot connect : for instance : trying to connect an input
     * connector on another input connector.
     */
bool OmiscidService::ConnectTo(SimpleString LocalConnector, OmiscidServiceProxy& ServiceProxy, SimpleString RemoteConnector)
{
	InOutputAttribut * pAtt = FindInOutput( LocalConnector );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find local connector '%s'.\n", LocalConnector.GetStr() );
		return false;
	}

	ConnectionInfos Connection;
	if ( ServiceProxy.GetConnectionInfos( RemoteConnector, Connection ) == false )
	{
		// TraceError already done in GetConnectionInfos
		return false;
	}

	// Check compatibility
	switch( pAtt->GetType() )
	{
		case AnInput:
			// Can connect only on output and inoutput
			if ( Connection.Type == AnInput )
			{
				TraceError( "Cound not connect local input connector '%s' to another input connector '%s'\n", LocalConnector.GetStr(), RemoteConnector.GetStr() );
				return false;
			}
			break;

		case AnOutput:
			// Can connect only on input and inoutput
			if ( Connection.Type == AnOutput )
			{
				TraceError( "Cound not connect local output connector '%s' to another output connector '%s'\n", LocalConnector.GetStr(), RemoteConnector.GetStr() );
				return false;
			}
			break;

		default:	// inoutput ans other
			// Can connect to every thing
			break;

	}

	// Ok, here we go
	TcpUdpClientServer * pConnector = dynamic_cast<TcpUdpClientServer *>(pAtt->GetComTool());

	// Let's connect to him
	pConnector->ConnectTo( ServiceProxy.GetHostName().GetStr(), Connection.TcpPort, Connection.UdpPort );

	return false;
}

	/**
	 * Add a message listener to a connector
	 * @param ConnectorName the name of the connector
	 * @param MsgListener the object that will handle messages sent to this connector
	 */
bool OmiscidService::AddConnectorListener(SimpleString ConnectorName, OmiscidMessageListener * MsgListener)
{
	InOutputAttribut * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}

	if ( pAtt->IsAnOutput() )	// can be Input, InOutput, but not Output
	{
		TraceError( "Could not receive data over a simple output connector.\n" );
		return false;
	}

	// Get the connector
	TcpUdpClientServer * pConnector = dynamic_cast<TcpUdpClientServer *>(pAtt->GetComTool());

	// Link to receive messages
	pConnector->LinkToMsgManager( MsgListener );
	MsgListener->StartThread();

	return false;
}


// Utility function



// Static
OmiscidServiceProxy * OmiscidService::FindService(OmiscidServiceFilter * Filter)
{
	if ( Filter == NULL )
	{
		return NULL;
	}

	// We want to be sure that we destroy data in the rigth order
	// Let's do it by ourself
	WaitForOmiscidServices * WFOS = new WaitForOmiscidServices;
	OmiscidServiceSearchData MyData;

	MyData.FilterList.Add( Filter );

	WFOS->NeedService( "", WaitForOmiscidServiceCallback, (void*)&MyData);
	WFOS->WaitAll();

	// Delete by ourself the object. Thus we are sure that
	// we will find
	delete WFOS;

	return MyData.Proxy;
}

OmiscidServiceProxy * OmiscidService::FindService(OmiscidServiceFilter& Filter)
{
	return FindService(Filter.Duplicate());
}
