/*! @file
 *  @brief Header of the common classes and values for the OMiSCID service package
 *  @author Dominique Vaufreydaz
 *  @author Special thanks to Sébastien Pesnel for debugging and testing
 *  @author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date	2004-2006
 */

#include <ServiceControl/ServicesTools.h>
#include <System/Portage.h>

using namespace Omiscid;

void RegisterOmiscidService::Init( bool AutoRegister )
{
	if ( AutoRegister )
	{
		Register();
	}
}

RegisterOmiscidService::RegisterOmiscidService( const SimpleString FullName, uint16_t ePort, bool AutoRegister /* = false */ )
	: RegisterService( FullName, ePort, /* AutoRegister = */ false, /* AutoRename */ false )
{
	if ( strstr( FullName.GetStr(), GetOmiscidServiceDnsSdType().GetStr() ) == NULL )
	{
		throw ServiceException( "It do not seem to be a valid BIP service" );
	}

	Init(AutoRegister);
}

RegisterOmiscidService::RegisterOmiscidService( const SimpleString ServiceName, const SimpleString Domain, uint16_t ePort, bool AutoRegister /* = false */ )
	: RegisterService( ServiceName, GetOmiscidServiceDnsSdType(), Domain, ePort, /* AutoRegister = */ false, /* AutoRename */ false )
{
	Init(AutoRegister);
}

WaitForOmiscidServices::WaitForOmiscidServices()
: WaitForDnsSdServices()
{
}

WaitForOmiscidServices::~WaitForOmiscidServices(void)
{
}

RegisterOmiscidService::~RegisterOmiscidService()
{
}
