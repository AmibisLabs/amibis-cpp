

#ifndef __OMISCID_SERVICE_PROXY_H__
#define __OMISCID_SERVICE_PROXY_H__

#include <System/Config.h>
#include <System/SimpleList.h>
#include <System/SimpleString.h>

#include <Com/TcpUdpClientServer.h>

#include <ServiceControl/ControlClient.h>

namespace Omiscid {

/**
 * @author 
 *
 */
class OmiscidServiceProxy  : protected ControlClient
{
public:
	OmiscidServiceProxy( SimpleString& eHostname, int eControlPort );

	~OmiscidServiceProxy();

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

	unsigned int GetControlPort();

    /**
     * The Peer Id of the remote bip service
     * @return the peer id
     */
    unsigned int GetPeerId();
	
	/**
     * The Peer Id of the remote bip service
     * @return the peer id
     */
    SimpleString GetName() ;
	
	/**
     * Sets the new value of a remote variable
     * @param varName the name of the remote variable
     * @param value the value (SimpleString format)
     */
    bool SetVariableValue(const SimpleString varName, const SimpleString value);

	/**
     * Gets the value of a remote variable
     * @param varName the name of the remote variable
     * @param value the value (SimpleString format)
     */
    bool GetVariableValue(const SimpleString varName, SimpleString& value);

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

#endif  // __OMISCID_SERVICE_PROXY_H__
