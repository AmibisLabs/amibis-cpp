

#include <ServiceControl/RemoteVariableChangeListener.h>

using namespace Omiscid;

RemoteVariableChangeListener::~RemoteVariableChangeListener()
{
}

void RemoteVariableChangeListener::VariableChanged( VariableAttribute * ChangedVariable )
{
	if ( GetUserData() == NULL )
	{
		// Bad call, do nothing...
		return;
	}

	// Do my job
	ServiceProxy * CurrentService = static_cast<ServiceProxy*>(UserData);

	VariableChanged(*CurrentService, ChangedVariable->GetName(), ChangedVariable->GetValue() );
}

	/**
	 * This method is called when a new value is request on a variable. This method must
	 * check that this new value is a valid value. As we are a remote componant, we
	 * do not care about change. So we always say yes...
	 * @param service the service owning the variable
	 * @param currentVariable the current value of the variable
	 * @param newValue the new requested value
	 * @return true if the new value is accepted, false if rejected.
	 */
bool RemoteVariableChangeListener::IsValid( VariableAttribute * ChangedVariable, SimpleString newValue )
{
	return true;
}
