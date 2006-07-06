

#ifndef __SERVICE_CONTROL_SERVICE_H__
#define __SERVICE_CONTROL_SERVICE_H__

#include <ServiceControl/Config.h>

#include <ServiceControl/ControlServer.h>
#include <ServiceControl/ServiceFilter.h> 
#include <ServiceControl/MessageListener.h> 
#include <ServiceControl/LocalVariableListener.h>

namespace Omiscid {
/**
 * @author Dominique Vaufreydaz
 *
 */
class Service : protected ControlServer
{
	// Ok, let's say, this cdlass is the only able to construct Service
	friend class Factory;

private:
	Service(const SimpleString ServiceName);

	// Every one can destroy an Service
public:
	~Service();

	/**
	 * Starts the corresponding service
	 */
	void Start();

	/**
	 * Adds a new connector to the Bip service.
	 * @param connectorName the name of the connector
	 * @param connectorDescription the description of the connector
	 * @param connectorKind connector type. This can be AnInput, AnOutput or AnInOutput
	 */
	bool AddConnector(SimpleString ConnectorName,
			                 SimpleString ConnectorDescription,
			                 ConnectorKind KindOfConnector);

	/**
	 * Sends a message to all the clients connected to the service I
	 * @param connectorName the name of the connector sending the message
	 * @param Buffer the message to send
	 * @param BufferLen the length of message to send
	 * @param FastSend should Omiscid send data maybe faster but with possibly message lost
	 */
	bool SendToAllClients(SimpleString ConnectorName, char * Buffer, int BufferLen, bool FastSend = false );


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
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, int PeerId, bool FastSend = false );

	/**
	 * Sends a message to a particular client. This client is identified by BipServiceProxy because
	 * we have been looking for it to create the connexion.
	 * @param connectorName the name of the connector that will send the message
	 * @param msg the message to send
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy& ServProxy, bool FastSend = false );

	/**
	 * Sends a message to a particular client. This client is identified by BipServiceProxy because
	 * we have been looking for it to create the connexion.
	 * @param connectorName the name of the connector that will send the message
	 * @param msg the message to send
	 */
	bool SendToOneClient(SimpleString ConnectorName, char * Buffer, int BufferLen, ServiceProxy * ServProxy, bool FastSend = false );

	/**
	 * Creates a new Omiscid Variable
	 * @param varName the variable name
	 * @param type the variable type (or null if no type is associated)
	 * @param accessType the access type of the variable
	 * name has already been declated
	 */
	bool AddVariable(SimpleString VarName, SimpleString type, SimpleString VarDescription, VariableAccess AccessType);

	/**
	 * Change a description to an existing variable
	 * @param varName the var name
	 * @param varDescription the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 */
	bool SetVariableDescription(SimpleString VarName, SimpleString VarDescription);

	/**
	 * Returns the description associated to a variable
	 * @param varName the variable name
	 * @return the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableDescription(SimpleString VarName);

	/**
	 * Sets the value of a service variable
	 * @param varName the variable name
	 * @param varValue the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	bool SetVariableValue(SimpleString varName, SimpleString varValue);

	/**
	 * Returns the variable value
	 * @param varName the variable name
	 * @return the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableValue(SimpleString varName);

	/**
	 * Returns the variable access type
	 * @param varName the variable name
	 * @return the access type (SimpleString version)
	 * @throws UnknownBipVariable thrown if the variable has not been decladed
	 * @see BipService#addVariable
	 */
	SimpleString GetVariableAccessType(SimpleString VarName);

	/**
	 * Returns the string version of the variable type
	 * @param varName the variable name
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
	bool AddConnectorListener(SimpleString ConnectorName, MessageListener * MsgListener);

	/**
	 * Adds a listener that will be triggered at every variable change
	 * @param varName the varName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 */
	bool AddVariableChangeListener(SimpleString VarName, LocalVariableListener * Listener);

	/**
	 * remove a listener that was triggering at every variable change
	 * @param varName the varName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 */
	bool Service::RemoveVariableChangeListener(SimpleString VarName, LocalVariableListener * Listener);

	/**
     * Finds a service on the network. The research is based on the service filter
	 * @params Wait for WaitTime ms to find the service. If WaitTime == 0, wait until a valid service is here
     * @return the service Proxy
     */
    static ServiceProxy * FindService(ServiceFilter * Filter, unsigned int WaitTime = 0) ;

	/**
     * Finds a service on the network. The research is based on the service filter
     * @return the service Proxy
     */
    static ServiceProxy * FindService(ServiceFilter& Filter, unsigned int WaitTime = 0);


#if 0

	/**
	 * Removes a listener on a variable change
	 * @param varName the varName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
	public void removeVariableChangeListener(SimpleString varName, VariableChangeListener listener)
			throws UnknownBipVariable ;

	/**
	 * Removes a connector listener
	 * @param connector the connector name
	 * @param listener the connector listener
	 * @throws UnknownBipConnector the connector does not exist
	 */
	public void removeConnectorListener(SimpleString connector, BipMessageListener listener) throws UnknownBipConnector ;

    /**
     * Finds a list of services on the network. The research is based on the service names (as registered in DNS_SD)
     * @param services the array of service names
     * @return the list of associated services proxy
     * @see BipServiceProxy
     */
    public HashMap<SimpleString, BipServiceProxy> findServices(SimpleString[] services, ServiceFilter[] filters) ;
#endif

};

} // namespace Omiscid

#endif  // __SERVICE_CONTROL_SERVICE_H__
