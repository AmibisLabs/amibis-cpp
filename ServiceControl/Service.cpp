

#include <ServiceControl/Service.h>

#include <Com/MsgManager.h>
#include <Com/TcpUdpClientServer.h>
#include <ServiceControl/WaitForDnsSdServices.h>
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/ServiceFilter.h>

using namespace Omiscid;

namespace Omiscid {

class OmiscidServiceSearchData
{
public:
	OmiscidServiceSearchData();
	~OmiscidServiceSearchData();

	ServiceFilter * Filter;
	unsigned int PeerId;
	ServiceProxy * Proxy;
};

bool FUNCTION_CALL_TYPE WaitForOmiscidServiceCallback(const char * fullname, const char *hosttarget, uint16_t port, uint16_t txtLen, const char *txtRecord, void * UserData)
{
	OmiscidServiceSearchData * MyData = (OmiscidServiceSearchData *)UserData;

	SimpleString Host(hosttarget);

	ServiceProxy * Proxy = new ServiceProxy( MyData->PeerId, Host, port );
	if ( Proxy == NULL )
	{
		return false;
	}

	if ( MyData->Filter->IsAGoodService( *Proxy ) == false )
	{
		delete Proxy;
		return false;
	}

	// Return the found service
	MyData->Proxy = Proxy;
	return true;
}

} // namespace Omiscid

OmiscidServiceSearchData::OmiscidServiceSearchData()
{
	Proxy  = NULL;
	Filter = NULL;
	PeerId = 0;
}

OmiscidServiceSearchData::~OmiscidServiceSearchData()
{
}

Service::Service(const SimpleString ServiceName, const SimpleString ClassName)
	: ControlServer( ServiceName )
{
	if ( ClassName.IsEmpty() )
	{
		SetClass( "Service" );
	}
	else
	{
		SetClass( ClassName );
	}
}

Service::~Service()
{
	// Remove all connector
	for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next() )
	{
		delete listInOutput.GetCurrent()->GetComTool();
		delete listInOutput.GetCurrent();
		listInOutput.RemoveCurrent();
	}
}

void Service::Start()
{
	StartServer();
}

/**
	* Adds a new connector to the Bip service.
	* @param connectorName the name of the connector
	* @param connectorDescription the description of the connector
	* @param connectorKind connector type. This can be input, output or input-output
	* @throws ConnectorAlreadyExisting thrown if we try to recreate an already existing connector
	* @throws IOException thrown if there is an error in the tcp socket creation
	*/
bool Service::AddConnector(SimpleString ConnectorName, SimpleString ConnectorDescription, ConnectorKind ConnectorKind)
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
	pAtt = AddInOutput( ConnectorName, pConnector, ConnectorKind );
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
bool Service::SendToAllClients(SimpleString ConnectorName, char * Buffer, int BufferLen, bool FastSend )
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
bool Service::SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, int PeerId, bool FastSend )
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
bool Service::SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy& ServProxy, bool FastSend )
{
	return SendToOneClient(ConnectorName, Buffer, BufferLen, ServProxy.GetPeerId(), FastSend );
}

bool Service::SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy* ServProxy, bool FastSend )
{
	if ( ServProxy == NULL )
	{
		TraceError( "Coulnd not send data to (null) proxy\n" );
		return false;
	}
	return SendToOneClient(ConnectorName, Buffer, BufferLen, ServProxy->GetPeerId(), FastSend );
}

   /**
	* Creates a new Omiscid Variable
	* @param VarName the variable name
	* @param type the variable type (or null if no type is associated)
	* @param accessType the access type of the variable
	* name has already been declated
	*/
bool Service::AddVariable(SimpleString VarName, SimpleString Type, SimpleString VarDescription, VariableAccessType AccessType)
{
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar != NULL )
	{
		TraceError( "A variable nammed '%s' is already defined.\n", VarName.GetStr() );
		return false;
	}

	pVar = ControlServer::AddVariable( VarName );
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
	* @param VarName the var name
	* @param varDescription the description
	* @throws UnknownBipVariable thrown if the variable has not been created
	*/
bool Service::SetVariableDescription(SimpleString VarName, SimpleString VarDescription)
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
	 * @param VarName the variable name
	 * @return the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
SimpleString Service::GetVariableDescription(SimpleString VarName)
{
	SimpleString Empty(SimpleString::EmptyString);

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
	 * @param VarName the variable name
	 * @param varValue the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
bool Service::SetVariableValue(SimpleString VarName, SimpleString VarValue)
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
	 * @param VarName the variable name
	 * @return the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
SimpleString Service::GetVariableValue(SimpleString VarName)
{
	SimpleString Empty(SimpleString::EmptyString);

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
	 * @param VarName the variable name
	 * @return the access type (SimpleString version)
	 * @throws UnknownBipVariable thrown if the variable has not been decladed
	 * @see BipService#addVariable
	 */
SimpleString Service::GetVariableAccessTypeType(SimpleString VarName)
{
	SimpleString Empty(SimpleString::EmptyString);

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
	 * @param VarName the variable name
	 * @return the variable type
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
SimpleString Service::GetVariableType(SimpleString VarName)
{
	SimpleString Empty(SimpleString::EmptyString);

	return Empty;
}

	/**
	 * Adds a listener that will be triggered at every variable change
	 * @param VarName the VarName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 */
bool Service::AddLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener)
{
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		return false;
	}

	// Add a pointer to myself in this listener
	Listener->SetUserData( this );

	// Ask to add the listenner
	return pVar->AddListener( Listener );
}

	/**
	 * remove a listener that was triggering at every variable change
	 * @param VarName the VarName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 */
bool Service::RemoveLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener)
{
	VariableAttribut * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		return false;
	}

	// Ask to remove the listener
	return pVar->RemoveListener( Listener );
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
bool Service::ConnectTo(SimpleString LocalConnector, ServiceProxy& ServProxy, SimpleString RemoteConnector)
{
	InOutputAttribut * pAtt = FindInOutput( LocalConnector );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find local connector '%s'.\n", LocalConnector.GetStr() );
		return false;
	}

	ConnectionInfos Connection;
	if ( ServProxy.GetConnectionInfos( RemoteConnector, Connection ) == false )
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
	pConnector->ConnectTo( ServProxy.GetHostName(), Connection.TcpPort, Connection.UdpPort );

	return false;
}

bool Service::ConnectTo(SimpleString LocalConnector, ServiceProxy* ServProxy, SimpleString RemoteConnector)
{
	if ( ServProxy == NULL )
	{
		TraceError( "Coulnd not connect to (null) proxy service\n" );
		return false;
	}
	return ConnectTo(LocalConnector, *ServProxy, RemoteConnector);
}

	/**
	 * Add a message listener to a connector
	 * @param ConnectorName the name of the connector
	 * @param MsgListener the object that will handle messages sent to this connector
	 */
bool Service::AddConnectorListener(SimpleString ConnectorName, ConnectorListener * MsgListener)
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

	MsgListener->ServiceOfTheConnector = this;
	MsgListener->ConnectorName = ConnectorName;

	// Link to receive messages
	return pConnector->AddCallbackObject( MsgListener );
}

	/**
	 * Remove a message listener from a connector
	 * @param ConnectorName the name of the connector
	 * @param MsgListener the object that will handle messages sent to this connector
	 */
bool Service::RemoveConnectorListener(SimpleString ConnectorName, ConnectorListener * MsgListener)
{
	InOutputAttribut * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		TraceError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}

	// Get the connector
	TcpUdpClientServer * pConnector = dynamic_cast<TcpUdpClientServer *>(pAtt->GetComTool());

	// Link to receive messages
	return pConnector->RemoveCallbackObject( MsgListener );
}

// Utility function



// Static
ServiceProxy * Service::FindService(ServiceFilter * Filter, unsigned int WaitTime)
{
	if ( Filter == NULL )
	{
		return NULL;
	}

	// We want to be sure that we destroy data in the rigth order
	// Let's do it by ourself
	WaitForOmiscidServices * WFOS = new WaitForOmiscidServices;
	if ( WFOS == NULL )
	{
		return NULL;
	}

	// Add serach information
	OmiscidServiceSearchData MyData;

	// Ok, I will ask for services
	MyData.PeerId = GetServiceId();
	MyData.Filter = Filter;

	// Ask to work on all service (we do not provide a name)
	WFOS->NeedService( WaitForOmiscidServiceCallback, (void*)&MyData);
	WFOS->WaitAll(WaitTime);

	// Delete by ourself the object. Thus we are sure that
	// we will do it at the right time
	delete WFOS;

	// Delete the filter
	delete Filter;

	// If not found, return NULL
	return MyData.Proxy;
}

ServiceProxy * Service::FindService(ServiceFilter& Filter, unsigned int WaitTime)
{
	return FindService(Filter.Duplicate());
}

SimpleList<ServiceProxy *>* Service::FindServices(SimpleList<ServiceFilter *>& Filters, unsigned int WaitTime )
{
	int i;

	if ( Filters.GetNumberOfElements() == 0 )
	{
		return NULL;
	}

	// We want to be sure that we destroy data in the rigth order
	// Let's do it by ourself
	WaitForOmiscidServices *    WFOS = new WaitForOmiscidServices;
	OmiscidServiceSearchData  * MyData;

	if ( WFOS == NULL )
	{
		return NULL;
	}

	for( Filters.First(); Filters.NotAtEnd(); Filters.Next() )
	{
		MyData = new OmiscidServiceSearchData;
		if ( MyData == NULL )
		{
			// delete WaitForOmiscidServices object
			delete WFOS;

			// delete all filters
			for( Filters.First(); Filters.NotAtEnd(); Filters.Next() )
			{
				// delete the current filter
				delete Filters.GetCurrent();
				// Remove it from the list
				Filters.RemoveCurrent();
			}
			// Problem, return nothing
			return NULL;
		}

		// Ok, I will ask for services
		MyData->PeerId = GetServiceId();
		MyData->Filter = Filters.GetCurrent();

		// Ask to work on all service (we do not provide a name)
		WFOS->NeedService( WaitForOmiscidServiceCallback, (void*)MyData);
	}

	// The final result
	SimpleList<ServiceProxy *>* ResultServicesProxy = NULL;

	// Let's serach for the services
	bool ret = WFOS->WaitAll(WaitTime);

	if ( ret == true && (ResultServicesProxy = new SimpleList<ServiceProxy *>) != NULL )
	{
		// We found what we need and we manadge to construct a list
		// Add the resulting proxy to the list
		for( i = 0; i < WFOS->GetNbOfSearchedServices(); i++ )
		{
			ResultServicesProxy->Add( (static_cast<OmiscidServiceSearchData*>(WFOS->operator [](i).UserData))->Proxy );
		}
	}

	// delete search object
	delete WFOS;

	// Delete all OmiscidServiceSearchData used
	for( i = 0; i < WFOS->GetNbOfSearchedServices(); i++ )
	{
		// delete the current i-th OmiscidServiceSearchData
		delete static_cast<OmiscidServiceSearchData*>(WFOS->operator [](i).UserData);
	}

	// Delete all filters
	for( Filters.First(); Filters.NotAtEnd(); Filters.Next() )
	{
		// delete the current filter
		delete Filters.GetCurrent();
		// Remove it from the list
		Filters.RemoveCurrent();
	}

	return ResultServicesProxy;
}
