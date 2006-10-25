/* @file ServiceControl/ServiceControl/UserFriendlyAPI.h
 * @ingroup UserFriendly
 * @brief Files included when using user friendly API
 * @date 2004-2006
 */

#ifndef __USER_FRIENDLY_API_H__
#define __USER_FRIENDLY_API_H__

// System classes used in User Friendly API (layer 0)
#include <System/SimpleList.h>
#include <System/SimpleString.h>

// Com classes used in User Friendly API (layer 1)
#include <Com/Message.h>

// ServiceControl classes used in User Friendly API (layer 2)
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/LocalVariableListener.h>
#include <ServiceControl/RemoteVariableChangeListener.h>
#include <ServiceControl/Service.h>
#include <ServiceControl/ServiceFilter.h>
#include <ServiceControl/ServiceProxy.h>
#include <ServiceControl/ServiceProxyList.h>

#endif // __USER_FRIENDLY_API_H__
