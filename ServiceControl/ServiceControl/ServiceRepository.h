/** 
 * @defgroup UserFriendly User friendly high-level API for Omiscid
 *
 */

#ifndef __SERVICE_REPOSITORY_H__
#define __SERVICE_REPOSITORY_H__

#include <ServiceControl/Config.h>

#include <System/Mutex.h>
#include <System/SimpleList.h>
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

private:
	SimpleList<ServiceRepositoryListener*> RepoListeners;	// All my listeners
};

} // namespace Omiscid

#endif  // __SERVICE_REPOSITORY_H__
