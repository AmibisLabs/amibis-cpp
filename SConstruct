import os
from primaScons import *
env = Environment()
primaInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,['xml2'])

conf = Configure(env)
primaCheckLibs(conf,['xml2','dns_sd','pthread']);
env = conf.Finish()

# common environment settings
env.AppendUnique(CXXFLAGS = ['-g'])

binToInstall = []
libToInstall = []

env_system = env.Copy()
env_system.Append(CPPPATH='System')
target_system = env_system.SharedLibrary(
    target='primasystem',
    source=['System/AtomicCounter.cpp','System/Event.cpp','System/MultipleReferencedData.cpp','System/Mutex.cpp','System/MutexedCounter.cpp','System/Portage.cpp','System/RecycleSimpleList.cpp','System/SimpleException.cpp','System/SimpleList.cpp','System/SimpleListException.cpp','System/SimpleString.cpp','System/Socket.cpp','System/SocketException.cpp','System/Thread.cpp']
)
libToInstall += target_system

env_com = env.Copy()
env_com.Append(CPPPATH=['System', 'Com'])
env_com.Append(LIBPATH=['.'])
env_com.Append(LIBS = ['primasystem'])
target_com = env_com.SharedLibrary(
    target='com',
    source=['Com/ComTools.cpp','Com/Message.cpp','Com/MsgManager.cpp','Com/MsgSocket.cpp','Com/MsgSocketException.cpp','Com/TcpClient.cpp','Com/TcpServer.cpp','Com/TcpUdpClientServer.cpp','Com/UdpExchange.cpp']
)

libToInstall += target_com

env_control = env.Copy()
env_control.Append(CPPPATH=['System', 'Com', 'ServiceControl'])
env_control.Append(LIBPATH=['.'])
env_control.Append(LIBS = ['com', 'primasystem'])
target_control = env_control.SharedLibrary(
    target='control',
    source=['ServiceControl/Attribut.cpp','ServiceControl/BrowseForService.cpp','ServiceControl/ControlClient.cpp','ServiceControl/ControlServer.cpp','ServiceControl/ControlUtils.cpp','ServiceControl/InOutputAttribut.cpp','ServiceControl/IntVariableAttribut.cpp','ServiceControl/OmiscidService.cpp','ServiceControl/OmiscidServiceFilters.cpp','ServiceControl/OmiscidServiceProxy.cpp','ServiceControl/OmiscidServiceRegistry.cpp','ServiceControl/OmiscidServicesTools.cpp','ServiceControl/Service.cpp','ServiceControl/ServiceFromXML.cpp','ServiceControl/ServiceProperties.cpp','ServiceControl/ServicesCommon.cpp','ServiceControl/VariableAttribut.cpp','ServiceControl/WaitForServices.cpp','ServiceControl/XMLTreeParser.cpp']
)
libToInstall += target_control


env.Depends(target_com, target_system)
env.Depends(target_control, target_com)


hToInstall = []
hToInstall += [['System/System/AtomicCounter.h','System'],['System/System/Event.h','System'],['System/System/MultipleReferencedData.h','System'],['System/System/Mutex.h','System'],['System/System/MutexedCounter.h','System'],['System/System/Portage.h','System'],['System/System/RecycleSimpleList.h','System'],['System/System/SimpleException.h','System'],['System/System/SimpleList.h','System'],['System/System/SimpleListException.h','System'],['System/System/SimpleString.h','System'],['System/System/Socket.h','System'],['System/System/SocketException.h','System'],['System/System/Thread.h','System']]

hToInstall += [['Com/Com/ComTools.h','Com'],['Com/Com/Message.h','Com'],['Com/Com/MsgManager.h','Com'],['Com/Com/MsgSocket.h','Com'],['Com/Com/MsgSocketException.h','Com'],['Com/Com/TcpClient.h','Com'],['Com/Com/TcpServer.h','Com'],['Com/Com/TcpUdpClientServer.h','Com'],['Com/Com/UdpExchange.h','Com']]

hToInstall += [['ServiceControl/ServiceControl/Attribut.h','ServiceControl'],['ServiceControl/ServiceControl/BrowseForService.h','ServiceControl'],['ServiceControl/ServiceControl/ControlClient.h','ServiceControl'],['ServiceControl/ServiceControl/ControlServer.h','ServiceControl'],['ServiceControl/ServiceControl/ControlUtils.h','ServiceControl'],['ServiceControl/ServiceControl/InOutputAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/IntVariableAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/OmiscidService.h','ServiceControl'],['ServiceControl/ServiceControl/OmiscidServiceFilters.h','ServiceControl'],['ServiceControl/ServiceControl/OmiscidServiceProxy.h','ServiceControl'],['ServiceControl/ServiceControl/OmiscidServiceRegistry.h','ServiceControl'],['ServiceControl/ServiceControl/OmiscidServicesTools.h','ServiceControl'],['ServiceControl/ServiceControl/Service.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceFromXML.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProperties.h','ServiceControl'],['ServiceControl/ServiceControl/ServicesCommon.h','ServiceControl'],['ServiceControl/ServiceControl/VariableAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/WaitForServices.h','ServiceControl'],['ServiceControl/ServiceControl/XMLTreeParser.h','ServiceControl']]

binToInstall += primaDotInFileTarget(env, 'Com/primacom-config', primaMapping())
binToInstall += primaDotInFileTarget(env, 'System/primasystem-config', primaMapping())
binToInstall += primaDotInFileTarget(env, 'ServiceControl/primacontrol-config', primaMapping())

primaInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

