

#ifndef __FACTORY_H__
#define __FACTORY_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/Service.h>

namespace Omiscid {

/** 
 * @defgroup UserFriendly User friendly API for Omiscid Service Control 
 *
 */

/**
 * @class Factory Factory.cpp ServiceControl/Factory.h
 * @ingroup UserFriendly
 * @ingroup ServiceControl
 * @brief High level user friendly class to create Omiscid services.
 *
 * This class is used to create a service. One have to invoque
 * static function Factory#Create(const SimpleString ServiceName) of
 * Factory#CreateFromXML(SimpleString XmlDesc) directly using
 * "Factory::Create..." or "ServiceFactory.Create..." syntax.
 * 
 * @author Dominique Vaufreydaz
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

#endif  // __FACTORY_H__
