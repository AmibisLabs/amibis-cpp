/* @file ServiceControl/ServiceControl//UserFriendlyAPI.h
 * @ingroup UserFriendly

 * @brief Files included when using user friendly API
 * @date 2004-2011
 * @author Dominique Vaufreydaz
 */

#ifndef __USER_FRIENDLY_API_H__
#define __USER_FRIENDLY_API_H__

// System subfolder
#include <System/AtomicCounter.h>
#include <System/AtomicReentrantCounter.h>
#include <System/AutoDelete.h>
#include <System/DateAndTime.h>
#include <System/ElapsedTime.h>
#include <System/Event.h>
#include <System/LockManagement.h>
#include <System/MultipleReferencedData.h>
#include <System/Mutex.h>
#include <System/MutexedSimpleList.h>
#include <System/Portage.h>
#include <System/RecycleSimpleList.h>
#include <System/ReentrantMutex.h>
#include <System/SharedMemSegment.h>
#include <System/SimpleException.h>
#include <System/SimpleList.h>
#include <System/SimpleListException.h>
#include <System/SimpleString.h>
#include <System/Socket.h>
#include <System/SocketException.h>
#include <System/TemporaryMemoryBuffer.h>
#include <System/Thread.h>
#include <System/TrackingMemoryLeaks.h>

// Com subfolder
#include <Com/Message.h>

// ServiceControl subfolder
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

// Messaging subfolder
#include <Messaging/Serializable.h>
#include <Messaging/SerializeException.h>
#include <Messaging/SerializeManager.h>
#include <Messaging/SerializeValue.h>
#include <Messaging/StructuredMessage.h>


#endif // __USER_FRIENDLY_API_H__


