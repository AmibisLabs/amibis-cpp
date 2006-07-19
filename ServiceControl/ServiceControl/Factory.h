/** 
 * @defgroup UserFriendly User friendly high-level API for Omiscid
 *
 */

#ifndef __FACTORY_H__
#define __FACTORY_H__

#include <ServiceControl/Config.h>

#include <System/SimpleString.h>
#include <ServiceControl/Service.h>

namespace Omiscid {

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
 * Factory#CreateFromXML is not accessible yet (under work).
 * 
 * @author Dominique Vaufreydaz
 */
class Factory
{
public:
	/** 
	 * Create a new Omiscid service. This service will not be advertised in DSN-SD
	 * until the Service#Start method is called.
	 * @param ServiceName the name of the service as it will appear in DNS-SD (maybe
	 * suffixed by a number automatically added).
	 * @param ClassName The classe name of this service. Can be usefull in order
	 * to search for services. By default, class is Service.
	 * @return the Omiscid service. All access to the service will be through this object
	 */
	static Service * Create(const SimpleString ServiceName, const SimpleString ClassName = Service::DefaultServiceClassName );
	
	/** 
	 * Create a new Omiscid service. This service will not be advertised in DSN-SD
	 * until the Service#Start method is called.
	 * @param XmlDesc the input stream of the full xml service description
	 * @return the Omiscid service. All access to the service will be through this object
	 */
	static Service * CreateFromXML(SimpleString XmlDesc);

};

/** 
 * An global object in order to invoque Factory# method.
 */
extern Factory ServiceFactory;

} // namespace Omiscid

#endif  // __FACTORY_H__
