

#ifndef __OMISCIDSERVICE_H__
#define __OMISCIDSERVICE_H__

#include <System/Portage.h>

namespace Omiscid {

/**
 * @author Patrick Reignier (UJF/Gravir)
 *
 */
public BipService
{

#if 0

    /** Variable for Read Access*/
    public static final String READ = "read";
    /** Variable for Read and Write Access*/
    public static final String READ_WRITE = "read_write";
    /** Object for Read-Write Access before init */
    public static final String READ_WRITE_BEFORE_INIT = "read_write_before_init";


	/**
	 * Adds a new connector to the Bip service.
	 * @param connectorName the name of the connector
	 * @param connectorDescription the description of the connector
	 * @param connectorKind connector type. This can be input, output or input-output
	 * @throws ConnectorAlreadyExisting thrown if we try to recreate an already existing connector
	 * @throws IOException thrown if there is an error in the tcp socket creation
	 */
	public void addConnector(String connectorName,
			                 String connectorDescription,
			                 InOutputKind connectorKind) throws ConnectorAlreadyExisting, IOException;

	/**
	 * Add a message listener to a connector
	 * @param connectorName the name of the connector
	 * @param msgListener the object that will handle messages sent to this connector
	 * @throws UnknownBipConnector thrown if the service has not declared this connector
	 */
	public void addConnectorListener(String connectorName,
									BipMessageListener msgListener) throws UnknownBipConnector;

	/**
	 * Starts the corresponding service
	 */
	public void start();

	/**
	 * Stops the bip service : closes all the connections and unregisters the service
	 */
	public void stop() ;


	/**
	 * Sends a message to a particular client. This client is identified by its Peer id (pid).
	 * This method is usually used to answer a request coming from another service that
	 * has requested a connexion with us. We know this service from its pid inside its request message. We
	 * do not have a bipServiceProxy for it because we have not found this service to initiate the connexion.
	 * @param connectorName the name of the connector that will send the message
	 * @param msg the message to send
	 * @param pid peer id : the identification of the client that must receive the message
	 * @throws UnknownBipConnector thrown if the service has not declared this connector
	 * @see BipService#sendToOneClient(String, byte[], BipServiceProxy)
	 */
	public void sendToOneClient(String connectorName, byte[] msg, int pid)
	           throws UnknownBipConnector;

	/**
	 * Sends a message to a particular client. This client is identified by BipServiceProxy because
	 * we have been looking for it to create the connexion.
	 * @param connectorName the name of the connector that will send the message
	 * @param msg the message to send
	 * @param bipServiceProxy : the proxy of the remote service
	 * @throws UnknownBipConnector thrown if the service has not declared this connector
	 * @see BipService#sendToOneClient(String, byte[], int)
	 */
	public void sendToOneClient(String connectorName, byte[] msg, BipServiceProxy bipServiceProxy)
	           throws UnknownBipConnector;


	/**
	 * Sends a message to all the clients connected to the service I
	 * @param connectorName the name of the connector sending the message
	 * @param msg the message to send
	 * @throws UnknownBipService thrown if serviceId is not a declared service
	 * @throws UnknownBipConnector thrown if the service has not declared this connector
	 */
	public void sendToAllClients(String connectorName, byte[] msg)
				throws UnknownBipConnector;

	/**
	 * Sets the value of a service variable
	 * @param varName the variable name
	 * @param varValue the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	public void setVariableValue(String varName, String varValue)
				throws UnknownBipVariable ;

	/**
	 * Returns the variable value
	 * @param varName the variable name
	 * @return the variable value
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	public String getVariableValue(String varName)
				throws UnknownBipVariable ;

	/**
	 * Creates a new Bip Variable
	 * @param varName the variable name
	 * @param type the variable type (or null if no type is associated)
	 * @param accessType the access type of the variable
	 * @see BipService#READ
	 * @see BipService#READ_WRITE
	 * @see BipService#READ_WRITE_BEFORE_INIT
	 * @throws VariableAlreadyExisting thrown if a variable with the same
	 * name has already been declated
	 */
	public void addVariable(String varName, String type, String accessType)
			throws VariableAlreadyExisting ;

	/**
	 * Associate a description to an existing variable
	 * @param varName the var name
	 * @param varDescription the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 */
	public void setVariableDescription(String varName, String varDescription)
			throws UnknownBipVariable ;

	/**
	 * Returns the description associated to a variable
	 * @param varName the variable name
	 * @return the description
	 * @throws UnknownBipVariable thrown if the variable has not been created
	 * @see BipService#addVariable
	 */
	public String getVariableDescription(String varName)
			throws UnknownBipVariable ;

	/**
	 * Returns the variable access type
	 * @param varName the variable name
	 * @return the access type (String version)
	 * @throws UnknownBipVariable thrown if the variable has not been decladed
	 * @see BipService#addVariable
	 */
	public String getVariableAccessType(String varName)
			throws UnknownBipVariable ;

	/**
	 * Returns the string version of the variable type
	 * @param varName the variable name
	 * @return the variable type
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
	public String getVariableType(String varName)
			throws UnknownBipVariable ;

	/**
	 * Adds a listener that will be triggered at every variable change
	 * @param varName the varName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
	public void addVariableChangeListener(String varName, final BipVariableChangeListener listener)
			throws UnknownBipVariable ;

	/**
	 * Removes a listener on a variable change
	 * @param varName the varName
	 * @param listener the listener
	 * @throws UnknownBipVariable thrown if the variable has not been declared
	 * @see BipService#addVariable
	 */
	public void removeVariableChangeListener(String varName, VariableChangeListener listener)
			throws UnknownBipVariable ;

	/**
	 * Removes a connector listener
	 * @param connector the connector name
	 * @param listener the connector listener
	 * @throws UnknownBipConnector the connector does not exist
	 */
	public void removeConnectorListener(String connector, BipMessageListener listener) throws UnknownBipConnector ;

    /**
     * Finds a list of services on the network. The research is based on the service names (as registered in DNS_SD)
     * @param services the array of service names
     * @return the list of associated services proxy
     * @see BipServiceProxy
     */
    public HashMap<String, BipServiceProxy> findServices(String[] services, OmiscidServiceFilter[] filters) ;

    /**
     * Finds a service on the network. The research is based on the service names (as registered in DNS_SD)
     * @param service the service name
     * @return the service Proxy
     */
    public BipServiceProxy findService(String service, OmiscidServiceFilter filters) ;

    /**
     * Connects a local connector to a remote connector of a remote service
     * @param localConnector
     * @param proxy the proxy of the remote service
     * @param remoteConnector the name of the remote connector on the remote service
     * @throws UnknownBipConnector thrown if one of the connector does not exist
     * @throws IncorrectConnectorType thrown if the coonnectors cannot connect : for instance : trying to connect an input
     * connector on another input connector.
     */
    public void connectTo(String localConnector, BipServiceProxy proxy, String remoteConnector)
    		throws UnknownBipConnector, IncorrectConnectorType;

#endif // #if 0
};

} // namespace Omiscid

#endif  // __OMISCIDSERVICE_H__