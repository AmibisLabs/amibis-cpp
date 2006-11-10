import os
from OmiscidInit.py import *
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
    source=[]
)
libToInstall += target_system

env_com = env.Copy()
env_com.Append(CPPPATH=['System', 'Com'])
env_com.Append(LIBPATH=['.'])
env_com.Append(LIBS = ['OmiscidSystem'])
target_com = env_com.SharedLibrary(
    target='OmiscidCom',
    source=[]
)

libToInstall += target_com

env_control = env.Copy()
env_control.Append(CPPPATH=['System', 'Com', 'ServiceControl'])
env_control.Append(LIBPATH=['.'])
env_control.Append(LIBS = ['OmiscidCom', 'OmiscidSystem'])
target_control = env_control.SharedLibrary(
    target='OmiscidControl',
    source=[]
)
libToInstall += target_control


env.Depends(target_com, target_system)
env.Depends(target_control, target_com)


hToInstall = []
hToInstall += []

hToInstall += []

hToInstall += []

binToInstall += OmiscidDotInFileTarget(env, 'Com/OmiscidCom-config', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, 'System/OmiscidSystem-config', OmiscidMapping())
binToInstall += OmiscidDotInFileTarget(env, 'ServiceControl/OmiscidControl-config', OmiscidMapping())

OmiscidInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

