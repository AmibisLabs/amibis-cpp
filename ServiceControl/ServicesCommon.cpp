/*! \file
 *  \brief Implementation file of the common classes for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/ServicesCommon.h>

#include <System/Portage.h>

using namespace Omiscid;

const SimpleString CommonServiceValues::DefaultDomain = "_bip._tcp";

SimpleString CommonServiceValues::OmiscidServiceDnsSdType(""); // help for debug

namespace Omiscid {

// We need to initialise OmiscidServiceDnsSdType from environment variable
class OmiscidDnsSdTypeInitClass : CommonServiceValues
{
public:
	OmiscidDnsSdTypeInitClass()
	{
		char * Option = getenv( "OMISCID_WORKING_DOMAIN" );
		if ( Option == NULL || strcmp( Option, DefaultDomain.GetStr() ) == 0 )
		{
			OmiscidServiceDnsSdType = DefaultDomain;
			TraceError( "OMISCID_WORKING_DOMAIN not override. Use '%s'.\n", DefaultDomain.GetStr() );
			return;
		}

		// Copy the environment variable
		size_t size = strlen( Option );
		TemporaryMemoryBuffer tmpdomain(128);

		if ( size >= RegtypeLength )
		{
			OmiscidServiceDnsSdType = CommonServiceValues::DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN too long (%d max). Use '%s' instead.\n", RegtypeLength-1, DefaultDomain.GetStr() );
			return;
		}

		if ( sscanf( Option, "_bip_%[^.]._tcp", (char*)tmpdomain) != 1 )
		{
			OmiscidServiceDnsSdType = DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN do not look like '_bip_XXX._tcp'. Use '%s' instead.\n", DefaultDomain.GetStr() );
			return;
		}

		CommonServiceValues::OmiscidServiceDnsSdType = Option;

		TraceError( "OMISCID_WORKING_DOMAIN defined in environment variable. Use '%s'.\n", OmiscidServiceDnsSdType.GetStr() );
	};
};

static OmiscidDnsSdTypeInitClass OmiscidServiceDnsSdTypeInitClassInitialisationObject;

} // namespace Omiscid

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
};
