/** 
 * @defgroup UserFriendly User friendly high-level API for Omiscid
 *
 */

#ifndef __SERVICE_REPOSITORY_LISTENER_H__
#define __SERVICE_REPOSITORY_LISTENER_H__

#include <ServiceControl/Config.h>

#include <System/Mutex.h>
#include <System/SimpleList.h>
#include <ServiceControl/DnsSdProxy.h>
#include <ServiceControl/ServiceProxy.h>

namespace Omiscid {

class ServiceRepository;

/**
 * @class ServiceRepositoryListener ServiceControl/ServiceRepositoryListener.h
 * @ingroup UserFriendly
 * @ingroup ServiceControl
 * @brief High level user friendly class to collect browse Omiscid services.
 * @author Dominique Vaufreydaz
 */
class ServiceRepositoryListener : private DnsSdProxyClient
{
	// Friend class ServiceRepository can access private members from DnsSdProxyClient
	friend class ServiceRepository;

public:
	// Constructor
	ServiceRepositoryListener();

	// virtual destructor
	virtual ~ServiceRepositoryListener();

	virtual void ServiceAdded(const ServiceProxy& ProxyForService   ) = 0;
	virtual void ServiceRemoved(const ServiceProxy& ProxyForService ) = 0;

protected:
	virtual void FUNCTION_CALL_TYPE DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& ServiceInfo );
};

} // namespace Omiscid

#endif  // __SERVICE_REPOSITORY_LISTENER_H__
