

#include <ServiceControl/OmiscidServiceProxy.h>

#include <ServiceControl/VariableAttribut.h>

using namespace Omiscid;

OmiscidServiceProxy::OmiscidServiceProxy( SimpleString& eHostName, int eControlPort )
	: ControlClient(ComTools::GeneratePeerId())
{
	HostName	= eHostName;
	ControlPort	= eControlPort;
	if ( ConnectToCtrlServer(HostName.GetStr(), ControlPort) == false )
	{
		throw "OmiscidServiceProxy failed";
	}
	UpdateDescription();
}

OmiscidServiceProxy::~OmiscidServiceProxy()
{
}

   /**
	 * Returns the list of variables
	 * @return the list of variables
	 */
SimpleList<SimpleString>& OmiscidServiceProxy::GetVariables()
{
	return GetVariableNameList();
}

/**
	* Returns the list of connectors (input type)
	* @return the list of connectors
	*/
SimpleList<SimpleString>& OmiscidServiceProxy::GetInputConnectors()
{
	return GetInputNameList();
}

/**
    * Returns the list of connectors (output type)
    * @return the list of connectors
    */
SimpleList<SimpleString>& OmiscidServiceProxy::GetOutputConnectors()
{
	return GetOutputNameList();
}

    /**
     * Returns the list of connectors (input-output type)
     * @return the list of connectors
     */
SimpleList<SimpleString>& OmiscidServiceProxy::GetInputOutputConnectors()
{
	return GetIn_OutputNameList();
}

    /**
     * Updates the local view of a remote bip service :
     * <ul>
     * <li> the list of variables
     * <li> the list of connectors
     * </ul>
     */
void OmiscidServiceProxy::UpdateDescription()
{
	QueryGlobalDescription();
}

    /**
     * Host name where the remote service is located
     * @return the host name
     */
SimpleString OmiscidServiceProxy::GetHostName()
{
	return HostName;
}


unsigned int OmiscidServiceProxy::GetControlPort()
{
	return ControlPort;
}

/**
* The Peer Id of the remote bip service
* @return the peer id
*/
unsigned int OmiscidServiceProxy::GetPeerId()
{
	return ControlClient::GetPeerId();
}
    /**
     * Sets the new value of a remote variable
     * @param varName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool OmiscidServiceProxy::setVariableValue(SimpleString varName, SimpleString value)
{
	VariableAttribut * pVar = FindVariable(varName.GetStr());
	if ( pVar == NULL )
	{
		pVar = QueryVariableDescription( varName.GetStr() );
	}
	if ( pVar == NULL )
	{
		// Not found
		return false;
	}

	pVar->SetValueStr( value );
	return true;
}