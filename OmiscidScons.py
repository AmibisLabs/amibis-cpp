import os
import sys
import string
import random
import re
import SCons

from SCons.Util import WhereIs

Chmod = SCons.Action.ActionFactory(os.chmod, lambda dest, mode: 'Chmod: "%s" with 0%o' % (dest, mode)) 

def OmiscidMessage(str):
 print '--==-- '+str

########################################################################################
### Command to initialize the environment for Linux/MacOS and in future gcc on Win32 ###
########################################################################################
def OmiscidLinuxMacOSInit(env,commandLineTargets,arguments,options=[]):
 # defines Global value (will be change...)
 global COMMAND_LINE_TARGETS
 COMMAND_LINE_TARGETS=commandLineTargets
 global ARGUMENTS
 ARGUMENTS=arguments
 global WhichZeroConfLibrary
 global TraceMode
 global DoValgrind
 
 if 'xml2' in options:
  env.ParseConfig('xml2-config --cflags')
  env.ParseConfig('xml2-config --libs')
  
 # default values
 if os.name == 'posix' :
  if string.find(sys.platform, 'darwin') != -1:
   # Mac OS
   WhichZeroConfLibrary = 'OMISCID_USE_MDNS' 
  else :
   # default for other posix plateform
   # Was WhichZeroConfLibrary = 'OMISCID_USE_AVAHI' before Avahi crash problems
   WhichZeroConfLibrary = 'OMISCID_USE_MDNS'
 else :
  WhichZeroConfLibrary = 'OMISCID_USE_MDNS'
  
 if 'zeroconf' in arguments :
  if arguments['zeroconf'] in ['avahi','Avahi'] :
   WhichZeroConfLibrary = 'OMISCID_USE_AVAHI'
  elif arguments['zeroconf'] in ['mdns','mDNS','MDNS','dns_sd'] :
   WhichZeroConfLibrary = 'OMISCID_USE_MDNS'
  else :
   OmiscidMessage("Bad value for zeroconf flag. Must be 'avahi' or 'Avahi' for avahi usage or 'mdns', 'mDNS', 'MDNS' or 'dns_sd' for DNS-SD usage")
   sys.exit(1)
   
 # check debug et trace parameter
 DebugMode = False
 DoValgrind = False
 if 'debug' in arguments :
  if arguments['debug'] in ['1','yes','true','valgrind'] :
   DebugMode = True
   if arguments['debug'] == 'valgrind' :
    OmiscidMessage('prepare Omiscid for valgrind')
    env.AppendUnique(CXXFLAGS = ['-O0','-fno-inline'])
    DoValgrind = True
  elif arguments['debug'] in ['0','no','false'] :
   DebugMode = False
  else :
   OmiscidMessage("Bad value for debug flag. Must be '1', 'yes', 'true' for debuging mode or '0', 'no', 'false' for non debugging mode")
   sys.exit(1)

 TraceMode = False
 if 'trace' in arguments :
  if arguments['trace'] in ['1','yes','true'] :
   TraceMode = True
  elif arguments['trace'] not in ['0','no','false'] :
   OmiscidMessage("Bad value for trace flag. Must be '1', 'yes', 'true' for tracing mode or '0', 'no', 'false' for non tracing mode")
   sys.exit(1)
   
 ChMemMode = False
 if 'chmem' in arguments :
  if arguments['chmem'] in ['1','yes','true'] :
   ChMemMode = True
  elif arguments['chmem'] not in ['0','no','false'] :
   OmiscidMessage("Bad value for chmem flag. Must be '1', 'yes', 'true' for tracing mode or '0', 'no', 'false' for non tracing mode")
   sys.exit(1)
   
 # Do what we ask
 if WhichZeroConfLibrary == 'OMISCID_USE_AVAHI' :
  OmiscidMessage('compiling using avahi')
  env.AppendUnique(CXXFLAGS = ['-DOMISCID_USE_AVAHI'])
 elif WhichZeroConfLibrary == 'OMISCID_USE_MDNS' :
  OmiscidMessage('compiling using mdns (DNS-SD, Zeroconf, Bonjour, rendez-vous...)')
  env.AppendUnique(CXXFLAGS = ['-DOMISCID_USE_MDNS'])
  
 if ChMemMode == True :   
  OmiscidMessage('compiling using memory leak detection mode')
  env.AppendUnique(CXXFLAGS = ['-DTRACKING_MEMORY_LEAKS'])
 
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
  # OmiscidMessage( "cat $SOURCE > $TARGET && chmod 755 $TARGET" )
  output += env.Command(target,target+".in","cat $SOURCE > $TARGET && chmod 755 $TARGET")
 else:
  # OmiscidMessage( "sed %s $SOURCE > $TARGET && chmod 755 $TARGET" % replacements )
  output += env.Command(target,target+".in","sed %s $SOURCE > $TARGET && chmod 755 $TARGET" % replacements)
 return output

##############################################
### Command to map file ###
##############################################
def OmiscidMapping():
 global WhichZeroConfLibrary
 global TraceMode
 global DoValgrind
 
 ReplaceList = {}
	
 if "prefix" in ARGUMENTS:
  ReplaceList = {
        "@prefix@": ARGUMENTS.get("prefix"),
        "@includedir@": os.path.join(ARGUMENTS.get("prefix"), "include", "Omiscid"),
        "@bindir@": os.path.join(ARGUMENTS.get("prefix"), "bin"),
        "@libdir@": os.path.join(ARGUMENTS.get("prefix"), "lib")}

 if WhichZeroConfLibrary == 'OMISCID_USE_AVAHI' :
  ReplaceList['@zeroconfflag@'] = ' -D' + WhichZeroConfLibrary + ' '
  ReplaceList['@zeroconflib@'] = ' -lavahi-client '
 else :
  if WhichZeroConfLibrary == 'OMISCID_USE_MDNS' :
   ReplaceList['@zeroconfflag@'] = ' -D' + WhichZeroConfLibrary + ' '
   ReplaceList['@zeroconflib@'] = ' -ldns_sd '
  else :
   OmiscidMessage("Bad value for zeroconf flags (internal).")
   sys.exit(1)
   
 if TraceMode == True :
  ReplaceList['@OmiscidTraceFlags@'] = ' -DOMISCID_TRACE_ENABLE '
 else :
  ReplaceList['@OmiscidTraceFlags@'] = ' '
  
 ReplaceList['@OmiscidCompilFlags@'] = ' '
 
 return ReplaceList


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
   prefix_h = os.path.join(ARGUMENTS.get("prefix"), "include", "Omiscid")
   lTarget = env.Install(prefix_bin, binToInstall)
   env.AddPostAction( lTarget, Chmod( prefix_bin, 0755 ) )
   lTarget = env.Install(prefix_lib, libToInstall)
   env.AddPostAction( lTarget, Chmod( prefix_lib, 0755 ) )
   hTargetToInstall = []
   for i in hToInstall:
    if type(i) in (str, unicode):
     lTarget = env.Install(os.path.join(prefix_h,i))
     env.AddPostAction( lTarget, Chmod( os.path.join(prefix_h,i), 0755 ) )
     hTargetToInstall += lTarget
    else:
     lTarget = env.Install(os.path.join(prefix_h,i[1]),i[0])
     env.AddPostAction( lTarget, Chmod( os.path.join(prefix_h,re.sub('^'+i[1]+'\/','',i[0])), 0755 ) )
     hTargetToInstall += lTarget
   #env.Install(prefix_h, hTargetToInstall)
   toInstall = [prefix_bin,prefix_lib,hTargetToInstall]
   env.Alias("install", toInstall)
  else :
   OmiscidMessage('prefix must be given for installation')
   OmiscidMessage('you can use "scons prefix=Prefix/Where/To/Install install" to specify the prefix')
   sys.exit(1)

##################################
### Command to check some libs ###
##################################
def OmiscidCheckLibs(conf,libs=[]):
 global WhichZeroConfLibrary
 missing = []
 
 if WhichZeroConfLibrary == 'OMISCID_USE_AVAHI' :
  libs.append('avahi-client')
 elif WhichZeroConfLibrary == 'OMISCID_USE_MDNS' :
  libs.append('dns_sd') 	
 else :
  OmiscidMessage("Bad value for zeroconf flag.")
  sys.exit(1)

 # Fix a strange behaviour: first check (of svideo in the tests) fails but the following are passing as expected
 if not conf.CheckLib():
  OmiscidMessage(":".join(conf.env.Dictionary().get("LIBPATH")))
   
 for lib in libs:
  if type(lib) in (str, unicode):
   if "dns_sd" == lib:
    if not conf.CheckCXXHeader("dns_sd.h") or \
       not conf.CheckLib([None, "dns_sd"], "DNSServiceRegister"):
     missing += ["dns_sd"]
   elif "xml2" == lib:
    if not conf.CheckLibWithHeader("xml2","libxml/tree.h","CXX"):
     missing += ["xml2"]
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
  
  
########################################################################################
### Command to initialize the environment for Linux/MacOS and in future gcc on Win32 ###
########################################################################################
def OmiscidWindowsInit(env,commandLineTargets,arguments,options=[]):
	# defines Global value (will be change...)
	global COMMAND_LINE_TARGETS
	COMMAND_LINE_TARGETS=commandLineTargets
	global ARGUMENTS
	ARGUMENTS=arguments
	
 	global ProjectType
	global ProjectName
	global LibXML
	global LibIconv
	global LibBonjour
	
	# Default type
	ProjectType = 'console'
	
	if 'help' in commandLineTargets :
		OmiscidMessage('HELP')
		sys.exit(0)
	
	if 'name' not in arguments or arguments['name'] == '' :
		OmiscidMessage("You must specify at least a non-empty project name invoquing 'scons name=MyProjectName'.")
		sys.exit(1)
	else :
		ProjectName = arguments['name']
		
		
	if 'type' in arguments :
		if arguments['type'] == 'console' :
			ProjectType = 'console'
		elif arguments['type'] == 'mfc' :
			ProjectType = 'mfc'
			OmiscidMessage("Visual studio project type 'mfc' unsupported for the moment." )
			sys.exit(1)
		else :
			OmiscidMessage('Unsupported Visual studio project type' + arguments['type'])
			sys.exit(1)
	
	LibXML = '../libxml2/'
	if 'libxml2' not in arguments :
		OmiscidMessage( "Assuming, 'libxml2' is installed in '" + LibXML + "'." )
	else :
		LibXML = os.path.normpath( arguments['libxml2'] )
		LibXML = re.sub( '\$', '', LibXML ) + '\\'
		OmiscidMessage( "'libxml' install folder : '" + LibXML + "'." )

	LibIconv = '../iconv/'
	if 'iconv' not in arguments :
		OmiscidMessage( "Assuming, 'iconv' is installed in '" + LibIconv + "'." )
	else :
		LibIconv = os.path.normpath( arguments['iconv'] )
		LibIconv = re.sub( '\$', '', LibIconv ) + '\\'
		OmiscidMessage( "'iconv' install folder : '" + LibIconv + "'." )

	LibBonjour = '../BonjourSDK/'
	if 'bonjour' not in arguments :
		OmiscidMessage( "Assuming, 'bonjour' is installed in '" + LibBonjour + "'." )
	else :
		LibBonjour = os.path.normpath( arguments['bonjour'] )
		LibBonjour = re.sub( '\$', '', LibBonjour ) + '\\'
		OmiscidMessage( "'bonjour' install folder : '" + LibBonjour + "'." )
		
	return

##################################
### Command to check some libs ###
##################################
def OmiscidCreateVisualStudioProject() :
 	global ProjectType
	global ProjectName
	global LibXML
	global LibIconv
	global LibBonjour
		
	#seed the ramdom generator with system time
	random.seed()

	# Generate variables
	ProjectFolder  = '../' + ProjectName
	
	# AdditionalIncludeDirectories="xxx"
	ProjectInclude = 'AdditionalIncludeDirectories="..\OMiSCID\System\;..\OMiSCID\Com\;..\OMiSCID\ServiceControl\;'
	ProjectInclude = ProjectInclude + '&quot;' + LibBonjour + 'include\&quot;;'
	ProjectInclude = ProjectInclude + '&quot;' + LibXML + 'include\&quot;;'
	ProjectInclude = ProjectInclude + '&quot;' + LibIconv + 'include\&quot;"'
	
	# AdditionalDependencies="Ws2_32.lib ..\Rendezvous\lib\dnssd.lib ..\libxml2-2.6.23.win32\lib\libxml2.lib ..\iconv-1.9.1.win32\lib\iconv.lib"
	ProjectLib = 'AdditionalDependencies="Ws2_32.lib '
	ProjectLib = ProjectLib + '&quot;' + LibBonjour + 'lib\&quot; '
	ProjectLib = ProjectLib + '&quot;' + LibXML + 'lib\&quot; '
	ProjectLib = ProjectLib + '&quot;' + LibIconv + 'lib\&quot;"'
	
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
		Line = re.sub( '##ADDITIONALINCLUDEDIRECTORIES##', ProjectInclude, Line )
		Line = re.sub( '##ADDITIONALDEPENDENCIES##', ProjectLib, Line )
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
