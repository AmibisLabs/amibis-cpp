

#include <ServiceControl/LocalVariableListener.h>

using namespace Omiscid;

LocalVariableListener::~LocalVariableListener()
{
}

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

bool LocalVariableListener::IsValid( VariableAttribut * ChangedVariable, SimpleString NewValue )
{
	if ( GetUserData() == NULL )
	{
		// Bad call, do nothing so say the variable can change...
		return true;
	}

	// Do my job
	Service * CurrentService = static_cast<Service*>(UserData);

	return IsValid(*CurrentService, ChangedVariable->GetName(), NewValue);
}
