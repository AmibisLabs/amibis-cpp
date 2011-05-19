import os
from OmiscidInit import *
from OmiscidScons import *
env = Environment(DISPLAY = 'deimos:0.0', UseInsure = 'no', OSis = 'Unknown' )
OmiscidLinuxMacOSInit(env,COMMAND_LINE_TARGETS,ARGUMENTS,['xml2'])
conf = Configure(env)
OmiscidCheckLibs(conf,['xml2','dns_sd','pthread']);
env = conf.Finish()
if env['UseInsure'] == 'yes' :
 OmiscidMessage('compiling using insure')
 env.Replace(CXX = 'insure')
binToInstall = []
libToInstall = []
# common environment settings
env.AppendUnique(CXXFLAGS = ['-g','-Werror','-Wall','-pedantic','-std=c++98'])
env.Append(CPPPATH=['System', 'Com', 'ServiceControl', 'Json', 'Messaging'])
env.Prepend(LIBPATH=['.'])
if env['OSis'] == 'Linux' :
 env.AppendUnique(LINKFLAGS = ['-Wl,-soname,libOmiscid.so.1.0'])
target_omiscid = env.SharedLibrary(
    target='Omiscid.1.0.2',
    source=OmiscidSources
)
libToInstall += target_omiscid
hToInstall = []
hToInstall += OmiscidHeaders
binToInstall += OmiscidDotInFileTarget(env, 'Com/OmiscidCom-config', OmiscidMapping(env))
binToInstall += OmiscidDotInFileTarget(env, 'System/OmiscidSystem-config', OmiscidMapping(env))
binToInstall += OmiscidDotInFileTarget(env, 'ServiceControl/OmiscidControl-config', OmiscidMapping(env))
binToInstall += OmiscidDotInFileTarget(env, 'Omiscid-config', OmiscidMapping(env))
OmiscidInstallTarget(env,binToInstall,libToInstall,hToInstall=hToInstall)

