import os
import sys
import string
import random
import re

from SCons.Util import WhereIs

def OmiscidMessage(str):
 print '--==-- '+str

########################################################################################
### Command to initialize the environment for Linux/MacOS and in future gcc on Win32 ###
########################################################################################
def OmiscidLinuxMacOSInit(env,commandLineTargets,arguments,options=[]):
 # defines Glabal value (will be change...)
 # global COMMAND_LINE_TARGETS
 # COMMAND_LINE_TARGETS=commandLineTargets
 # global ARGUMENTS
 # ARGUMENTS=arguments
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
 
 # check debug et trace parameter
 DebugMode = False
 if 'debug' in arguments :
  if arguments['debug'] in ['1','yes','true'] :
   DebugMode = True
  elif arguments['debug'] in ['0','no','false'] :
   DebugMode = False
  else :
   OmiscidMessage("Bad value for debug flag. Must be '1', 'yes', 'true' for debuging mode or '0', 'no', 'false' for non debugging mode")
   Exit()

 TraceMode = False
 if 'trace' in arguments :
  if arguments['trace'] in ['1','yes','true'] :
   TraceMode = True
  elif arguments['trace'] not in ['0','no','false'] :
   OmiscidMessage("Bad value for trace flag. Must be '1', 'yes', 'true' for tracing mode or '0', 'no', 'false' for non tracing mode")
   Exit()
   
 # Do what we ask   
 if DebugMode == True :   
  OmiscidMessage('compiling in debug mode (with trace mode)')
  env.AppendUnique(CXXFLAGS = ['-DDEBUG','-DOMISCID_TRACE_ENABLE'])
 else :
  OmiscidMessage('compiling in non-debug mode')
  env.AppendUnique(CXXFLAGS = ['-DNDEBUG','-O3'])

 # do not add flag trace, we already add it
 if DebugMode == True :
  return

 if TraceMode == True :
  OmiscidMessage('compiling in trace mode')
  env.AppendUnique(CXXFLAGS = ['-DOMISCID_TRACE_ENABLE'])

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
 # global COMMAND_LINE_TARGETS
 # global ARGUMENTS
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
   sys.exit(1)

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
  
  
########################################################################################
### Command to initialize the environment for Linux/MacOS and in future gcc on Win32 ###
########################################################################################
def OmiscidWindowsInit(env,commandLineTargets,arguments,options=[]):
	global ProjectType
	global ProjectName
	
	# Default type
	ProjectType = 'console'
	
	if 'help' in commandLineTargets :
		OmiscidMessage('HELP')
		sys.exit(0)
	
	if 'type' in arguments :
		if arguments['type'] == 'console' :
			ProjectType = 'console'
		elif arguments['type'] == 'mfc' :
			ProjectType = 'mfc'
			OmiscidMessage('Visual studio project type \'mfc\' unsupported for the moment' + arguments['type'])
			sys.exit(1)
		else :
			OmiscidMessage('Unsupported Visual studio project type' + arguments['type'])
			sys.exit(1)
	
	if 'name' not in arguments or arguments['name'] == '' :
		OmiscidMessage('You must specify at least a non-empty project name invoquing "scons name=MyProjectName"')
		sys.exit(1)
	else
		ProjectName = arguments['name']

	
	return

	# TraceMode = False
	# if 'trace' in arguments :
	#  if arguments['trace'] in ['1','yes','true'] :
	#   TraceMode = True
	#  elif arguments['trace'] not in ['0','no','false'] :
	#   OmiscidMessage("Bad value for trace flag. Must be '1', 'yes', 'true' for tracing mode or '0', 'no', 'false' for non tracing mode")
	#   Exit()
  
##################################
### Command to check some libs ###
##################################
def OmiscidCreateVisualStudioProject() :
	
	#seed the ramdom generator with system time
	random.seed()

	ProjectName   = 'Test';
	ProjectFolder = '../' + ProjectName
	
	if ( os.path.isfile( ProjectFolder ) ) :
		OmiscidMessage( ProjectName + ' is a file. Could not create project folder.' )
		sys.exit(1)
		
	ProjectFolder = ProjectFolder + '/'
	if ( os.path.isdir( ProjectFolder ) ) :
		OmiscidMessage( 'Warning : ' + ProjectName + ' folder already exists. Work in it.' )
	else :	
		os.mkdir( ProjectFolder )

	FileIn  = open ( 'VisualStudio.in/OMiSCID-' + ProjectType + '.sln', 'r' )
	FileOut = open ( ProjectFolder + ProjectName + '.sln', 'w' )
	for Line in FileIn.readlines():
		Line = re.sub( '##PROJECTNAME##', ProjectName, Line )
		FileOut.write( Line )
	FileOut.close()
	FileIn.close()

	FileIn  = open ( 'VisualStudio.in/OMiSCID-' + ProjectType + '.vcproj', 'r' )
	FileOut = open ( ProjectFolder + ProjectName + '.vcproj', 'w' )
	for Line in FileIn.readlines():
		Line = re.sub( '##PROJECTNAME##', ProjectName, Line )
		FileOut.write( Line )
	FileOut.close()
	FileIn.close()
	
	FileIn  = open ( 'README', 'r' )
	FileOut = open ( ProjectFolder + 'README', 'w' )
	for Line in FileIn.readlines():
		FileOut.write( Line )
	FileOut.close()
	FileIn.close()

	FileIn  = open ( 'LICENCE', 'r' )
	FileOut = open ( ProjectFolder + 'LICENCE', 'w' )
	for Line in FileIn.readlines():
		FileOut.write( Line )
	FileOut.close()
	FileIn.close()

	sys.exit(0)