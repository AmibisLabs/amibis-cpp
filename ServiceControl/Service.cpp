

#include <ServiceControl/Service.h>

#include <Com/MsgManager.h>
#include <Com/Connector.h>
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

bool FUNCTION_CALL_TYPE WaitForOmiscidServiceCallback(const SimpleString fullname, const SimpleString hosttarget, uint16_t port, uint16_t txtLen, const SimpleString txtRecord, void * UserData)
{
	OmiscidServiceSearchData * MyData = (OmiscidServiceSearchData *)UserData;

	SimpleString Host(hosttarget);

	// Set empty service properties regards to Omiscid Service definition
	ServiceProperties PropertiesForProxy;
	PropertiesForProxy["desc"] = "part";

	PropertiesForProxy.ImportTXTRecord( txtLen, (const unsigned char*)txtRecord.GetStr() );

#ifdef OMISCID_USE_AVAHI
	// Remove  avahi flag because it will let Omiscid thing that we need to connect to
	// the service
	PropertiesForProxy.Undefine( "org.freedesktop.Avahi.cookie" );
#endif

	// To say if the service is the one we are looking for...
	ServiceProxy * SP = new OMISCID_TLM ServiceProxy( ComTools::GeneratePeerId(), Host, port, PropertiesForProxy ); // MyData->PeerId
	if ( SP == NULL )
	{
		return false;
	}

	// DevOmiscidTrace( "Look if %s is suitable for me\n", SP->GetName().GetStr() );
	if ( MyData->Filter->IsAGoodService( *SP ) == false )
	{
		// DevOmiscidTrace( "=> no\n", SP->GetName().GetStr() );
		delete SP;
		return false;
	}

	// Return the found service
	// DevOmiscidTrace( "=> yes\n", SP->GetName().GetStr() );
	MyData->Proxy = SP;
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
		SetClass( CommonServiceValues::DefaultServiceClassName );
	}
	else
	{
		SetClass( ClassName );
	}
}

Service::~Service()
{
	// Destroy our created ComTools
	ComTools * pComTools;

	for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next() )
	{
		pComTools = listInOutput.GetCurrent()->GetComTool();
		if ( pComTools )
		{
			// Destroy it
			delete pComTools;
			// Set the com tools to null
			listInOutput.GetCurrent()->SetComTool(NULL);
		}
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
	* @param TcpPort optionnal : the TcpPort to use fotr this conenctor. Please note
	* that specifying this port may lead to problem if this port is already in use.
	* Usage reserved to very specific conditions, when using Services over several
	* local area network for example.
	*/
bool Service::AddConnector(SimpleString ConnectorName, SimpleString ConnectorDescription, ConnectorKind ConnectorKind, unsigned int TcpPort /* = 0 */)
{
	if ( GetStatus() == STATUS_RUNNING )
	{
		OmiscidError( "Could not add connector while running.\n" );
		return false;
	}

	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt != NULL )
	{
		OmiscidError( "A connector named '%s' is already defined.\n", ConnectorName.GetStr() );
		return false;
	}

	Connector * pConnector = new OMISCID_TLM Connector();
	if ( pConnector == NULL )
	{
		OmiscidError( "Could not allocate a new connector\n" );
		return false;
	}

	// Create TCP and UDP ports
	pConnector->SetName(ConnectorName);
	pConnector->Create((int)TcpPort);
	pAtt = AddInOutput( ConnectorName, pConnector, ConnectorKind );
	pAtt->SetDescription( ConnectorDescription );

	return true;
}

	/**
	 * Gets the number of clients currently connected to the given connector.
	 * This number is indicative has there can be connections and disconnections at any time.
	 *
	 * @param localConnectorName the name of the local connector
	 * @return
	 */
unsigned int Service::GetConnectorClientCount(SimpleString ConnectorName) 
{
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == (InOutputAttribute *)NULL )
	{
		SimpleString Msg = "Unknown connector '";
		Msg += ConnectorName;
		Msg += "'\n";
		throw SimpleException(  Msg );
		return false;
	}

	return pAtt->GetListPeerId().GetNumberOfElements();
}

	/**
	 * Sends a message to all the clients connected to the service I
	 * @param connectorName the name of the connector sending the message
	 * @param msg the message to send
	 */
bool Service::SendToAllClients(SimpleString ConnectorName, char * Buffer, int BufferLen, bool FastSend )
{
	if ( Buffer == NULL && BufferLen != 0 )
	{
		OmiscidError( "Could not send (null) buffer\n" );
		return false;
	}
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}
	if ( pAtt->IsAnInput() )
	{
		OmiscidError( "Could not send data on an input connector.\n" );
		return false;
	}

	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

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
		OmiscidError( "Could not send (null) buffer\n" );
		return false;
	}
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}
	if ( pAtt->IsAnInput() )
	{
		OmiscidError( "Could not send data on an input connector.\n" );
		return false;
	}

	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	// What is it ?
	// if ( pConnector->GetMaxMessageSizeForTCP() <= BufferLen )

	// if ReliableSend == true, we should send over udp so the last parameter (udp send)
	// must be true
	if ( pConnector->SendToPeer( BufferLen, Buffer, PeerId, FastSend ) <= 0 )
	{
		return false;
	}

	return true;
}

   /**
	* Sends a message to a particular client. This client is identified by ServiceProxy because
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
		OmiscidError( "Coulnd not send data to (null) proxy\n" );
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
	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar != NULL )
	{
		OmiscidError( "A variable named '%s' is already defined.\n", VarName.GetStr() );
		return false;
	}

	pVar = ControlServer::AddVariable( VarName );
	if ( pVar == NULL )
	{
		OmiscidError( "Could not allocate a new variable\n" );
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
	*/
bool Service::SetVariableDescription(SimpleString VarName, SimpleString VarDescription)
{
	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		OmiscidError( "Could not find variable named '%s'.\n", VarName.GetStr() );
		return false;
	}

	pVar->SetDescription( VarDescription );
	return true;
}

	/**
	 * Returns the description associated to a variable
	 * @param VarName the variable name
	 * @return the description
	 * @see Service#addVariable
	 */
SimpleString Service::GetVariableDescription(SimpleString VarName)
{
	SimpleString Empty(SimpleString::EmptyString);

	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		OmiscidError( "Could not find variable named '%s'.\n", VarName.GetStr() );
		return Empty;
	}

	return pVar->GetDescription();
}

	/**
	 * Sets the value of a service variable
	 * @param VarName the variable name
	 * @param varValue the variable value
	 * @see Service#addVariable
	 */
bool Service::SetVariableValue(SimpleString VarName, SimpleString VarValue, bool SkipValidationPhase /* = false */ )
{
	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		OmiscidError( "Could not find variable named '%s'.\n", VarName.GetStr() );
		return false;
	}

	if ( SkipValidationPhase == true )
	{
		pVar->SetValueFromControl( VarValue );
	}
	else
	{
		pVar->SetValue( VarValue );
	}
	return true;
}

	/**
	 * Returns the variable value
	 * @param VarName the variable name
	 * @return the variable value
	 * @see Service#addVariable
	 */
SimpleString Service::GetVariableValue(SimpleString VarName)
{
	SimpleString Empty(SimpleString::EmptyString);

	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		OmiscidError( "Could not find variable named '%s'.\n", VarName.GetStr() );
		return Empty;
	}

	return pVar->GetValue();
}

	/**
	 * Returns the variable access type
	 * @param VarName the variable name
	 * @return the access type (SimpleString version)
	 * @see Service#addVariable
	 */
SimpleString Service::GetVariableAccessTypeType(SimpleString VarName)
{
	VariableAttribute * pVar = FindVariable( VarName );
	if ( pVar == NULL )
	{
		OmiscidError( "Could not find variable named '%s'.\n", VarName.GetStr() );
		return SimpleString::EmptyString;
	}

	return VariableAttribute::AccessToStr(pVar->GetAccess());
}

	/**
	 * Returns the string version of the variable type
	 * @param VarName the variable name
	 * @return the variable type
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see Service#addVariable
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
	 */
bool Service::AddLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener)
{
	VariableAttribute * pVar = FindVariable( VarName );
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
	 */
bool Service::RemoveLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener)
{
	VariableAttribute * pVar = FindVariable( VarName );
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
	 */
bool Service::ConnectTo(SimpleString LocalConnector, ServiceProxy& ServProxy, SimpleString RemoteConnector)
{
	InOutputAttribute * pAtt = FindInOutput( LocalConnector );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", LocalConnector.GetStr() );
		return false;
	}

	ConnectionInfos Connection;
	if ( ServProxy.GetConnectionInfos( RemoteConnector, Connection ) == false )
	{
		// OmiscidError already done in GetConnectionInfos
		return false;
	}

	// Check compatibility
	switch( pAtt->GetType() )
	{
		case AnInput:
			// Can connect only on output and inoutput
			if ( Connection.Type == AnInput )
			{
				OmiscidError( "Cound not connect local input connector '%s' to another input connector '%s'\n", LocalConnector.GetStr(), RemoteConnector.GetStr() );
				return false;
			}
			break;

		case AnOutput:
			// Can connect only on input and inoutput
			if ( Connection.Type == AnOutput )
			{
				OmiscidError( "Cound not connect local output connector '%s' to another output connector '%s'\n", LocalConnector.GetStr(), RemoteConnector.GetStr() );
				return false;
			}
			break;

		default:	// inoutput ans other
			// Can connect to every thing
			break;

	}

	// Ok, here we go
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	// Let's connect to him
	try
	{
		if ( pConnector->ConnectTo( ServProxy.GetHostName(), Connection.TcpPort ) == 0 )
		{
			return false;
		}
	}
	catch(SimpleException& e)
	{
		OmiscidError( "%s (%d)\n.", e.msg.GetStr(), e.err );
		return false;
	}

	return true;
}

bool Service::ConnectTo(SimpleString LocalConnector, ServiceProxy* ServProxy, SimpleString RemoteConnector)
{
	if ( ServProxy == NULL )
	{
		OmiscidError( "Coulnd not connect to (null) proxy service\n" );
		return false;
	}
	return ConnectTo(LocalConnector, *ServProxy, RemoteConnector);
}

	/**
	 * Connects a local connector to a remote connector of a remote Service Add the ability to connect
	 * to undiscovered Services (like Services not on local area network).
	 * @param LocalConnector
	 * @param HostName of the service (IP or DNS name)
	 * @param Port (TCP Port of the remote connector).
	 */
bool Service::ConnectTo(SimpleString LocalConnector, SimpleString HostName, unsigned int TcpPort )
{
	InOutputAttribute * pAtt = FindInOutput( LocalConnector );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", LocalConnector.GetStr() );
		return false;
	}

	// Ok, here we go
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	// Let's connect to him
	try
	{
		if ( pConnector->ConnectTo( HostName, TcpPort ) == 0 )
		{
			return false;
		}
	}
	catch(SimpleException& e)
	{
		OmiscidError( "%s (%d)\n.", e.msg.GetStr(), e.err );
		return false;
	}

	return true;
}

	/**
	 * Add a message listener to a connector
	 * @param ConnectorName the name of the connector
	 * @param MsgListener the object that will handle messages sent to this connector
	 */
bool Service::AddConnectorListener(SimpleString ConnectorName, ConnectorListener * MsgListener)
{
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}

	if ( pAtt->IsAnOutput() )	// can be Input, InOutput, but limited on Output
	{
		OmiscidTrace( "Could only receive connection/deconnecion notification over %s output connector.\n", ConnectorName.GetStr() );
	}

	// Get the connector
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	MsgListener->ServiceOfTheConnector = this;

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
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}

	// Get the connector
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	// Link to receive messages
	return pConnector->RemoveCallbackObject( MsgListener );
}

	/**
     * Removes all listeners on a given connector.
     * @param ConnectorName the connector on which listeners will be removed.
     */
void Service::RemoveAllConnectorListeners(SimpleString ConnectorName)
{
	// Search connector
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return;
	}

	// Get the connector
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	// Just remove all callback objects
	pConnector->RemoveAllCallbackObjects();
}

	/**
     * Removes all listeners on a all connectors.
     */
void Service::RemoveAllConnectorListeners()
{
	// Walk amoung all connector
	for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next() )
	{
		// Get the connector
		Connector * pConnector = dynamic_cast<Connector *>(listInOutput.GetCurrent()->GetComTool());

		// Retrieve all PeerIds
		pConnector->RemoveAllCallbackObjects();
	}
}

    /**
     * Closes a connection on a given connector to a given remote peer.
     * @param connectorName the connector on which connection will be closed
     * @param peerId the id of the remote peer which connection is to be closed
 	 * @return a boolean to say if we managed to close the connection
    */
bool Service::CloseConnection(SimpleString ConnectorName, unsigned int PeerId)
{
	// Search connector
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return false;
	}

	// Get the connector
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	return pConnector->DisconnectPeerId( PeerId );
}

    /**
     * Closes a connection on a given connector to a given remote peer.
     * @param ConnectorName the connector on which connection will be closed
     * @param SrvProxy a ServiceProxy we want to close connection to.
	 * @return a boolean to say if we managed to close the connection
     */
bool Service::CloseConnection(SimpleString ConnectorName, ServiceProxy * SrvProxy)
{
	if ( SrvProxy == (ServiceProxy *)NULL )
	{
		OmiscidTrace( "Wrong parameter (NULL) for SrvProxy in Service::CloseConnection\n" );
		return false;
	}

	// call myself with the Srv
	return CloseConnection( ConnectorName, SrvProxy->GetPeerId() );
}

    /**
     * Closes all connections on a given connector.
     * @param connectorName the connector on which connections will be closed
     */
void Service::CloseAllConnections(SimpleString ConnectorName)
{
	// Search connector
	InOutputAttribute * pAtt = FindInOutput( ConnectorName );
	if ( pAtt == NULL )
	{
		OmiscidError( "Could not find local connector '%s'.\n", ConnectorName.GetStr() );
		return;
	}

	// Get the connector
	Connector * pConnector = dynamic_cast<Connector *>(pAtt->GetComTool());

	// Retrieve all PeerIds
	SimpleList<unsigned int> PeerIdsList;
	pConnector->GetListPeerId( PeerIdsList );
	while( PeerIdsList.IsNotEmpty() )
	{
		pConnector->DisconnectPeerId( PeerIdsList.ExtractFirst() );
	}
}

    /**
     * Closes all connections on all connectors.
     */
void Service::CloseAllConnections()
{
	SimpleList<unsigned int> PeerIdsList;

	// Walk amoung all connector
	for( listInOutput.First(); listInOutput.NotAtEnd(); listInOutput.Next() )
	{
		// Get the connector
		Connector * pConnector = dynamic_cast<Connector *>(listInOutput.GetCurrent()->GetComTool());

		// Retrieve all PeerIds
		pConnector->GetListPeerId( PeerIdsList );
		while( PeerIdsList.IsNotEmpty() )
		{
			pConnector->DisconnectPeerId( PeerIdsList.ExtractFirst() );
		}
	}
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
	WaitForOmiscidServices * WFOS = new OMISCID_TLM WaitForOmiscidServices;
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

	// If not found, return NULL
	return MyData.Proxy;
}

ServiceProxy * Service::FindService(ServiceFilter& Filter, unsigned int WaitTime)
{
	return FindService(&Filter, WaitTime);
}

ServiceProxyList * Service::FindServices(ServiceFilterList& Filters, unsigned int WaitTime )
{
	int i;

	if ( Filters.GetNumberOfElements() == 0 )
	{
		return NULL;
	}

	// We want to be sure that we destroy data in the rigth order
	// Let's do it by ourself
	WaitForOmiscidServices *	WFOS = new OMISCID_TLM WaitForOmiscidServices;
	OmiscidServiceSearchData  * MyData;

	if ( WFOS == NULL )
	{
		return NULL;
	}

	for( Filters.First(); Filters.NotAtEnd(); Filters.Next() )
	{
		MyData = new OMISCID_TLM OmiscidServiceSearchData;
		if ( MyData == NULL )
		{
			// delete WaitForOmiscidServices object
			delete WFOS;

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
	ServiceProxyList * ResultServicesProxy = NULL;

	// Let's serach for the services
	bool ret = WFOS->WaitAll(WaitTime);

	// Here all search threads have stopped
	if ( ret == true && (ResultServicesProxy = new OMISCID_TLM ServiceProxyList) != NULL )
	{
		// We found what we need and we manadge to construct a list
		// Add the resulting proxy to the list
		for( i = 0; i < WFOS->GetNbOfSearchedServices(); i++ )
		{
			ResultServicesProxy->AddTail( (static_cast<OmiscidServiceSearchData*>(WFOS->operator [](i).UserData))->Proxy );
		}
	}

	// Delete all OmiscidServiceSearchData used
	for( i = 0; i < WFOS->GetNbOfSearchedServices(); i++ )
	{
		// delete the current i-th OmiscidServiceSearchData
		delete static_cast<OmiscidServiceSearchData*>(WFOS->operator [](i).UserData);
	}

	// delete search object
	delete WFOS;

	return ResultServicesProxy;
}

/**
  * @brief Retrieve the name of this service
  * @return the name of this service
  */
const SimpleString Service::GetName()
{
	return serviceName;
}

	/**
	 * @brief Retrieve the PeerId of this service
	 * @return the PeerId of this service in a SimpleString::
	 */
const SimpleString Service::GetPeerIdAsString()
{
	return PeerIdVariable->GetValue();
}

	/**
	 * @brief Retrieve the PeerId of this service
	 * @return the PeerId of this service
	 */
const unsigned int Service::GetPeerId()
{
	return PeerIdFromString(PeerIdVariable->GetValue());
}

	/**
	 * Sends a message back to the sender of a message just received.
	 * Defaults to reliable send.
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param Msg the message to reply to
	  * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 * @return true if the answer was successfully send
	 */
bool Service::SendReplyToMessage( char * Buffer, int BufferLen, const Message& Msg, bool UnreliableButFastSend /* = false */ )
{
	ConnectorListener::ExtendedMessageForService * pExtMsg = dynamic_cast<ConnectorListener::ExtendedMessageForService *>((Message*)&Msg);
	if ( pExtMsg == (ConnectorListener::ExtendedMessageForService *)NULL )
	{
		// We do not have correct information
		return false;
	}

	return SendToOneClient( pExtMsg->ReceivedFromConnector, Buffer, BufferLen, pExtMsg->GetPeerId(), UnreliableButFastSend );
}

	/**
	 * Sends a message back to the sender of a message just received.
	 * Allows to specify on which connector to send the answer.
	 * Defaults to reliable send.
	 * @param ConnectorName the name of the connector sending the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param Msg the message to reply to
	  * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 * @return true if the answer was successfully send
	 */
bool Service::SendReplyToMessage(SimpleString ConnectorName, char * Buffer, int BufferLen, const Message& Msg, bool UnreliableButFastSend /* = false */ )
{
	return SendToOneClient( ConnectorName, Buffer, BufferLen, Msg.GetPeerId(), UnreliableButFastSend );
}
