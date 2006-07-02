

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
	// UpdateDescription();
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
	return GetInOutputNameList();
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

SimpleString OmiscidServiceProxy::GetName()
{
	SimpleString ServiceName;

	GetVariableValue( "name", ServiceName );

	return ServiceName;
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
bool OmiscidServiceProxy::SetVariableValue(const SimpleString VarName, const SimpleString Value)
{
	VariableAttribut * pVar = FindVariable(VarName.GetStr());
	if ( pVar == NULL )
	{
		return false;
	}

	pVar->SetValueStr( Value );
	return true;
}

	/**
     * Gets the value of a remote variable
     * @param varName the name of the remote variable
     * @param value the value (SimpleString format)
     */
bool OmiscidServiceProxy::GetVariableValue(const SimpleString VarName, SimpleString& Value)
{
	VariableAttribut * pVar = FindVariable(VarName.GetStr());
	if ( pVar == NULL )
	{
		return false;
	}

	Value = pVar->GetValueStr();
	return true;
}

// Utility functions
VariableAttribut * OmiscidServiceProxy::FindVariable( SimpleString VarName )
{
	VariableAttribut * pVar = ControlClient::FindVariable(VarName.GetStr());
	if ( pVar == NULL )
	{
		pVar = ControlClient::QueryVariableDescription( VarName.GetStr() );
	}
	if ( pVar == NULL )
	{
		// Not found
		TraceError( "Variable '%s' not found\n", VarName.GetStr() );
		return NULL;
	}
	return pVar;
}

InOutputAttribut * OmiscidServiceProxy::FindConnector( SimpleString ConnectortName )
{
	InOutputAttribut * pAtt;
	bool FirstTry;

	for(FirstTry = true;;)
	{
		pAtt = ControlClient::FindInput( ConnectortName.GetStr() );
		if ( pAtt )
		{
			// We've got it
			return pAtt;
		}
		pAtt = ControlClient::FindOutput( ConnectortName.GetStr() );
		if ( pAtt )
		{
			// idem
			return pAtt;
		}
		pAtt = ControlClient::FindInOutput( ConnectortName.GetStr() );
		if ( pAtt )
		{
			// idem
			return pAtt;
		}
		// Here, we do not get the connector, if it is the first time, we first try to update
		// the omiscid service description, if not, we exit without finding the connector
		if ( FirstTry == false )
		{
			// exit the loop
			break;
		}
		// Update the description and loop one more time
		UpdateDescription();
		FirstTry = false;
	}
	// Here we did not find the connector
	TraceError( "Connector '%s' not found\n", ConnectortName.GetStr() );
	return NULL;
}