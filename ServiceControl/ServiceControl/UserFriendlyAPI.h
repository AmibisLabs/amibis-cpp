/* @file ServiceControl/ServiceControl/UserFriendlyAPI.h
 * @ingroup UserFriendly
 * @brief Files included when using user friendly API
 * @date 2004-2006
 */

#ifndef __USER_FRIENDLY_API_H__
#define __USER_FRIENDLY_API_H__

// To be compliant with usual OMiSCID include schema
#include <ServiceControl/ConfigServiceControl.h>

// System classes used in User Friendly API (layer 0)
// In this contexte, let's say that everything is used...
#include <System/AtomicCounter.h>
#include <System/AtomicReentrantCounter.h>
#include <System/AutoDelete.h>
#include <System/DateAndTime.h>
#include <System/ElapsedTime.h>
#include <System/Event.h>
#include <System/MultipleReferencedData.h>
#include <System/Mutex.h>
#include <System/Portage.h>
#include <System/RecycleSimpleList.h>
#include <System/ReentrantMutex.h>
#include <System/SimpleException.h>
#include <System/SimpleList.h>
#include <System/MutexedSimpleList.h>
#include <System/SimpleListException.h>
#include <System/SimpleString.h>
#include <System/Socket.h>
#include <System/SocketException.h>
#include <System/Thread.h>

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
#include <ServiceControl/ServiceRepository.h>
#include <ServiceControl/ServiceRepositoryListener.h>

#include <Messaging/StructuredMessage.h>
#include <Messaging/Serializable.h>

#endif // __USER_FRIENDLY_API_H__
