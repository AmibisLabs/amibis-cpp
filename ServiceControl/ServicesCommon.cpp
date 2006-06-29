/*! \file
 *  \brief Implementation file of the common classes for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/ServicesCommon.h>

using namespace Omiscid;

const char * CommonServiceValues::DefaultDomain = "_bip._tcp";

char * CommonServiceValues::OmiscidServiceDnsSdType = (char*)NULL; // help for debug

namespace Omiscid {

// We need to initialise OmiscidServiceDnsSdType from environment variable
class OmiscidDnsSdTypeInitClass : CommonServiceValues
{
public:
	OmiscidDnsSdTypeInitClass()
	{
		char * Option = getenv( "OMISCID_WORKING_DOMAIN" );
		if ( Option == NULL || strcmp( Option, (char*)DefaultDomain ) == 0 )
		{
			OmiscidServiceDnsSdType = (char*)DefaultDomain;
			#ifdef DEBUG
				fprintf( stderr, "OMISCID_WORKING_DOMAIN not override. Use '%s'.\n", DefaultDomain );
			#endif
			return;
		}

		// Copy the environment variable
		size_t size = strlen( Option );
		char tmpdomain[128];

		if ( size >= RegtypeLength )
		{
			OmiscidServiceDnsSdType = (char*)CommonServiceValues::DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN too long (%d max). Use '%s' instead.\n", RegtypeLength-1, DefaultDomain );
			return;
		}

		if ( sscanf( Option, "_bip_%[^.]._tcp", tmpdomain) != 1 )
		{
			OmiscidServiceDnsSdType = (char*)DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN do not look like '_bip_XXX._tcp'. Use '%s' instead.\n", DefaultDomain );
			return;
		}

		OmiscidServiceDnsSdType = new char[size+1];
		strcpy( CommonServiceValues::OmiscidServiceDnsSdType, Option );

		#ifdef DEBUG
			fprintf( stderr, "OMISCID_WORKING_DOMAIN defined in environment variable. Use '%s'.\n", OmiscidServiceDnsSdType );
		#endif
	};

	~OmiscidDnsSdTypeInitClass()
	{
		if ( CommonServiceValues::OmiscidServiceDnsSdType != (char*)CommonServiceValues::DefaultDomain )
		{
			delete CommonServiceValues::OmiscidServiceDnsSdType;
			CommonServiceValues::OmiscidServiceDnsSdType = NULL;
		}
	}
};

static OmiscidDnsSdTypeInitClass OmiscidServiceDnsSdTypeInitClassInitialisationObject;

} // namespace Omiscid

ServiceException::ServiceException( const char * Message )
  : SimpleException(Message)
{}

ServiceException::ServiceException(const ServiceException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{
}

ServiceException::~ServiceException( )
{
}
