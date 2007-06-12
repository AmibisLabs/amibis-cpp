

#include <ServiceControl/Factory.h>

using namespace Omiscid;

namespace Omiscid {

Factory ServiceFactory;

} // namespace Omiscid

    /**
     * Create a new Omiscid service. This service will not be advertised in DSN-SD
     * until the Service#Start method is called.
     * @param ServiceName the name of the service as it will appear in DNS-SD (maybe
     * suffixed by a number automatically added).
     * @param ClassName The classe name of this service. Can be usefull in order
     * to search for services. By default, class is 'Service'.
     * @return the Omiscid service. All access to the service will be through this object
     */
Service * Factory::Create( const SimpleString ServiceName, const SimpleString ClassName )
{
    if ( ServiceName.IsEmpty() )
    {
        return NULL;
    }

    // construct the service
    Service * NewOmiscidService = new OMISCID_TLM Service(ServiceName, ClassName);

    // Return the service
    return NewOmiscidService;
}

    /**
     * Create a new Omiscid service. This service will not be advertised in DSN-SD
     * until the Service#Start method is called.
     * @param XmlDesc the input stream of the full xml service description
     * @return the Omiscid service. All access to the service will be through this object
     */
Service * Factory::CreateFromXML( SimpleString XmlDesc )
{
    return NULL;
}


    /**
     * Create a new Omiscid Service Repository. This repository is used to browse
     * add/remove of Omiscid services. You will need to add listeners to this object
     * using classes inherited from #ServiceRepositoryListener.
     * @return the #ServiceRepository.
     */
ServiceRepository * Factory::CreateServiceRepository()
{
    // For the moment, nothing actually difficult
    return new ServiceRepository();
}
