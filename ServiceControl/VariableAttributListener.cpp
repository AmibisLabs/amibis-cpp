
#include <ServiceControl/VariableAttributListener.h>

using namespace Omiscid;


VariableAttributListener::VariableAttributListener()
{
	UserData = NULL;
}

// Get and Set user data
void VariableAttributListener::SetUserData( void * eUserData )
{
	UserData = eUserData;
}

void * VariableAttributListener::GetUserData() const
{
	return UserData;
}
