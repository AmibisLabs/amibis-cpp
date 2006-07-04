

#include <ServiceControl/OmiscidServiceRegistry.h>

using namespace Omiscid;

OmiscidServiceRegistry ServiceFactory;

/** 
	* Registers a new Omiscid service. This service will be advertised in DSN-SD
	* @param serviceName the name of the service as it will appear in DNS-SD
	* @return the bip service. All access to the service will be through this object
	*/
OmiscidService * OmiscidServiceRegistry::Create( const SimpleString ServiceName )
{
	OmiscidService * NewOmiscidService = new OmiscidService(ServiceName);

	return NewOmiscidService;
}

/** 
	* Registers a new Omiscid service. This service will be advertised in DSN-SD
	* @param stream the input stream of the xml service description
	* @return the bip service. All access to the service will be through this object
	* -------------------            Not implemented yet         -----------
	*/
OmiscidService * OmiscidServiceRegistry::CreateFromXML( SimpleString XmlDesc )
{
	return NULL;
}
