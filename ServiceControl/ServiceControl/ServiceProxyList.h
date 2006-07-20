/* @file
 * @ingroup ServiceControl
 * @ingroup UserFriendly
 * @brief Defines ServiceProxyList class in order to get list of other services
 * @author Dominique Vaufreydaz
 * @version 1.0
 * @date    2004-2006
 */

#ifndef __SERVICE_PROXY_LIST_H__
#define __SERVICE_PROXY_LIST_H__

#include <ServiceControl/Config.h>

#include <System/SimpleList.h>
#include <ServiceControl/ServiceProxy.h>

namespace Omiscid {

class RemoteVariableChangeListener;
class Service;

/**
 * @author 
 *
 */
class ServiceProxyList  : public SimpleList<ServiceProxy*>
{
public:
	// Destructor
	virtual ~ServiceProxyList();

	virtual void Empty();
};

} // namespace Omiscid

#endif  // __SERVICE_PROXY_LIST_H__
