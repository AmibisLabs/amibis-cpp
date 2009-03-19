# Tables of layers files

import os

# Layer System
SystemHeaders=[['System/System/AtomicCounter.h','System'],['System/System/AtomicReentrantCounter.h','System'],['System/System/AutoDelete.h','System'],['System/System/ConfigSystem.h','System'],['System/System/DateAndTime.h','System'],['System/System/ElapsedTime.h','System'],['System/System/Event.h','System'],['System/System/LockManagement.h','System'],['System/System/MultipleReferencedData.h','System'],['System/System/Mutex.h','System'],['System/System/MutexedSimpleList.h','System'],['System/System/Portage.h','System'],['System/System/RecycleSimpleList.h','System'],['System/System/ReentrantMutex.h','System'],['System/System/SharedMemSegment.h','System'],['System/System/SimpleException.h','System'],['System/System/SimpleList.h','System'],['System/System/SimpleListException.h','System'],['System/System/SimpleString.h','System'],['System/System/Socket.h','System'],['System/System/SocketException.h','System'],['System/System/TemporaryMemoryBuffer.h','System'],['System/System/Thread.h','System'],['System/System/TrackingMemoryLeaks.h','System']]
SystemSources=['System/AtomicCounter.cpp','System/AtomicReentrantCounter.cpp','System/ConfigSystem.cpp','System/DateAndTime.cpp','System/ElapsedTime.cpp','System/Event.cpp','System/LockManagement.cpp','System/MultipleReferencedData.cpp','System/Mutex.cpp','System/MutexedSimpleList.cpp','System/Portage.cpp','System/RecycleSimpleList.cpp','System/ReentrantMutex.cpp','System/SharedMemSegment.cpp','System/SimpleException.cpp','System/SimpleList.cpp','System/SimpleListException.cpp','System/SimpleString.cpp','System/Socket.cpp','System/SocketException.cpp','System/TemporaryMemoryBuffer.cpp','System/Thread.cpp','System/TrackingMemoryLeaks.cpp']

# Layer Com
ComHeaders=[['Com/Com/ComTools.h','Com'],['Com/Com/ConfigCom.h','Com'],['Com/Com/Connector.h','Com'],['Com/Com/Message.h','Com'],['Com/Com/MsgManager.h','Com'],['Com/Com/MsgSocket.h','Com'],['Com/Com/MsgSocketException.h','Com'],['Com/Com/TcpClient.h','Com'],['Com/Com/TcpServer.h','Com'],['Com/Com/UdpExchange.h','Com']]
ComSources=['Com/ComTools.cpp','Com/ConfigCom.cpp','Com/Connector.cpp','Com/Message.cpp','Com/MsgManager.cpp','Com/MsgSocket.cpp','Com/MsgSocketException.cpp','Com/TcpClient.cpp','Com/TcpServer.cpp','Com/UdpExchange.cpp']

# Layer ServiceControl
ServiceControlHeaders=[['ServiceControl/ServiceControl/Attribute.h','ServiceControl'],['ServiceControl/ServiceControl/BrowseForDnsSdService.h','ServiceControl'],['ServiceControl/ServiceControl/ConfigServiceControl.h','ServiceControl'],['ServiceControl/ServiceControl/ConnectorListener.h','ServiceControl'],['ServiceControl/ServiceControl/ControlClient.h','ServiceControl'],['ServiceControl/ServiceControl/ControlServer.h','ServiceControl'],['ServiceControl/ServiceControl/ControlUtils.h','ServiceControl'],['ServiceControl/ServiceControl/DnsSdProxy.h','ServiceControl'],['ServiceControl/ServiceControl/DnsSdService.h','ServiceControl'],['ServiceControl/ServiceControl/Factory.h','ServiceControl'],['ServiceControl/ServiceControl/InOutputAttribute.h','ServiceControl'],['ServiceControl/ServiceControl/IntVariableAttribute.h','ServiceControl'],['ServiceControl/ServiceControl/LocalVariableListener.h','ServiceControl'],['ServiceControl/ServiceControl/RemoteVariableChangeListener.h','ServiceControl'],['ServiceControl/ServiceControl/Service.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceFilter.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceFromXML.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProperties.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProxy.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProxyList.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceRepository.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceRepositoryListener.h','ServiceControl'],['ServiceControl/ServiceControl/ServicesCommon.h','ServiceControl'],['ServiceControl/ServiceControl/ServicesTools.h','ServiceControl'],['ServiceControl/ServiceControl/StringVariableAttribute.h','ServiceControl'],['ServiceControl/ServiceControl/UserFriendlyAPI.h','ServiceControl'],['ServiceControl/ServiceControl/VariableAttribute.h','ServiceControl'],['ServiceControl/ServiceControl/VariableAttributeListener.h','ServiceControl'],['ServiceControl/ServiceControl/WaitForDnsSdServices.h','ServiceControl'],['ServiceControl/ServiceControl/XMLTreeParser.h','ServiceControl'],['ServiceControl/ServiceControl/XsdSchema.h','ServiceControl'],['ServiceControl/ServiceControl/XsdValidator.h','ServiceControl']]
ServiceControlSources=['ServiceControl/Attribute.cpp','ServiceControl/BrowseForDnsSdService.cpp','ServiceControl/ConfigServiceControl.cpp','ServiceControl/ConnectorListener.cpp','ServiceControl/ControlClient.cpp','ServiceControl/ControlServer.cpp','ServiceControl/ControlUtils.cpp','ServiceControl/DnsSdProxy.cpp','ServiceControl/DnsSdService.cpp','ServiceControl/Factory.cpp','ServiceControl/InOutputAttribute.cpp','ServiceControl/IntVariableAttribute.cpp','ServiceControl/LocalVariableListener.cpp','ServiceControl/RemoteVariableChangeListener.cpp','ServiceControl/Service.cpp','ServiceControl/ServiceFilter.cpp','ServiceControl/ServiceFromXML.cpp','ServiceControl/ServiceProperties.cpp','ServiceControl/ServiceProxy.cpp','ServiceControl/ServiceProxyList.cpp','ServiceControl/ServiceRepository.cpp','ServiceControl/ServiceRepositoryListener.cpp','ServiceControl/ServicesCommon.cpp','ServiceControl/ServicesTools.cpp','ServiceControl/StringVariableAttribute.cpp','ServiceControl/VariableAttribute.cpp','ServiceControl/VariableAttributeListener.cpp','ServiceControl/WaitForDnsSdServices.cpp','ServiceControl/XMLTreeParser.cpp','ServiceControl/XsdSchema.cpp','ServiceControl/XsdValidator.cpp']

