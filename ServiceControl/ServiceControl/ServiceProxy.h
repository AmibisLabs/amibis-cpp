/* @file
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Defines ServiceProxy class in order to interact/query properties of other services
 * @author Dominique Vaufreydaz
 * @version 1.0
 * @date	2004-2006
 */

#ifndef __SERVICE_PROXY_H__
#define __SERVICE_PROXY_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/SimpleList.h>
#include <System/SimpleString.h>

#include <ServiceControl/ControlClient.h>
#include <ServiceControl/RemoteVariableChangeListener.h>
#include <ServiceControl/ServiceProperties.h>

namespace Omiscid {

class RemoteVariableChangeListener;
class Service;

/**
 * @author
 *
 */
class ServiceProxy  : protected ControlClient
{
	// In order to access to GetConnectionInfos
	friend class Service;

public:
	ServiceProxy( unsigned int PeerId, SimpleString eHostname, int eControlPort );
	ServiceProxy( unsigned int PeerId, SimpleString eHostname, int eControlPort, ServiceProperties& ServiceProps );

	virtual ~ServiceProxy();

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
	 * <li> the list of variables (and value)
	 * <li> the list of connectors (and port)
	 * </ul>
	 */
	void UpdateDescription();

	/**
	 * Host name where the remote service is located
	 * @return the host name
	 */
	SimpleString GetHostName();

	/**
	 * The Peer Id of the remote Omiscid service
	 * @return the peer id
	 */
	unsigned int GetPeerId();

	/**
	* The string containing the Peer Id of the remote bip service
	* @return the peer id as a string
	*/
	SimpleString GetPeerIdAsString();

	/**
	 * The name of the remote Omiscid service
	 * @return name
	 */
	SimpleString GetName() ;

	/**
	 * Check availability of a variable
	 * @param VarName the name of the remote variable
	 */
	bool HasVariable(const SimpleString VarName);

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
	SimpleString GetVariableValue(const SimpleString VarName);

	/**
	 * Gets the type of a remote variable
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
	SimpleString GetVariableType(const SimpleString VarName);

	/**
	 * Gets the description of a remote variable
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
	SimpleString GetVariableDescription(const SimpleString VarName);

	/**
	 * Gets the value of a remote variable
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
	VariableAccessType GetVariableAccessTypeType(const SimpleString VarName);

	/**
	 * Add a listener to monitor variable changes
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
	bool AddRemoteVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener );

	/**
	 * Remove a listener to monitor variable changes
	 * @param VarName the name of the remote variable
	 * @param value the value (SimpleString format)
	 */
	bool RemoveRemoteVariableChangeListener(const SimpleString VarName, RemoteVariableChangeListener * Listener );

	/**
	 * search for a connector on the remote Omiscid service
	 * @param ConnectorName the name of the remote variable
	 * @return true or false
	 */
	bool HasConnector(const SimpleString ConnectorName, ConnectorKind ItsKind = UnkownConnectorKind );

	/**
	 * Find a connector using its full or local PeerId
	 */
	SimpleString FindConnector( unsigned int PeerId );

	/**
	 * Find a connector using its full or local PeerId
	 */
	SimpleString GetConnectorDescription(const SimpleString ConnectorName);

private:

	/**
	 * Gets the connection points of a remote connector
	 * @param value the value (SimpleString format)
	 */
	bool GetConnectionInfos( const SimpleString ConnectorString, ConnectionInfos& Connection );

	// Connection data
	SimpleString HostName;
	unsigned int ControlPort;

	// Internal Utility function
	VariableAttribute * FindVariable( SimpleString VarName );
	InOutputAttribute * FindConnector( SimpleString InOutputName );

	// Do we have a full description ?
	bool FullDescription;
};

} // namespace Omiscid

#endif  // __SERVICE_PROXY_H__
