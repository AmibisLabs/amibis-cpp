import os
import sys
import string
from SCons.Util import WhereIs

def OmiscidMessage(str):
 print '--==-- '+str

#############################################
### Command to initialize the environment ###
#############################################
def OmiscidInit(env,commandLineTargets,arguments,options=[]):
 # defines Glabal value (will be change...)
 global COMMAND_LINE_TARGETS
 COMMAND_LINE_TARGETS=commandLineTargets
 global ARGUMENTS
 ARGUMENTS=arguments
 if 'omiscid' in options:
  env.ParseConfig('xml2-config --cflags')
  env.ParseConfig('xml2-config --libs')
  env.ParseConfig('Omiscidcontrol-config --cflags')
  env.ParseConfig('Omiscidcontrol-config --libs')
  env.ParseConfig('Omiscidcom-config --cflags')
  env.ParseConfig('Omiscidcom-config --libs')
  env.ParseConfig('Omiscidsystem-config --cflags')
  env.ParseConfig('Omiscidsystem-config --libs')
  
 if 'xml2' in options:
  env.ParseConfig('xml2-config --cflags')
  env.ParseConfig('xml2-config --libs')
  
 DebugMode = False
 if 'debug' in arguments :
  if arguments['debug'] in ['1','yes','true'] :
   DebugMode = True
  elseif arguments['debug'] in ['0','no','false'] :
   DebugMode = False
  else :
   OmiscidMessage 'Bad value for debug flag. Must be '1', 'yes', 'true' for debuging mode or '0', 'no', 'false' for non debugging mode'
   exit
   
 if DebugMode == True :   
  OmiscidMessage('compiling in debug mode (with trace mode)')
  env.AppendUnique(CXXFLAGS = ['-DDEBUG','-DOMISCID_TRACE_ENABLE'])
 else :
  OmiscidMessage('compiling in non-debug mode')
  env.AppendUnique(CXXFLAGS = ["-DNDEBUG"])
 if "trace" in arguments :
  OmiscidMessage('compiling in trace mode')
  env.AppendUnique(CXXFLAGS = ["-DOMISCID_TRACE_ENABLE"])

##############################################
### Command to build a file from a file.in ###
##############################################
def OmiscidDotInFileTarget(env, target, mapping):
 replacements = ""
 for i in mapping.keys():
  replacements += " -e 's#"
  for c in i:
   if c in string.letters:
    replacements += "[" + c + "]"
   else:
    replacements += "\\" + c
  replacements += "#"
  for c in mapping[i]:
   if c in string.letters or c in string.digits:
    replacements += c
   else:
    replacements += "\\" + c
  replacements += "#g'"
 output = []
 if len(replacements) == 0:
  output += env.Command(target,target+".in","cat $SOURCE > $TARGET && chmod 755 $TARGET")
 else:
  output += env.Command(target,target+".in","sed %s $SOURCE > $TARGET && chmod 755 $TARGET" % replacements)
 return output

##############################################
### Command to map file ###
##############################################
def OmiscidMapping():
 if "prefix" in ARGUMENTS:
  return {
        "@prefix@": ARGUMENTS.get("prefix"),
        "@includedir@": os.path.join(ARGUMENTS.get("prefix"), "include"),
        "@bindir@": os.path.join(ARGUMENTS.get("prefix"), "bin"),
        "@libdir@": os.path.join(ARGUMENTS.get("prefix"), "lib")}
 else:
  return {}

##############################################
### Command to generate the install target ###
##############################################
def OmiscidInstallTarget(env,binToInstall=[],libToInstall=[],modToInstall=[],hToInstall=[]):
 global COMMAND_LINE_TARGETS
 global ARGUMENTS
 if "install" in COMMAND_LINE_TARGETS :
  if "prefix" in ARGUMENTS :
   prefix_bin = os.path.join(ARGUMENTS.get("prefix"), "bin")
   prefix_lib = os.path.join(ARGUMENTS.get("prefix"), "lib")
   prefix_h = os.path.join(ARGUMENTS.get("prefix"), "include")
   env.Install(prefix_bin, binToInstall)
   env.Install(prefix_lib, libToInstall)
   hTargetToInstall = []
   for i in hToInstall:
    if type(i) in (str, unicode):
     hTargetToInstall += env.Install(os.path.join(prefix_h,i))
    else:
     hTargetToInstall += env.Install(os.path.join(prefix_h,i[1]),i[0])
   #env.Install(prefix_h, hTargetToInstall)
   toInstall = [prefix_bin,prefix_lib,hTargetToInstall]
   if "ravimoduledest" in ARGUMENTS:
    destScm = ARGUMENTS.get("ravimoduledest")
    destSo = os.path.join(destScm,os.popen(WhereIs('ravitool')+' --host').read().strip("\n"))
    for i in modToInstall:
     if i.name.endswith(".scm"):
      env.Install(destScm, i)
     elif not i.name.endswith(".cpp"):
      env.Install(destSo, i)
    toInstall += [destScm,destSo]
   env.Alias("install", toInstall)
  else :
   OmiscidMessage('prefix must be given for installation')
   OmiscidMessage('you can use "scons prefix=Prefix/Where/To/Install install" to specify the prefix')

##################################
### Command to check some libs ###
##################################
def OmiscidCheckLibs(conf,libs=[]):
 missing = []
 # Fix a strange behaviour: first check (of svideo in the tests) fails but the following are passing as expected
 if not conf.CheckLib():
  OmiscidMessage(":".join(conf.env.Dictionary().get("LIBPATH")))
   
 for lib in libs:
  if type(lib) in (str, unicode):
   if "svideo" == lib:
    if not conf.CheckLibWithHeader("svideo","svideotools/VideoClient.h","CXX"):
     missing += ["svideo"]
   elif "dns_sd" == lib:
    if not conf.CheckCXXHeader("dns_sd.h") or \
       not conf.CheckLib([None, "dns_sd"], "DNSServiceRegister"):
     missing += ["dns_sd"]
   elif "xml2" == lib:
    if not conf.CheckLibWithHeader("xml2","libxml/tree.h","CXX"):
     missing += ["xml2"]
   elif "bip" == lib:
    if not conf.CheckLibWithHeader("xml2","libxml/tree.h","CXX"):
     missing += ["xml2"]
    if not conf.CheckLibWithHeader("control","ServiceControl/ControlServer.h","CXX"):
     missing += ["control"]
   elif not conf.CheckLib(lib):
    missing += [lib]
  elif len(lib) == 1:
   if not conf.CheckLib(lib[0]):
    missing += [lib[0]]
  elif len(lib) == 2:
   if not conf.CheckLibWithHeader(lib[0],lib[1],"CXX"):
    missing += [lib[0]]
  elif len(lib) == 3:
   if not conf.CheckLibWithHeader(lib[0],lib[1],lib[2]):
    missing += [lib[0]]
  else:
   OmiscidMessage(" !!! some libs to check had wrong syntax")

 if not missing == []:
  OmiscidMessage("Some libraries are missing: ")
  OmiscidMessage("   "+", ".join(missing))
  for miss in missing:
   if miss == "svideo":
    OmiscidMessage("missing "+miss)
    OmiscidMessage("       You can specify svideo path using 'scons svideo=/prefix/for/svideo'")
  sys.exit(1)
