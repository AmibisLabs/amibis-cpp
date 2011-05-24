/**
 * @defgroup UserFriendly User friendly high-level API for Omiscid
 *
 */

#ifndef __SERVICE_REPOSITORY_LISTENER_H__
#define __SERVICE_REPOSITORY_LISTENER_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/Mutex.h>
#include <System/SimpleList.h>
#include <ServiceControl/DnsSdProxy.h>
#include <ServiceControl/ServiceProxy.h>
#include <ServiceControl/ServiceFilter.h>

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

	virtual void ServiceAdded( ServiceProxy& ProxyForService   ) = 0;
	virtual void ServiceRemoved( ServiceProxy& ProxyForService ) = 0;

private:
	// function used to set/unset filter on this listener. Used only by ServiceRepository
	bool SetFilter( ServiceFilter * Filter );
	bool UnsetFilter();

protected:
	// Callback to receive notification.
	void FUNCTION_CALL_TYPE DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& ServiceInfo );

	ServiceFilter * FilterForMonitoring;
	SimpleList<unsigned int> MonitoredServicesForRemovals; // Used to check is service has to be monitored for removals
};

} // namespace Omiscid

#endif  // __SERVICE_REPOSITORY_LISTENER_H__

