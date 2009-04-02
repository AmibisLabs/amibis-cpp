/* @file Service.h
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Definition of class Service. Used for creating/use services
 * @date 2004-2006
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <ServiceControl/ControlServer.h>
#include <ServiceControl/LocalVariableListener.h>
#include <ServiceControl/ServiceFilter.h>
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/ServiceProxyList.h>

namespace Omiscid {

class LocalVariableListener;
class ControlServer;

/**
 * @class Service ServiceControl/Service.h
 * @ingroup UserFriendly
 * @ingroup ServiceControl
 * @brief High level user friendly class to manage an Omiscid service.
 *
 * This class is used to manadge a service and register it. It can not
 * be created directly by the end user. One should invoque the
 * Factory#Create(const SimpleString ServiceName, const SimpleString ClassName)
 * or Factory#CreateFromXml(const SimpleString XmlDesc) to create a new service.
 * An unregistered service can be used in order to communicate with other service.
 *
 * @author Dominique Vaufreydaz
 */
class Service : protected ControlServer
{
	// Ok, let's say, this cdlass is the only able to construct Service
	friend class Factory;

private:
	/**
	 * Create a new Omiscid service. This service will not be advertised in DSN-SD
	 * until the Service#Start method is called. This method can not be invoque
	 * by the end user. The service is usuable to connect and communicate with
	 * other services even if it is not discoverable.
	 * @param ServiceName the name of the service as it will appear in DNS-SD (maybe
	 * suffixed by a number automatically added).
	 * @param ClassName The classe name of this service. Can be usefull in order
	 * to search for services. By default, class is 'Service'.
	 */
	Service(const SimpleString ServiceName, const SimpleString ClassName = CommonServiceValues::DefaultServiceClassName );

public:
	/**
	 * Standard destructor. Will unregistered the service (if a call to Service#Start
	 * was made). This method is public; everyone can destroy a service.
	 */
	virtual ~Service();

	/**
	 * Starts the corresponding service, i.e, registers it in DNS-SD. The service
	 * is now discoverable.
	 */
	void Start();

	/**
	 * Adds a new connector to the service. This connector can be an Input
	 * an Ouput or an In/Output
	 * @param ConnectorName the name of the connector
	 * @param ConnectorDescription the description of the connector
	 * @param KindOfConnector the connector type. This can be AnInput, AnOutput or AnInOutput
	 * @param TcpPort optionnal : the TcpPort to use fotr this conenctor. Please note
	 * that specifying this port may lead to problem if this port is already in use.
	 * Usage reserved to very specific conditions, when using Services over several
	 * local area network for example.
	*/
	bool AddConnector(SimpleString ConnectorName, SimpleString ConnectorDescription, ConnectorKind KindOfConnector, unsigned int TcpPort = 0 );

	/**
	 * Gets the number of clients currently connected to the given connector.
	 * This number is indicative has there can be connections and disconnections at any time.
	 *
	 * @param localConnectorName the name of the local connector
	 * @return
	 */
	unsigned int GetConnectorClientCount(SimpleString ConnectorName) ;

	/**
	 * Sends a message to all the clients connected to a specific connector of the service
	 * @param ConnectorName the name of the connector sending the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
	 */
	bool SendToAllClients(SimpleString ConnectorName, char * Buffer, int BufferLen, bool UnreliableButFastSend = false );

	/**
	 * Sends a message to all the clients connected to a specific connector of the service
	 * @param ConnectorName the name of the connector sending the message
	 * @param MessageAsString the string containing the message
	 * @param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
	 */
	inline bool SendToAllClients(SimpleString ConnectorName, SimpleString MessageAsString, bool UnreliableButFastSend = false )
	{
		// Just call the previous one
		return SendToAllClients( ConnectorName, (char*)MessageAsString.GetStr(), MessageAsString.GetLength(), UnreliableButFastSend );
	};

	/**
	 * Sends a mesage to a particular client. This client is identified by ServiceProxy because
	 * we have been looking for it to create the connexion. We will send message to the first connector
	 * connected to this service.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param ServProxy the service proxy we want to send data
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy& ServProxy, bool UnreliableButFastSend = false );

	/**
	 * Sends a mesage to a particular client. This client is identified by ServiceProxy because
	 * we have been looking for it to create the connexion. We will send message to the first connector
	 * connected to this service.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param ServProxy the service proxy we want to send data
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	inline bool SendToOneClient(SimpleString ConnectorName, SimpleString MessageAsString, ServiceProxy& ServProxy, bool UnreliableButFastSend = false )
	{
		// Just call the previous one
		return SendToOneClient( ConnectorName, (char*)MessageAsString.GetStr(), MessageAsString.GetLength(), ServProxy, UnreliableButFastSend );
	};

	/**
	 * Sends a message to a particular client. This client is identified by ServiceProxy because
	 * we have been looking for it to create the connexion. We will send message to the first connector
	 * connected to this service.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param ServProxy the service proxy we want to send data
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy * ServProxy, bool UnreliableButFastSend = false );

	/**
	 * Sends a message contained in a string to a particular client. This client is identified by ServiceProxy because
	 * we have been looking for it to create the connexion. We will send message to the first connector
	 * connected to this service.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param MessageAsString the string containing the message
	 * @param ServProxy the service proxy we want to send data
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	inline bool SendToOneClient(SimpleString ConnectorName, SimpleString MessageAsString, ServiceProxy * ServProxy, bool UnreliableButFastSend = false )
	{
		// Just call the previous one
		return SendToOneClient( ConnectorName, (char*)MessageAsString.GetStr(), MessageAsString.GetLength(), ServProxy, UnreliableButFastSend );
	};

	/**
	 * Sends a message to a particular client. This client is identified by its Peer id (pid).
	 * This method is usually used to answer a request coming from another service that
	 * has requested a connexion with us. We know this service from its pid inside its request message (see Message#). We
	 * do not have a ServiceProxy for it because we have not found this service to initiate the connexion. If the
	 * Peer Id is a service peerid and not a connector one, the message will be sent to the
	 * first connector of the service.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param PeerId : the identification of the client that must receive the message
	 * @param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, int PeerId, bool UnreliableButFastSend = false );

	/**
	 * Sends a message to a particular client. This client is identified by its Peer id (pid).
	 * This method is usually used to answer a request coming from another service that
	 * has requested a connexion with us. We know this service from its pid inside its request message (see Message#). We
	 * do not have a ServiceProxy for it because we have not found this service to initiate the connexion. If the
	 * Peer Id is a service peerid and not a connector one, the message will be sent to the
	 * first connector of the service.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param MessageAsString the string containing the message
	 * @param PeerId : the identification of the client that must receive the message
	 * @param UnreliableButFastSend Should Omiscid send data, if possible, faster but with possibly message lost ?
	 */
	bool SendToOneClient(SimpleString ConnectorName, SimpleString MessageAsString, int PeerId, bool UnreliableButFastSend = false )
	{
		// Just call the previous one
		return SendToOneClient( ConnectorName, (char*)MessageAsString.GetStr(), MessageAsString.GetLength(), PeerId, UnreliableButFastSend );
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
	bool SendReplyToMessage( SimpleString ConnectorName, char * Buffer, int BufferLen, const Message& Msg, bool UnreliableButFastSend = false );

	/**
	 * Sends a message back to the sender of a message just received.
	 * Allows to specify on which connector to send the answer.
	 * Defaults to reliable send.
	 * @param ConnectorName the name of the connector sending the message
	 * @param MessageAsString the string containing the message
	 * @param Msg the message to reply to
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 * @return true if the answer was successfully send
	 */
	inline bool SendReplyToMessage( SimpleString ConnectorName, SimpleString MessageAsString, const Message& Msg, bool UnreliableButFastSend = false )
	{
		// just call the previous one
		return SendReplyToMessage( ConnectorName, (char*)MessageAsString.GetStr(), MessageAsString.GetLength(), Msg, UnreliableButFastSend );
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
	bool SendReplyToMessage( char * Buffer, int BufferLen, const Message& Msg, bool UnreliableButFastSend = false );

	/**
	 * Sends a message back to the sender of a message just received.
	 * Defaults to reliable send.
	 * @param MessageAsString the string containing the message
	 * @param Msg the message to reply to
	  * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 * @return true if the answer was successfully send
	 */
	inline bool SendReplyToMessage( SimpleString MessageAsString, const Message& Msg, bool UnreliableButFastSend = false )
	{
		// Just call the previous one
		return SendReplyToMessage( (char*)MessageAsString.GetStr(), MessageAsString.GetLength(), Msg, UnreliableButFastSend );
	}

public:
	/**
	 * Creates a new Omiscid Variable
	 * @param VarName the variable name
	 * @param Type the variable type (SimpleStrint#EmptyString or "" if no type is associated)
	 * @param VarDescription the variable description ((SimpleStrint#EmptyString or "" if no description provided)
	 * @param AccessType the access type of the variable. Can be ReadAccess, WriteAccess, ConstantAccess
	 * @return true if the variable is created, false if a problem occurs or if the name has already been used
	 * for a variable or a connector
	 */
	bool AddVariable(SimpleString VarName, SimpleString Type, SimpleString VarDescription, VariableAccessType AccessType);

	/**
	 * Change a description to an existing variable
	 * @param VarName the var name
	 * @param VarDescription the description
	  * @return true if the new description is set, false otherwise.
	 * @see Service#AddVariable
	 */
	bool SetVariableDescription(SimpleString VarName, SimpleString VarDescription);

	/**
	 * Returns the description associated to a variable
	 * @param VarName the variable name
	 * @return the description
	 * @see Service#AddVariable
	 */
	SimpleString GetVariableDescription(SimpleString VarName);

	/**
	 * Sets the value of a service variable
	 * @param VarName the variable name
	 * @param VarValue the new variable value
	 * @param SkipValidationPhase whether to prevent LocalVariableListener#isValid to be called
	 * @see Service#AddVariable
	 */
	bool SetVariableValue(SimpleString VarName, SimpleString VarValue, bool SkipValidationPhase = false );

	/**
	 * Returns the variable value
	 * @param VarName the variable name
	 * @return the variable value
	 * @see Service#addVariable
	 */
	SimpleString GetVariableValue(SimpleString VarName);

	/**
	 * Returns the variable access type
	 * @param VarName the variable name
	 * @return the access type
	 * @see Service#AddVariable
	 */
	VariableAccessType GetVariableAccessType(SimpleString VarName);

	/**
	 * Returns the string version of the variable type
	 * @param VarName the variable name
	 * @return the variable type
	 * @see Service#addVariable
	 */
	SimpleString GetVariableType(SimpleString VarName);

	/**
	 * @brief Retrieve the name of this service
	 * @return the name of this service
	 */
	const SimpleString GetName();

	/**
	 * @brief Retrieve the name of this service
	 * @return the name of this service
	 */
	const SimpleString GetPeerIdAsString();

		/**
	 * @brief Retrieve the name of this service
	 * @return the name of this service
	 */
	const unsigned int GetPeerId();

	/**
	 * Connects a local connector to a remote connector of a remote ServiceProxy
	 * @param LocalConnector
	 * @param ServProxy the proxy of the remote service
	 * @param RemoteConnector the name of the remote connector on the remote service
	 */
	bool ConnectTo(SimpleString LocalConnector, ServiceProxy& ServProxy, SimpleString RemoteConnector);

	/**
	 * Connects a local connector to a remote connector of a remote ServiceProxy
	 * @param LocalConnector
	 * @param ServProxy the proxy of the remote service
	 * @param RemoteConnector the name of the remote connector on the remote service
	 */
	bool ConnectTo(SimpleString LocalConnector, ServiceProxy* ServProxy, SimpleString RemoteConnector);

	/**
	 * Connects a local connector to a remote connector of a remote Service Add the ability to connect
	 * to undiscovered Services (like Services not on local area network).
	 * !!!! Warning, we can not ensure that remote connector type are compatible !!!!
	 * @param LocalConnector
	 * @param HostName of the service (IP or DNS name)
	 * @param TcpPort (TCP Port of the remote connector).
	 */
	bool ConnectTo(SimpleString LocalConnector, SimpleString HostName, unsigned int TcpPort);

	/**
	 * Add a message listener to a variable
	 * @param ConnectorName the name of the connector
	 * @param MsgListener the object that will handle messages sent to this connector
	 */
	bool AddConnectorListener(SimpleString ConnectorName, ConnectorListener * MsgListener);

	/**
	 * Remove a message listener from a connector
	 * @param ConnectorName the name of the connector
	 * @param MsgListener the object that will handle messages sent to this connector
	 */
	bool RemoveConnectorListener(SimpleString ConnectorName, ConnectorListener * MsgListener);

	/**
     * Removes all listeners on a given connector.
     * @param ConnectorName the connector on which listeners will be removed.
     */
    void RemoveAllConnectorListeners(SimpleString ConnectorName);

	/**
     * Removes all listeners on a all connectors.
     */
	void RemoveAllConnectorListeners();

    /**
     * Closes a connection on a given connector to a given remote peer.
     * @param ConnectorName the connector on which connection will be closed
     * @param PeerId the id of the remote peer which connection is to be closed
     */
    bool CloseConnection(SimpleString ConnectorName, unsigned int PeerId);

    /**
     * Closes a connection on a given connector to a given remote peer.
     * @param ConnectorName the connector on which connection will be closed
     * @param SrvProxy a ServiceProxy we want to close connection to.
     */
	bool CloseConnection(SimpleString ConnectorName, ServiceProxy * SrvProxy);
    
    /**
     * Closes all connections on a given connector.
     * @param connectorName the connector on which connections will be closed
     */
    void CloseAllConnections(SimpleString ConnectorName);

    /**
     * Closes all connections on all connectors.
     */
    void CloseAllConnections();

	/**
	 * Adds a listener that will be triggered at every variable change
	 * @param VarName the variable name
	 * @param Listener the adresse of the listener
	 * @see LocalVariableListener
	 */
	bool AddLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener);

	/**
	 * remove a listener that was triggering at every variable change
	 * @param VarName the variable name
	 * @param Listener the adresse of the listener
	 * @see Service#AddLocalVariableListener
	 */
	bool RemoveLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener);

	/**
	 * Finds a service on the network. The research is based on a ServiceFilter
	 * @param Filter A filter in order to check if a service is valid for us. This Filter will be destroy
	 * during the function call. You <B>must</B> not provide adresses of stack objects but usually
	 * call function such as NameIs(const SimpleString Name) to create a filter.
	 * @param WaitTime Wait for WaitTime ms to find the service. If WaitTime == 0, wait until a valid service is here
	 * @return a ServiceProxy if we can find a suitable one in the assigned time, NULL otherwise.
	 * @see ServiceFilter
	 */
	ServiceProxy * FindService(ServiceFilter * Filter, unsigned int WaitTime = 0) ;

	/**
	 * Finds a service on the network. The research is based on a ServiceFilter
	 * @param Filter A filter in order to check if a service is valid for us. This Filter will be destroy
	 * during the function call. You <B>can</B> not provide adresses of stack objects, this method
	 * do *no longer* call ServiceFilter#Duplicate ont the Filter.
	 * @param WaitTime Wait for WaitTime ms to find the service. If WaitTime == 0, wait until a valid service is here
	 * @return a ServiceProxy if we can find a suitable one in the assigned time, NULL otherwise.
	 * @see ServiceFilter
	 */
	ServiceProxy * FindService(ServiceFilter& Filter, unsigned int WaitTime = 0);

	/**
	 * Finds a service on the network. The research is based on a ServiceFilter
	 * @param Filters A ServiceFilterList who contain for all search services, a filter to find it. If the
	 * same search is done over 2 different services, the result can be computed only if there are
	 * 2 different services that are valids.
	 * @param WaitTime Wait for WaitTime ms to find the service. If WaitTime == 0, wait until a valid service is here
	 * @return a ServiceProxyList containing all found services. NULL if we can not find <B>all of them</B>
	 * in the assigned time.
	 * @see ServiceFilter
	 */
	ServiceProxyList * FindServices( ServiceFilterList& Filters, unsigned int WaitTime = 0 );

};

} // namespace Omiscid

#endif  // __SERVICE_H__
