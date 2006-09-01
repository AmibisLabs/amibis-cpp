import os
from OmiscidScons import *
env = Environment()
OmiscidLinuxMacOSInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,['xml2'])

conf = Configure(env)
OmiscidCheckLibs(conf,['xml2','dns_sd','pthread']);
env = conf.Finish()

# common environment settings
env.AppendUnique(CXXFLAGS = ['-g','-Werror','-Wall','-pedantic','-std=c++98'])

binToInstall = []
libToInstall = []

env_system = env.Copy()
env_system.Append(CPPPATH='System')
target_system = env_system.SharedLibrary(
    target='OmiscidSystem',
    source=['System/AtomicCounter.cpp','System/AtomicReentrantCounter.cpp','System/Event.cpp','System/MultipleReferencedData.cpp','System/Mutex.cpp','System/Portage.cpp','System/RecycleSimpleList.cpp','System/ReentrantMutex.cpp','System/SimpleException.cpp','System/SimpleList.cpp','System/SimpleListException.cpp','System/SimpleString.cpp','System/Socket.cpp','System/SocketException.cpp','System/Thread.cpp']
)
libToInstall += target_system

env_com = env.Copy()
env_com.Append(CPPPATH=['System', 'Com'])
env_com.Append(LIBPATH=['.'])
env_com.Append(LIBS = ['OmiscidSystem'])
target_com = env_com.SharedLibrary(
    target='OmiscidCom',
    source=['Com/ComTools.cpp','Com/Config.cpp','Com/Message.cpp','Com/MsgManager.cpp','Com/MsgSocket.cpp','Com/MsgSocketException.cpp','Com/TcpClient.cpp','Com/TcpServer.cpp','Com/TcpUdpClientServer.cpp','Com/UdpExchange.cpp']
)

libToInstall += target_com

env_control = env.Copy()
env_control.Append(CPPPATH=['System', 'Com', 'ServiceControl'])
env_control.Append(LIBPATH=['.'])
env_control.Append(LIBS = ['OmiscidCom', 'OmiscidSystem'])
target_control = env_control.SharedLibrary(
    target='OmiscidControl',
    source=['ServiceControl/Attribut.cpp','ServiceControl/BrowseForDnsSdService.cpp','ServiceControl/ConnectorListener.cpp','ServiceControl/ControlClient.cpp','ServiceControl/ControlServer.cpp','ServiceControl/ControlUtils.cpp','ServiceControl/DnsSdProxy.cpp','ServiceControl/DnsSdService.cpp','ServiceControl/Factory.cpp','ServiceControl/InOutputAttribut.cpp','ServiceControl/IntVariableAttribut.cpp','ServiceControl/LocalVariableListener.cpp','ServiceControl/RemoteVariableChangeListener.cpp','ServiceControl/Service.cpp','ServiceControl/ServiceFilter.cpp','ServiceControl/ServiceFromXML.cpp','ServiceControl/ServiceProperties.cpp','ServiceControl/ServiceProxy.cpp','ServiceControl/ServiceProxyList.cpp','ServiceControl/ServicesCommon.cpp','ServiceControl/ServicesTools.cpp','ServiceControl/StringVariableAttribut.cpp','ServiceControl/VariableAttribut.cpp','ServiceControl/VariableAttributListener.cpp','ServiceControl/WaitForDnsSdServices.cpp','ServiceControl/XMLTreeParser.cpp','ServiceControl/XsdSchema.cpp','ServiceControl/XsdValidator.cpp']
)
libToInstall += target_control


env.Depends(target_com, target_system)
env.Depends(target_control, target_com)


hToInstall = []
hToInstall += [['System/System/AtomicCounter.h','System'],['System/System/AtomicReentrantCounter.h','System'],['System/System/Config.h','System'],['System/System/Event.h','System'],['System/System/MultipleReferencedData.h','System'],['System/System/Mutex.h','System'],['System/System/Portage.h','System'],['System/System/RecycleSimpleList.h','System'],['System/System/ReentrantMutex.h','System'],['System/System/SimpleException.h','System'],['System/System/SimpleList.h','System'],['System/System/SimpleListException.h','System'],['System/System/SimpleString.h','System'],['System/System/Socket.h','System'],['System/System/SocketException.h','System'],['System/System/Thread.h','System']]

hToInstall += [['Com/Com/ComTools.h','Com'],['Com/Com/Config.h','Com'],['Com/Com/Message.h','Com'],['Com/Com/MsgManager.h','Com'],['Com/Com/MsgSocket.h','Com'],['Com/Com/MsgSocketException.h','Com'],['Com/Com/TcpClient.h','Com'],['Com/Com/TcpServer.h','Com'],['Com/Com/TcpUdpClientServer.h','Com'],['Com/Com/UdpExchange.h','Com']]

hToInstall += [['ServiceControl/ServiceControl/Attribut.h','ServiceControl'],['ServiceControl/ServiceControl/BrowseForDnsSdService.h','ServiceControl'],['ServiceControl/ServiceControl/Config.h','ServiceControl'],['ServiceControl/ServiceControl/ConnectorListener.h','ServiceControl'],['ServiceControl/ServiceControl/ControlClient.h','ServiceControl'],['ServiceControl/ServiceControl/ControlServer.h','ServiceControl'],['ServiceControl/ServiceControl/ControlUtils.h','ServiceControl'],['ServiceControl/ServiceControl/DnsSdProxy.h','ServiceControl'],['ServiceControl/ServiceControl/DnsSdService.h','ServiceControl'],['ServiceControl/ServiceControl/Factory.h','ServiceControl'],['ServiceControl/ServiceControl/InOutputAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/IntVariableAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/LocalVariableListener.h','ServiceControl'],['ServiceControl/ServiceControl/RemoteVariableChangeListener.h','ServiceControl'],['ServiceControl/ServiceControl/Service.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceFilter.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceFromXML.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProperties.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProxy.h','ServiceControl'],['ServiceControl/ServiceControl/ServiceProxyList.h','ServiceControl'],['ServiceControl/ServiceControl/ServicesCommon.h','ServiceControl'],['ServiceControl/ServiceControl/ServicesTools.h','ServiceControl'],['ServiceControl/ServiceControl/StringVariableAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/VariableAttribut.h','ServiceControl'],['ServiceControl/ServiceControl/VariableAttributListener.h','ServiceControl'],['ServiceControl/ServiceControl/WaitForDnsSdServices.h','ServiceControl'],['ServiceControl/ServiceControl/XMLTreeParser.h','ServiceControl'],['ServiceControl/ServiceControl/XsdSchema.h','ServiceControl'],['ServiceControl/ServiceControl/XsdValidator.h','ServiceControl']]

binToInstall += OmiscidDotInFileTarget(env, 'Com/OmiscidCom-config', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, 'System/OmiscidSystem-config', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, 'ServiceControl/OmiscidControl-config', OmiscidMapping())

OmiscidInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

