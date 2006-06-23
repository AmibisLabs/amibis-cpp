import os
import sys
import string
from SCons.Util import WhereIs

def primaMessage(str):
 print '--==-- '+str

#############################################
### Command to initialize the environment ###
#############################################
def primaInit(env,commandLineTargets,arguments,options=[]):
 global COMMAND_LINE_TARGETS
 COMMAND_LINE_TARGETS=commandLineTargets
 global ARGUMENTS
 ARGUMENTS=arguments
 if "ravimoduledest" in ARGUMENTS \
 or "ravi" in options:
  env.ParseConfig('ravitool --cflags')
  env.ParseConfig('ravitool --ldflags')
 if "bip" in options:
  env.ParseConfig('xml2-config --cflags')
  env.ParseConfig('xml2-config --libs')
  env.ParseConfig('primacontrol-config --cflags')
  env.ParseConfig('primacontrol-config --libs')
  env.ParseConfig('primacom-config --cflags')
  env.ParseConfig('primacom-config --libs')
  env.ParseConfig('primasystem-config --cflags')
  env.ParseConfig('primasystem-config --libs')
 if "primavision" in options:
  env.ParseConfig('primavision-config --cflags')
  env.ParseConfig('primavision-config --libs')
 if "xml2" in options:
  env.ParseConfig('xml2-config --cflags')
  env.ParseConfig('xml2-config --libs')
 if "ffmpeg" in options:
  if "ffmpeginc" in ARGUMENTS :
   env.Append(CPPPATH = [ARGUMENTS.get("ffmpeginc")])
   #env.Append(LIBS = ["avformat","avcodec"])
  else :
   env.ParseConfig('ffmpeg-config --cflags')
   env.ParseConfig('ffmpeg-config --libs avformat')
 if "svideo" in ARGUMENTS :
  env.Append(CPPPATH = [os.path.join(ARGUMENTS.get("svideo"),"include")])
  env.Append(LIBPATH = [os.path.join(ARGUMENTS.get("svideo"),"lib")])
 if "debug" in ARGUMENTS :
  primaMessage("compiling in debug mode")
  env.Append(CPPFLAGS = ["-g","-Wall"])

######################################
### Command to build a ravi module ###
######################################
def primaRaviModuleTarget(env, target, dcl, addModPaths, source):
 envRavi = env.Copy()
 envRavi.Append(ENV = os.environ)
 raviOptions = ""
 sourceH = []
 sourceO = []
 sourceHString = ""
 sourceOString = ""
 addModPathsString = ""
 for path in addModPaths:
  addModPathsString += " -R "+path
 for path in env["CPPPATH"]:
  raviOptions += " -I "+path
 for s in source:
  sourceH += [s+".h"]
  sourceO += [s+".os"]
  sourceHString += s+".h "
  sourceOString += s+".os "
 output = []
 output += envRavi.Command([target+".cpp",target+".x.scm"],sourceH,"ravitool --generate %s %s -I ./ -o $TARGET %s" % (addModPathsString,dcl,sourceHString))
 envRavi.Command(target+".os",target+".cpp","ravitool --compile %s -o $TARGET $SOURCE" % raviOptions)
 output += envRavi.Command(target+".so",[target+".os"]+sourceO,"ravitool --link %s -o $TARGET $SOURCE" % raviOptions)
 return output

##############################################
### Command to build a file from a file.in ###
##############################################
def primaDotInFileTarget(env, target, mapping):
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

def primaMapping():
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
def primaInstallTarget(env,binToInstall=[],libToInstall=[],modToInstall=[],hToInstall=[]):
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
   primaMessage('prefix must be given for installation')
   primaMessage('you can use "scons prefix=Prefix/Where/To/Install install" to specify the prefix')

##################################
### Command to check some libs ###
##################################
def primaCheckLibs(conf,libs=[]):
 missing = []
 # Fix a strange behaviour: first check (of svideo in the tests) fails but the following are passing as expected
 if not conf.CheckLib():
  primaMessage(":".join(conf.env.Dictionary().get("LIBPATH")))
   
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
   primaMessage(" !!! some libs to check had wrong syntax")

 if not missing == []:
  primaMessage("Some libraries are missing: ")
  primaMessage("   "+", ".join(missing))
  for miss in missing:
   if miss == "svideo":
    primaMessage("missing "+miss)
    primaMessage("       You can specify svideo path using 'scons svideo=/prefix/for/svideo'")
  sys.exit(1)
