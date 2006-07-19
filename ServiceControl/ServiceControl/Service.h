/* @file Service.h
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Definition of class Service. Used for creating/use services
 * @date 2004-2006
 */

#ifndef __SERVICE_H__
#define __SERVICE_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/ControlServer.h>
#include <ServiceControl/LocalVariableListener.h>
#include <ServiceControl/ServiceFilter.h> 
#include <ServiceControl/ConnectorListener.h> 

namespace Omiscid {

class LocalVariableListener;
class ControlServer;

/**
 * @class Service Service.cpp ServiceControl/Service.h
 * @ingroup UserFriendly
 * @ingroup ServiceControl
 * @brief High level user friendly class to manage an Omiscid service.
 *
 * This class is used to manadge a service and register it. It can not
 * be created directly by the end user. One should invoque the
 * Factory#Create(const SimpleString ServiceName, const SimpleString ClassName) 
 * or Factory#CreateFromXml(const SimpleString XmlDesc) to create a new service.
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
	Service(const SimpleString ServiceName, const SimpleString ClassName = DefaultServiceClassName );

	// Every one can destroy an Service
public:
	/** 
	 * Standard destructor. Will unregistered the service (if a call to Service#Create
	 * was made).
	 */
	virtual ~Service();

	/**
	 * Starts the corresponding service, i.e, registers it in DNS-SD. The service
	 * is now discoverable.
	 */
	void Start();

	/**
	 * Adds a new connector to the service.
	 * @param ConnectorName the name of the connector
	 * @param ConnectorDescription the description of the connector
	 * @param KindOfConnector the connector type. This can be AnInput, AnOutput or AnInOutput
	 */
	bool AddConnector(SimpleString ConnectorName,
			                 SimpleString ConnectorDescription,
			                 ConnectorKind KindOfConnector);

	/**
	 * Sends a message to all the clients connected to the service I
	 * @param connectorName the name of the connector sending the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	bool SendToAllClients(SimpleString ConnectorName, char * Buffer, int BufferLen, bool UnreliableButFastSend = false );


	/**
	 * Sends a message to a particular client. This client is identified by its Peer id (pid).
	 * This method is usually used to answer a request coming from another service that
	 * has requested a connexion with us. We know this service from its pid inside its request message. We
	 * do not have a ServiceProxy for it because we have not found this service to initiate the connexion.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param PeerId : the identification of the client that must receive the message
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, int PeerId, bool UnreliableButFastSend = false );

	/**
	 * Sends a mesage to a particular client. This client is identified by BipServiceProxy because
	 * we have been looking for it to create the connexion.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param ServProxy the service proxy we want to send data
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy& ServProxy, bool UnreliableButFastSend = false );

	/**
	 * Sends a mesage to a particular client. This client is identified by BipServiceProxy because
	 * we have been looking for it to create the connexion.
	 * @param ConnectorName the name of the connector that will send the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param ServProxy the service proxy we want to send data
	 * @param UnreliableButFastSend should Omiscid send data, if possible, maybe faster but with possibly message lost
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy * ServProxy, bool UnreliableButFastSend = false );

	/**
	 * Creates a new Omiscid Variable
	 * @param VarName the variable name
	 * @param Type the variable type (SimpleStrint#EmptyString or "" if no type is associated)
	 * @param VarDescription the variable description ((SimpleStrint#EmptyString or "" if no description provided)
	 * @param AccessType the access type of the variable
	 * @return true if the variable is created, false if a problem occurs or if the name has already been declared
	 */
	bool AddVariable(SimpleString VarName, SimpleString Type, SimpleString VarDescription, VariableAccessType AccessType);

	/**
	 * Change a description to an existing variable
	 * @param VarName the var name
	 * @param VarDescription the description
 	 * @return true if the new description is set, false otherwise.
	 */
	bool SetVariableDescription(SimpleString VarName, SimpleString VarDescription);

	/**
	 * Returns the description associated to a variable
	 * @param VarName the variable name
	 * @return the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableDescription(SimpleString VarName);

	/**
	 * Sets the value of a service variable
	 * @param VarName the variable name
	 * @param varValue the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	bool SetVariableValue(SimpleString VarName, SimpleString varValue);

	/**
	 * Returns the variable value
	 * @param VarName the variable name
	 * @return the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableValue(SimpleString VarName);

	/**
	 * Returns the variable access type
	 * @param VarName the variable name
	 * @return the access type (SimpleString version)
	 * @throws UnknownBipVariable thrown if the variable has not been decladed
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableAccessTypeType(SimpleString VarName);

	/**
	 * Returns the string version of the variable type
	 * @param VarName the variable name
	 * @return the variable type
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableType(SimpleString VarName);

    /**
     * Connects a local connector to a remote connector of a remote service
     * @param localConnector
     * @param proxy the proxy of the remote service
     * @param remoteConnector the name of the remote connector on the remote service
     * @throws UnknownBipConnector thrown if one of the connector does not exist
     * @throws IncorrectConnectorType thrown if the coonnectors cannot connect : for instance : trying to connect an input
     * connector on another input connector.
     */
    bool ConnectTo(SimpleString LocalConnector, ServiceProxy& ServProxy, SimpleString RemoteConnector);

    /**
     * Connects a local connector to a remote connector of a remote service
     * @param localConnector
     * @param proxy the proxy of the remote service
     * @param remoteConnector the name of the remote connector on the remote service
     * @throws UnknownBipConnector thrown if one of the connector does not exist
     * @throws IncorrectConnectorType thrown if the coonnectors cannot connect : for instance : trying to connect an input
     * connector on another input connector.
     */
    bool ConnectTo(SimpleString LocalConnector, ServiceProxy* ServProxy, SimpleString RemoteConnector);

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
	 * Adds a listener that will be triggered at every variable change
	 * @param VarName the VarName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 */
	bool AddLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener);

	/**
	 * remove a listener that was triggering at every variable change
	 * @param VarName the VarName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 */
	bool RemoveLocalVariableListener(SimpleString VarName, LocalVariableListener * Listener);

	/**
     * Finds a service on the network. The research is based on the service filter
	 * @param Wait for WaitTime ms to find the service. If WaitTime == 0, wait until a valid service is here
     * @return the service Proxy
     */
    ServiceProxy * FindService(ServiceFilter * Filter, unsigned int WaitTime = 0) ;

	/**
     * Finds a service on the network. The research is based on the service filter
     * @return the service Proxy
     */
    ServiceProxy * FindService(ServiceFilter& Filter, unsigned int WaitTime = 0);

	/**
     * Finds services on the network. The research is based on a list of service filters
     * @return a list of service Proxy. User is responsible for deleting each ServiceProxy
	 * and the list itself.
     */
	SimpleList<ServiceProxy *>* FindServices( SimpleList<ServiceFilter *>& Filters, unsigned int WaitTime );
};

} // namespace Omiscid

#endif  // __SERVICE_H__
