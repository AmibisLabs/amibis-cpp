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

namespace Omiscid {

/**
 * @class ServiceRepositoryListener ServiceControl/ServiceRepositoryListener.h
 * @ingroup UserFriendly
 * @ingroup ServiceControl
 * @brief High level user friendly class to collect browse Omiscid services.
 * @author Dominique Vaufreydaz
 */
class ServiceRepositoryListener : protected DnsSdProxyClient
{
public:
	// Constructor
	ServiceRepositoryListener();

	// virtual destructor
	virtual ~ServiceRepositoryListener();



private:
	SimpleList<ServiceRepositoryListener*> RepoListeners;	// All my listeners

protected:
	virtual void FUNCTION_CALL_TYPE DnsSdProxyServiceBrowseReply( unsigned int flags, const DnsSdService& Service );
};

} // namespace Omiscid

#endif  // __SERVICE_REPOSITORY_LISTENER_H__
