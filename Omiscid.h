/* @file Omiscid.h
 * @ingroup UserFriendly

 * @brief Files included when using user friendly API
 * @date 2004-2011
 * @author Dominique Vaufreydaz
 */

#ifndef __OMISCID_H__
#define __OMISCID_H__

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
#include <Com/ComTools.h>
#include <Com/Connector.h>
#include <Com/Message.h>
#include <Com/MsgManager.h>
#include <Com/MsgSocket.h>
#include <Com/MsgSocketException.h>
#include <Com/TcpClient.h>
#include <Com/TcpServer.h>
#include <Com/UdpExchange.h>

// ServiceControl subfolder
#include <ServiceControl/Attribute.h>
#include <ServiceControl/BrowseForDnsSdService.h>
#include <ServiceControl/ConnectorListener.h>
#include <ServiceControl/ControlClient.h>
#include <ServiceControl/ControlServer.h>
#include <ServiceControl/ControlUtils.h>
#include <ServiceControl/DnsSdProxy.h>
#include <ServiceControl/DnsSdService.h>
#include <ServiceControl/Factory.h>
#include <ServiceControl/InOutputAttribute.h>
#include <ServiceControl/IntVariableAttribute.h>
#include <ServiceControl/LocalVariableListener.h>
#include <ServiceControl/RemoteVariableChangeListener.h>
#include <ServiceControl/Service.h>
#include <ServiceControl/ServiceFilter.h>
#include <ServiceControl/ServiceFromXML.h>
#include <ServiceControl/ServiceProperties.h>
#include <ServiceControl/ServiceProxy.h>
#include <ServiceControl/ServiceProxyList.h>
#include <ServiceControl/ServiceRepository.h>
#include <ServiceControl/ServiceRepositoryListener.h>
#include <ServiceControl/ServicesCommon.h>
#include <ServiceControl/ServicesTools.h>
#include <ServiceControl/StringVariableAttribute.h>
#include <ServiceControl/VariableAttribute.h>
#include <ServiceControl/VariableAttributeListener.h>
#include <ServiceControl/WaitForDnsSdServices.h>
#include <ServiceControl/XMLTreeParser.h>
#include <ServiceControl/XsdSchema.h>
#include <ServiceControl/XsdValidator.h>

// Messaging subfolder
#include <Messaging/Serializable.h>
#include <Messaging/SerializeException.h>
#include <Messaging/SerializeManager.h>
#include <Messaging/SerializeValue.h>
#include <Messaging/StructuredMessage.h>


#endif // __OMISCID_H__


