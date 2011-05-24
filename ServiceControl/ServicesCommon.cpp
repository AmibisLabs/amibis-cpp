/*! @file
 *  @brief Implementation file of the common classes for the OMiSCID service package
 *  @author Dominique Vaufreydaz
 *  @author Special thanks to Sébastien Pesnel for debugging and testing
 *  @author Special thanks to Julien Letessier for his kind help about DNS-SD
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
			OmiscidTrace( "OMiSCID working domain not override. Use '%s'.\n", DefaultDomain.GetStr() );
			return OmiscidServiceDnsSdType;
		}

		// Copy the environment variable
		size_t size = strlen( Option );
		TemporaryMemoryBuffer tmpdomain(128);

		if ( size >= RegtypeLength )
		{
			SimpleString Msg = "OMiSCID working domain '";
			Msg += Option;
			Msg += "' too long (";
			Msg += (RegtypeLength-1);
			Msg += ")";

			// Set default domain in cas of exception catch 
			OmiscidServiceDnsSdType = DefaultDomain;
			
			OmiscidError( Msg.GetStr() );
			throw SimpleException( Msg );
		}

		if ( sscanf( Option, "_bip_%[^.]._tcp", (char*)tmpdomain) == 1 )
		{
			// Set default domain in cas of exception catch 
			OmiscidServiceDnsSdType = DefaultDomain;
			
			OmiscidError( "Old style OMiSCID working domain, please conform to new 'XXX._bip._tcp' pattern" );
			throw SimpleException( "Old style OMiSCID working domain, please conform to new 'XXX._bip._tcp' pattern" );
		}

		if ( sscanf( Option, "%[^.]._bip._tcp", (char*)tmpdomain) == 1 )
		{
			OmiscidServiceDnsSdType = Option;
			OmiscidTrace( "OMISCID_WORKING_DOMAIN defined in environment variable. Use '%s'.\n", OmiscidServiceDnsSdType.GetStr() );
			return OmiscidServiceDnsSdType;
		}

		// OMiSCID Domain simplest way to define it
		OmiscidServiceDnsSdType = Option;
		OmiscidTrace( "OMISCID_WORKING_DOMAIN defined in environment variable. Use '%s'.\n", OmiscidServiceDnsSdType.GetStr() );
	}

	return OmiscidServiceDnsSdType;
}

const SimpleString CommonServiceValues::DefaultServiceClassName("Service");

// Constant string values for service registration
const SimpleString CommonServiceValues::NameForLockString( "lock" );
const SimpleString CommonServiceValues::NameForNameString( "name" );
const SimpleString CommonServiceValues::NameForOwnerString( "owner" );
const SimpleString CommonServiceValues::NameForClassString( "class" );
const SimpleString CommonServiceValues::NameForPeerIdString( "peerId" );


void CommonServiceValues::InitFromLayer()
{
}

