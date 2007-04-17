/** 
 * @defgroup UserFriendly User friendly high-level API for Omiscid
 *
 */

#ifndef __SERVICE_REPOSITORY_H__
#define __SERVICE_REPOSITORY_H__

#include <ServiceControl/Config.h>

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

	// defaults to false
	bool AddListener(ServiceRepositoryListener* Listener, bool NotifyOnlyNewEvents = false );
	bool RemoveListener(ServiceRepositoryListener* Listener, bool NotifyAsIfExistingServicesDisappear = false );

	ServiceProxyList * GetAllServices();

	// To be compliant with java API
	void Stop();

private:
	MutexedSimpleList<ServiceRepositoryListener*> RepoListeners;	// All my listeners
};

} // namespace Omiscid

#endif  // __SERVICE_REPOSITORY_H__
