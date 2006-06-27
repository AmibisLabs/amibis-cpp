

#ifndef __OMISCID_SERVICE_REGISTRY_H__
#define __OMISCID_SERVICE_REGISTRY_H__

#include <System/Config.h>
#include <System/SimpleString.h>
#include <ServiceControl/OmiscidService.h>

namespace Omiscid {

/**
 * @author 
 *
 */
class OmiscidServiceRegistry
{
public:
	/** 
	 * Registers a new Omiscid service. This service will be advertised in DSN-SD
	 * @param serviceName the name of the service as it will appear in DNS-SD
	 * @return the bip service. All access to the service will be through this object
	 */
	static OmiscidService * Register(const SimpleString& ServiceName);
	
	/** 
	 * Registers a new Bip service. This service will be advertised in DSN-SD
	 * @param stream the input stream of the xml service description
	 * @return the bip service. All access to the service will be through this object
	 */
	static OmiscidService * RegisterFromXML(SimpleString XmlDesc);	

};

} // namespace Omiscid

#endif  // __OMISCID_SERVICE_REGISTRY_H__
