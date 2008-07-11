import os
from OmiscidInit import *
from OmiscidScons import *

env = Environment(DISPLAY = 'deimos:0.0', UseInsure = 'no')
OmiscidLinuxMacOSInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,['xml2'])

conf = Configure(env)
OmiscidCheckLibs(conf,['xml2','dns_sd','pthread']);
env = conf.Finish()

if env['UseInsure'] == 'yes' :
 OmiscidMessage('compiling using insure')
 env.Replace(CXX = '/home/oberon/vaufreyd/bin/parasoft/bin/insure')

binToInstall = []
libToInstall = []

# common environment settings
env.AppendUnique(CXXFLAGS = ['-g','-Werror','-Wall','-pedantic','-std=c++98'])
env_system = env.Clone()
env_system.Append(CPPPATH='System')
target_system = env_system.SharedLibrary(
    target='OmiscidSystem',
    source=SystemSources
)
libToInstall += target_system

env_com = env.Clone()
env_com.Append(CPPPATH=['System', 'Com'])
env_com.Prepend(LIBPATH=['.'])
env_com.Append(LIBS = ['OmiscidSystem'])
target_com = env_com.SharedLibrary(
    target='OmiscidCom',
    source=ComSources
)

libToInstall += target_com

env_control = env.Clone()
env_control.Append(CPPPATH=['System', 'Com', 'ServiceControl'])
env_control.Prepend(LIBPATH=['.'])
env_control.Append(LIBS = ['OmiscidCom', 'OmiscidSystem'])
target_control = env_control.SharedLibrary(
    target='OmiscidControl',
    source=ServiceControlSources
)
libToInstall += target_control


env.Depends(target_com, target_system)
env.Depends(target_control, target_com)

hToInstall = []
hToInstall += SystemHeaders
hToInstall += ComHeaders
hToInstall += ServiceControlHeaders

binToInstall += OmiscidDotInFileTarget(env, 'Com/OmiscidCom-config', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, 'System/OmiscidSystem-config', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, 'ServiceControl/OmiscidControl-config', OmiscidMapping())

OmiscidInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

