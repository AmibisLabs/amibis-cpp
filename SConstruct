
import os
from primaScons import *
env = Environment()
primaInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,["xml2"])

conf = Configure(env)
primaCheckLibs(conf,["xml2","dns_sd","pthread"]);
env = conf.Finish()


binToInstall = []
libToInstall = []

env.Append(CPPPATH="System")
libToInstall += env.SharedLibrary(
    target="primasystem",
    source=["System/SimpleString.cpp","System/AtomicCounter.cpp","System/Event.cpp","System/MultipleReferencedData.cpp","System/Mutex.cpp","System/Portage.cpp","System/RecycleSimpleList.cpp","System/SimpleException.cpp","System/SimpleList.cpp","System/SimpleListException.cpp","System/Socket.cpp","System/SocketException.cpp","System/Thread.cpp"],
    libs=["pthread"]
)

env.Append(CPPPATH="Com")
libToInstall += env.SharedLibrary(
    target="com",
    source=["Com/ComTools.cpp","Com/Message.cpp","Com/MsgManager.cpp","Com/MsgSocket.cpp","Com/MsgSocketException.cpp","Com/TcpClient.cpp","Com/TcpServer.cpp","Com/TcpUdpClientServer.cpp","Com/UdpExchange.cpp"]
)

env.Append(CPPPATH="ServiceControl")
libToInstall += env.SharedLibrary(
    target="control",
    source=["ServiceControl/ControlServer.cpp","ServiceControl/VariableAttribut.cpp","ServiceControl/Attribut.cpp","ServiceControl/BrowseForService.cpp","ServiceControl/ControlClient.cpp","ServiceControl/ControlUtils.cpp","ServiceControl/InOutputAttribut.cpp","ServiceControl/IntVariableAttribut.cpp","ServiceControl/OmiscidServices.cpp","ServiceControl/Service.cpp","ServiceControl/ServiceFromXML.cpp","ServiceControl/ServiceProperties.cpp","ServiceControl/ServicesCommon.cpp","ServiceControl/WaitForServices.cpp","ServiceControl/XMLTreeParser.cpp"]
#"ServiceControl/RaviControlServer.cpp"
)

hToInstall = []
hToInstall += [["System/System/SimpleString.h","System"],["System/System/AtomicCounter.h","System"],["System/System/Event.h","System"],["System/System/MultipleReferencedData.h","System"],["System/System/Mutex.h","System"],["System/System/Portage.h","System"],["System/System/RecycleSimpleList.h","System"],["System/System/SimpleException.h","System"],["System/System/SimpleList.h","System"],["System/System/SimpleListException.h","System"],["System/System/Socket.h","System"],["System/System/SocketException.h","System"],["System/System/Thread.h","System"]]

hToInstall += [["Com/Com/ComTools.h","Com"],["Com/Com/Message.h","Com"],["Com/Com/MsgManager.h","Com"],["Com/Com/MsgSocket.h","Com"],["Com/Com/MsgSocketException.h","Com"],["Com/Com/TcpClient.h","Com"],["Com/Com/TcpServer.h","Com"],["Com/Com/TcpUdpClientServer.h","Com"],["Com/Com/UdpExchange.h","Com"]]

hToInstall += [["ServiceControl/ServiceControl/ControlServer.h","ServiceControl"],["ServiceControl/ServiceControl/VariableAttribut.h","ServiceControl"],["ServiceControl/ServiceControl/Attribut.h","ServiceControl"],["ServiceControl/ServiceControl/BrowseForService.h","ServiceControl"],["ServiceControl/ServiceControl/ControlClient.h","ServiceControl"],["ServiceControl/ServiceControl/ControlUtils.h","ServiceControl"],["ServiceControl/ServiceControl/InOutputAttribut.h","ServiceControl"],["ServiceControl/ServiceControl/IntVariableAttribut.h","ServiceControl"],["ServiceControl/ServiceControl/OmiscidServices.h","ServiceControl"],["ServiceControl/ServiceControl/Service.h","ServiceControl"],["ServiceControl/ServiceControl/ServiceFromXML.h","ServiceControl"],["ServiceControl/ServiceControl/ServiceProperties.h","ServiceControl"],["ServiceControl/ServiceControl/ServicesCommon.h","ServiceControl"],["ServiceControl/ServiceControl/WaitForServices.h","ServiceControl"],["ServiceControl/ServiceControl/XMLTreeParser.h","ServiceControl"],["ServiceControl/ServiceControl/RaviControlServer.h","ServiceControl"]]

binToInstall += primaDotInFileTarget(env, "Com/primacom-config", primaMapping())
binToInstall += primaDotInFileTarget(env, "System/primasystem-config", primaMapping())
binToInstall += primaDotInFileTarget(env, "ServiceControl/primacontrol-config", primaMapping())

primaInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)


