/*! \file
 *  \brief Implementation file of the common classes for the PRIMA Service package
 *  \author Dominique Vaufreydaz
 *  \author Special thanks to Sébastien Pesnel for debugging and testing
 *  \author Special thanks to Julien Letessier for his kind help about DNS-SD
 *  \version 1.0
 *  \date    2004-2005
 */

#include <ServiceControl/ServicesCommon.h>


const char * CommonServiceValues::DefaultDomain = "_bip_dev._tcp";

char * CommonServiceValues::OmiscidServiceDnsSdType = (char*)NULL; // help for debug

// We need to initialise OmiscidServiceDnsSdType from environment variable
class OmiscidServiceDnsSdTypeInitClass
{
public:
	OmiscidServiceDnsSdTypeInitClass()
	{
		char * Option = getenv( "OMISCID_WORKING_DOMAIN" );
		if ( Option == NULL )
		{
			CommonServiceValues::OmiscidServiceDnsSdType = (char*)CommonServiceValues::DefaultDomain;
			#ifdef _DEBUG
				fprintf( stderr, "OMISCID_WORKING_DOMAIN not defined. Use '%s'.\n", CommonServiceValues::DefaultDomain );
			#endif
			return;
		}

		// Copy the environment variable
		size_t size = strlen( Option );
		char tmpdomain[128];

		if ( size >= 50 )
		{
			CommonServiceValues::OmiscidServiceDnsSdType = (char*)CommonServiceValues::DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN too long. Use '%s' instead.\n", CommonServiceValues::DefaultDomain );
			return;
		}

		if ( sscanf( Option, "_bip_%[^.]._tcp", tmpdomain) != 1 )
		{
			CommonServiceValues::OmiscidServiceDnsSdType = (char*)CommonServiceValues::DefaultDomain;
			fprintf( stderr, "OMISCID_WORKING_DOMAIN do not look like '_bip_XXX._tcp'. Use '%s' instead.\n", CommonServiceValues::DefaultDomain );
			return;
		}

		CommonServiceValues::OmiscidServiceDnsSdType = new char[size+1];

		strcpy( CommonServiceValues::OmiscidServiceDnsSdType, Option );
	};

	~OmiscidServiceDnsSdTypeInitClass()
	{
		if ( CommonServiceValues::OmiscidServiceDnsSdType != (char*)CommonServiceValues::DefaultDomain )
		{
			delete CommonServiceValues::OmiscidServiceDnsSdType;
			CommonServiceValues::OmiscidServiceDnsSdType = NULL;
		}
	}
};

static OmiscidServiceDnsSdTypeInitClass OmiscidServiceDnsSdTypeInitClassInitialisationObject;

ServiceException::ServiceException( const char * Message )
  : SimpleException(Message)
{}

ServiceException::ServiceException(const ServiceException& ExceptionToCopy)
  : SimpleException(ExceptionToCopy)
{}

ServiceException::~ServiceException( )
{}

