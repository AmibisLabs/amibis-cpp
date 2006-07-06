

#include <ServiceControl/LocalVariableListener.h>

using namespace Omiscid;

void LocalVariableListener::VariableChanged( VariableAttribut * ChangedVariable )
{
	if ( GetUserData() == NULL )
	{
		// Bad call, do nothing...
		return;
	}

	// Do my job
	Service * CurrentService = static_cast<Service*>(UserData);

	VariableChanged(*CurrentService, ChangedVariable->GetName(), ChangedVariable->GetValue() );
}

bool LocalVariableListener::IsValid( VariableAttribut * ChangedVariable, SimpleString newValue )
{
	// For us, the changes are always legal, we just want to be sure we've got
	// the changes informations
	return true;
}