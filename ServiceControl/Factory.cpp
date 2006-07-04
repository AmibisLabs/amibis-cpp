

#include <ServiceControl/Factory.h>

using namespace Omiscid;

Factory ServiceFactory;

/** 
	* Registers a new Omiscid service. This service will be advertised in DSN-SD
	* @param serviceName the name of the service as it will appear in DNS-SD
	* @return the bip service. All access to the service will be through this object
	*/
OmiscidService * Factory::Create( const SimpleString ServiceName )
{
	OmiscidService * NewOmiscidService = new Service(ServiceName);

	return NewOmiscidService;
}

/** 
	* Registers a new Omiscid service. This service will be advertised in DSN-SD
	* @param stream the input stream of the xml service description
	* @return the bip service. All access to the service will be through this object
	* -------------------            Not implemented yet         -----------
	*/
OmiscidService * Factory::CreateFromXML( SimpleString XmlDesc )
{
	return NULL;
}
