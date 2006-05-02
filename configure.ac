AC_INIT(Makefile.in)

AC_PROG_CXX
CXX="g++"
CXXFLAGS="-O3 -Wall"

AC_PROG_CC
CFLAGS="-O3 -Wall"

RM="/bin/rm -rf"
MKDIRHIER="mkdir -p"
INSTALL="/bin/cp"
UNINSTALL="/bin/rm -f"


###################### RAVI ##########################################

RAVITOOL=""
HOST=""

### Use RAVI
RAVI_MAKE=""
RAVI_INSTALL=""
RAVI_UNINSTALL=""

AC_ARG_ENABLE(ravimodule, 
              AC_HELP_STRING([--enable-ravimodule], 
                             [create the ravi modules]),
              [enable_ravimodules=$enableval],
              [enable_ravimodules=no])
if test "$enable_ravimodules" != "no" ; then
 RAVITOOL="ravitool"
 HOST="`ravitool --host`"

 RAVI_MAKE="objs modules"
 RAVI_INSTALL="install-module"
 RAVI_UNINSTALL="uninstall-module"
fi
###################### PREFIX ##########################
if test "$prefix" = "NONE"
then
	prefix="/usr/local"
fi
libdir="$prefix/lib/"
includedir="$prefix/include/"
bindir="$prefix/bin/"
RAVIMOD="$prefix/RaviModules/"

DOCDIR_SYSTEM="$prefix/share/doc/System"
DOCDIR_COM="$prefix/share/doc/Com"
DOCDIR_CONTROL="$prefix/share/doc/ServiceControl"


########################################################

###################### LIB XML 2  ##########################
AC_CHECK_PROG(xml2config, xml2-config, yes, no)
if test "$xml2config" = "yes"
then
  CFLAGS_LIB_XML2=` xml2-config --cflags`
  LDFLAGS_LIB_XML2=` xml2-config --libs`
else
  AC_MSG_ERROR([Cannot find xml2-config (maybe not in your path) : the libxml2 library in needed by the control part])
fi
############################################################
###################### LIB DNS-SD ############################
if test `uname` = "Darwin"
then
  dnssdlibraryfound="yes"
  LDFLAGS_LIB_DNSSD=""
else
  AC_HAVE_LIBRARY(dns_sd, dnssdlibraryfound="yes", dnssdlibraryfound="no")
  if test "$dnssdlibraryfound" != "yes"
  then
    AC_MSG_ERROR([Cannot find the dns-sd library : libdns_sd.so"])
  fi
  LDFLAGS_LIB_DNSSD="-ldns_sd"
fi

AC_CHECK_HEADER(dns_sd.h, dnssdincludefound="yes", dnssdincludefound="no")
if test "$dnssdincludefound" = "yes"
then
  CFLAGS_LIB_DNSSD=""
else
  AC_MSG_ERROR([Cannot find the dns-sd include files : dns_sd.h"])
fi
########################################################

BASE_PATH=`pwd`
# FOR COM
CFLAGS_LIB_SYSTEM="-I$BASE_PATH/System/"
LDFLAGS_LIB_SYSTEM="-L$BASE_PATH/System/ -lprimasystem"
# FOR CONTROL
CFLAGS_LIB_COM="-I$BASE_PATH/Com/ $CFLAGS_LIB_SYSTEM"
LDFLAGS_LIB_COM="-L$BASE_PATH/Com/ -lcom -L$BASE_PATH/System/ -lprimasystem"


#DOC_TAG_COM=${DOCDIR_COM}/doc.tag=${DOCDIR_COM}
#DOC_TAG_SYSTEM=${DOCDIR_SYSTEM}/doc.tag=${DOCDIR_SYSTEM}

DOC_TAG_COM=${BASE_PATH}/Com/DOC/html/doc_com.tag=${BASE_PATH}/Com/DOC/html
DOC_TAG_SYSTEM=${BASE_PATH}/System/DOC/html/doc_system.tag=${BASE_PATH}/System/DOC/html

if test `uname` = "Darwin"
then
  LDFLAGS="$LDFLAGS -dynamiclib"
  LIBEXT=dylib
else
  LDFLAGS="$LDFLAGS -fPIC -shared"
  LIBEXT=so
fi

SUBDIRS="System Com ServiceControl"

##############################################################
VERSION_NUMBER="000"
AC_ARG_ENABLE(svn-version, 
              AC_HELP_STRING([--enable-svn-version], 
                             [extract version number from svn]),
              [enable_svnversion=$enableval],
              [enable_svnversion=no])
if test "$enable_svnversion" != "no" ; then
 VERSION_NUMBER=`svn info | grep Revision: | cut -c 11-`
fi
TAR_NAME=omiscid-1.0.${VERSION_NUMBER}
##############################################################

AC_SUBST(CXX)
AC_SUBST(LDFLAGS)
AC_SUBST(LIBEXT)
AC_SUBST(RM)
AC_SUBST(MKDIRHIER)
AC_SUBST(INSTALL)
AC_SUBST(UNINSTALL)

AC_SUBST(RAVITOOL)
AC_SUBST(HOST)

AC_SUBST(BASE_PATH)

AC_SUBST(DOCDIR_SYSTEM)
AC_SUBST(DOCDIR_COM)
AC_SUBST(DOCDIR_CONTROL)
AC_SUBST(DOC_TAG_SYSTEM)
AC_SUBST(DOC_TAG_COM)

AC_SUBST(RAVIMOD)
AC_SUBST(RAVI_MAKE)
AC_SUBST(RAVI_INSTALL)
AC_SUBST(RAVI_UNINSTALL)

# FOR COM
AC_SUBST(CFLAGS_LIB_SYSTEM)
AC_SUBST(LDFLAGS_LIB_SYSTEM)

#FOR CONTROL
AC_SUBST(CFLAGS_LIB_COM)
AC_SUBST(LDFLAGS_LIB_COM)

AC_SUBST(CFLAGS_LIB_XML2)
AC_SUBST(LDFLAGS_LIB_XML2)

AC_SUBST(CFLAGS_LIB_DNSSD)
AC_SUBST(LDFLAGS_LIB_DNSSD)

AC_SUBST(SUBDIRS)

AC_SUBST(TAR_NAME)

AC_OUTPUT(Makefile \
	  System/Makefile.conf \
	  System/primasystem-config \
	  Com/Makefile.conf \
	  Com/primacom-config \
	  Com/Doxyfile \
	  ServiceControl/Makefile.conf \
	  ServiceControl/primacontrol-config \
	  ServiceControl/Doxyfile)

chmod a+x System/primasystem-config
chmod a+x Com/primacom-config
chmod a+x ServiceControl/primacontrol-config

cp omiscid.ebuild ${TAR_NAME}.ebuild
