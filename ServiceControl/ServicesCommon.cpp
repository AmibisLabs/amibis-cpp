/*! \file
 *  \brief Implementation file of the common classes for the OMiSCID service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date	2004-2006
 */

#include <ServiceControl/ServicesCommon.h>

#include <System/Portage.h>

using namespace Omiscid;

ServiceException::ServiceException( const SimpleString Message, int Err )
  : SimpleException(Message, Err)
{}

ServiceException::ServiceException(const ServiceException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{
}

ServiceException::~ServiceException( )
{
}

SimpleString ServiceException::GetExceptionType() const
{
	return SimpleString("ServiceException");
}

const SimpleString CommonServiceValues::GetOmiscidServiceDnsSdType()
{
	static SimpleString OmiscidServiceDnsSdType;

	if ( OmiscidServiceDnsSdType.IsEmpty() )
	{
		SimpleString DefaultDomain = "_bip._tcp";

		char * Option = getenv( "OMISCID_WORKING_DOMAIN" );
		if ( Option == NULL || DefaultDomain == Option )
		{
			OmiscidServiceDnsSdType = DefaultDomain;
			OmiscidTrace( "OMISCID_WORKING_DOMAIN not override. Use '%s'.\n", DefaultDomain.GetStr() );
			return OmiscidServiceDnsSdType;
		}

		// Copy the environment variable
		size_t size = strlen( Option );
		TemporaryMemoryBuffer tmpdomain(128);

		if ( size >= RegtypeLength )
		{
			OmiscidServiceDnsSdType = DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN too long (%d max). Use '%s' instead.\n", RegtypeLength-1, DefaultDomain.GetStr() );
			return OmiscidServiceDnsSdType;
		}

		if ( sscanf( Option, "_bip_%[^.]._tcp", (char*)tmpdomain) != 1 )
		{
			OmiscidServiceDnsSdType = DefaultDomain;
			OmiscidError( "OMISCID_WORKING_DOMAIN do not look like '_bip_XXX._tcp'. Use '%s' instead.\n", DefaultDomain.GetStr() );
			return OmiscidServiceDnsSdType;
		}

		// OmiscidServiceDnsSdType = DefaultDomain;
		OmiscidServiceDnsSdType = Option;
		OmiscidTrace( "OMISCID_WORKING_DOMAIN defined in environment variable. Use '%s'.\n", OmiscidServiceDnsSdType.GetStr() );
	}

	return OmiscidServiceDnsSdType;
}

const SimpleString CommonServiceValues::GetDefaultServiceClassName()
{
	return SimpleString("Service");
}

// Constant string values for service registration
const SimpleString CommonServiceValues::GetNameForLockString()
{
	return SimpleString( "lock" );
}

const SimpleString CommonServiceValues::GetNameForNameString()
{
	return SimpleString( "name" );
}

const SimpleString CommonServiceValues::GetNameForOwnerString()
{
	return SimpleString( "owner" );
}

const SimpleString CommonServiceValues::GetNameForClassString()
{
	return SimpleString( "class" );
}

const SimpleString CommonServiceValues::GetNameForPeerIdString()
{
	return SimpleString( "peerId" );
}

void CommonServiceValues::InitFromLayer()
{
}
