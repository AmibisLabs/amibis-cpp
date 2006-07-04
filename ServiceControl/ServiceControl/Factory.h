

#ifndef __OMISCID_SERVICE_REGISTRY_H__
#define __OMISCID_SERVICE_REGISTRY_H__

#include <System/Config.h>
#include <System/SimpleString.h>
#include <ServiceControl/Service.h>

namespace Omiscid {

/**
 * @author 
 *
 */
class Factory
{
public:
	/** 
	 * Registers a new Omiscid service. This service will be advertised in DSN-SD
	 * @param serviceName the name of the service as it will appear in DNS-SD
	 * @return the bip service. All access to the service will be through this object
	 */
	static Service * Create(const SimpleString ServiceName);
	
	/** 
	 * Registers a new Bip service. This service will be advertised in DSN-SD
	 * @param stream the input stream of the xml service description
	 * @return the bip service. All access to the service will be through this object
	 */
	static Service * CreateFromXML(SimpleString XmlDesc);

};

extern Factory ServiceFactory;

} // namespace Omiscid

#endif  // __OMISCID_SERVICE_REGISTRY_H__
