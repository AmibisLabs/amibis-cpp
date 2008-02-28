/**
 * @defgroup UserFriendly User friendly high-level API for Omiscid
 *
 */

#ifndef __SERVICE_REPOSITORY_H__
#define __SERVICE_REPOSITORY_H__

#include <ServiceControl/ConfigServiceControl.h>

#include <System/SimpleList.h>
#include <ServiceControl/ServiceProxy.h>
#include <ServiceControl/ServiceProxyList.h>
#include <ServiceControl/ServiceRepositoryListener.h>

namespace Omiscid {

class Factory;

/**
 * @class ServiceRepository ServiceControl/ServiceRepository.h
 * @ingroup UserFriendly
 * @ingroup ServiceControl
 * @brief High level user friendly class to browse Omiscid services.
 * @author Dominique Vaufreydaz
 */
class ServiceRepository
{
private:
	// Only the Factory class can create a ServiceRepository
	friend class Factory;

	// Constructor
	ServiceRepository();

public:
	// virtual destructor
	virtual ~ServiceRepository();

/**
 * Adds a listener to be notified of service apparitions and disapparitions.
 * Apparition notifications will only happen for all services.
 * Disapparition notifications will only happen for all services.
 *
 * From this addition, the listener will be notified of each service event (see {@link ServiceRepositoryListener}).
 * When a listener is added to the repository, this latter can already contain some services.
 * The boolean parameter tells whether the listener should be notified of these services as if they were appearing.
 *
 * @param Listener the listener that will be notified of service events
 * @param NotifyOnlyNewEvents a boolean used to ignore services running at the time of listener addition. Setting it to false (default value) will cause the listener to be immediately notified of services already present in the repository
 */
	bool AddListener(ServiceRepositoryListener* Listener, bool NotifyOnlyNewEvents = false );

/**
 * Adds a listener to be notified of service apparitions and disapparitions.
 * Apparition notifications will only happen for services matching given filter.
 * Disapparition notifications will only happen for services that have been notified of apparition.
 *
 * From this addition, the listener will be notified of each service event (see {@link ServiceRepositoryListener}).
 * When a listener is added to the repository, this latter can already contain some services.
 * The boolean parameter tells whether the listener should be notified of these services as if they were appearing.
 *
 * @param Listener the listener that will be notified of service events
 * @param Filter the filter to apply to service apparition/removal
 * @param NotifyOnlyNewEvents a boolean used to ignore services running at the time of listener addition. Setting it to false (default value) will cause the listener to be immediately notified of services already present in the repository
 */
	bool AddListener(ServiceRepositoryListener* Listener, ServiceFilter * Filter, bool NotifyOnlyNewEvents = false);


/**
 * Removes a listener from the notification process.
 * Disapparition notifications will only happen if NotifyAsIfExistingServicesDisappear is set to true.
 *
 * From this removal, the listener will no longer be notified of each service event (see {@link ServiceRepositoryListener}).
 * It can be reused later maybe with another filter.
 * The boolean parameter tells whether the listener should be notified of current running services as if they were disappearing.
 *
 * @param Listener the listener that will be notified of service events
 * @param NotifyAsIfExistingServicesDisappear a boolean used to asked to be notified as if services were disappearing (default value = false)
 */
	bool RemoveListener(ServiceRepositoryListener* Listener, bool NotifyAsIfExistingServicesDisappear = false );

	ServiceProxyList * GetAllServices();

	// To be compliant with java API
	void Stop();

private:
	MutexedSimpleList<ServiceRepositoryListener*> RepoListeners;	// All my listeners
};

} // namespace Omiscid

#endif  // __SERVICE_REPOSITORY_H__
