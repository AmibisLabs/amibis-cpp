/*! \file
 *  \brief Header of the common classes and values for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/OmiscidServices.h>

void RegisterOmiscidService::Init( bool AutoRegister )
{
	if ( AutoRegister )
	{
		Register();
	}
}

RegisterOmiscidService::RegisterOmiscidService( const char * FullName, uint16_t ePort, bool AutoRegister /* = false */ )
	: RegisterService( FullName, ePort, /* AutoRegister = */ false )
{
	if ( strstr( FullName, OmiscidServiceDnsSdType ) == NULL )
	{
		throw ServiceException( "It do not seem to be a valid BIP service" );
	}

	Init(AutoRegister);
}

RegisterOmiscidService::RegisterOmiscidService( const char * ServiceName, const char * Domain, uint16_t ePort, bool AutoRegister /* = false */ )
	: RegisterService( ServiceName, OmiscidServiceDnsSdType, Domain, ePort, /* AutoRegister = */ false )
{
	Init(AutoRegister);
}

void RegisterOmiscidService::SetOwner()
{
#ifdef WIN32
	DWORD len;
	char UserName[512];

	len = sizeof( UserName );

	GetUserName( UserName, &len );
	Properties["owner"] = UserName;
#else
	if( getenv("LOGNAME") == NULL )
		return;
	Properties["owner"] = getenv("LOGNAME");
	// alternatively we could use getpwuid( geteuid() );
#endif
}

WaitForOmiscidServices::WaitForOmiscidServices()
: WaitForServices()
{
}

WaitForOmiscidServices::~WaitForOmiscidServices(void)
{
}
