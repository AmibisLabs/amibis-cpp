
#include <ServiceControl/VariableAttributeListener.h>

using namespace Omiscid;

VariableAttributeListener::~VariableAttributeListener()
{
}

VariableAttributeListener::VariableAttributeListener()
{
	UserData = NULL;
}

// Get and Set user data
void VariableAttributeListener::SetUserData( void * eUserData )
{
	UserData = eUserData;
}

void * VariableAttributeListener::GetUserData() const
{
	return UserData;
}
