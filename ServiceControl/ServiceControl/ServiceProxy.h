

#ifndef __SERVICE_PROXY_H__
#define __SERVICE_PROXY_H__

#include <ServiceControl/Config.h>

#include <System/SimpleList.h>
#include <System/SimpleString.h>

#include <ServiceControl/ControlClient.h>
#include <ServiceControl/RemoteVariableChangeListener.h>

namespace Omiscid {

class RemoteVariableChangeListener;

/**
 * @author 
 *
 */
class ServiceProxy  : protected ControlClient
{
public:
	ServiceProxy( SimpleString eHostname, int eControlPort );

	~ServiceProxy();

   /**
	 * Returns the list of variables
	 * @return the list of variables
	 */
	SimpleList<SimpleString>& GetVariables();

	/**
	 * Returns the list of connectors (input type)
	 * @return the list of connectors
	 */
	SimpleList<SimpleString>& GetInputConnectors();

    /**
     * Returns the list of connectors (output type)
     * @return the list of connectors
     */
    SimpleList<SimpleString>& GetOutputConnectors();

    /**
     * Returns the list of connectors (input-output type)
     * @return the list of connectors
     */
    SimpleList<SimpleString>& GetInputOutputConnectors();
	
	/**
     * Updates the local view of a remote Omiscid service :
     * <ul>
     * <li> the list of variables
     * <li> the list of connectors
     * </ul>
     */
    void UpdateDescription();

    /**
     * Host name where the remote service is located
     * @return the host name
     */
    SimpleString GetHostName();


    /**
     * Retrieve the control port of this service
     * @return the control port
     */
	unsigned int GetControlPort();

    /**
     * The Peer Id of the remote Omiscid service
     * @return the peer id
     */
    unsigned int GetPeerId();
	
	/**
     * The name of the remote Omiscid service
     * @return name
     */
    SimpleString GetName() ;
	
	/**
     * Sets the new value of a remote variable
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
    bool SetVariableValue(const SimpleString VarName, const SimpleString value);

	/**
     * Gets the value of a remote variable
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
    bool GetVariableValue(const SimpleString VarName, SimpleString& value);

	/**
     * Add a listener to monitor variable changes
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
    bool AddVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener );

	/**
     * Remove a listener to monitor variable changes
     * @param VarName the name of the remote variable
     * @param value the value (SimpleString format)
     */
    bool RemoveVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener );

	/**
     * search for a connector on the remote Omiscid service
     * @param ConnectorName the name of the remote variable
     * @return true or false
     */
    bool HasConnector(const SimpleString ConnectorName );

		/**
     * search for a connector on the remote Omiscid service
     * @param ConnectorName the name of the remote variable
     * @return UnknowKind if the connecotr do not exists, AnInput, AnOutput, AnInOutput otherwise
     */
    ConnectorKind GetConnectorKind(const SimpleString ConnectorName );

	/**
     * Gets the connection points of a remote connector
     * @param value the value (SimpleString format)
     */
	bool GetConnectionInfos( const SimpleString Connector, ConnectionInfos& Connection );

private:
	SimpleString HostName;
	unsigned int ControlPort;

	// Internal Utility function
	VariableAttribut * FindVariable( SimpleString VarName );
	InOutputAttribut * FindConnector( SimpleString InOutputName );
};

} // namespace Omiscid

#endif  // __SERVICE_PROXY_H__
