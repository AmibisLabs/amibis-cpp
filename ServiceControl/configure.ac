AC_INIT(ControlServer.cpp)

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

RAVITOOL="ravitool"
HOST="`ravitool --host`"

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
DOCDIR_CONTROL="$prefix/share/doc/ServiceControl"

###################### LIB PRIMA COM ############################
AC_CHECK_PROG(primacomconfig, primacom-config, yes, no)
if test "$primacomconfig" = "yes"
then
  CFLAGS_LIB_COM=`primacom-config --cflags`
  LDFLAGS_LIB_COM=`primacom-config --libs`
  DOC_TAG_SYSTEM=`primasystem-config --doc-tag`
  DOC_TAG_COM=`primacom-config --doc-tag`
else
  AC_MSG_ERROR([Cannot find primacom-config (maybe not in your path)])

fi
########################################################

###################### LIB XML 2 ############################
AC_CHECK_PROG(xml2config, xml2-config, yes, no)
if test "$xml2config" = "yes"
then
  CFLAGS_LIB_XML2=` xml2-config --cflags`
  LDFLAGS_LIB_XML2=` xml2-config --libs`
else
  AC_MSG_ERROR([Cannot find xml2-config (maybe not in your path) : the libxml2 library in needed by the control part])
fi
########################################################

###################### LIB DNS-SD ############################
AC_HAVE_LIBRARY(dns_sd, dnssdlibraryfound="yes", dnssdlibraryfound="no")
if test "$dnssdlibraryfound" = "yes"
then
  LDFLAGS_LIB_DNSSD="-ldns_sd"
else
  AC_MSG_ERROR([Cannot find the dns-sd library : libdns_sd.so"])
fi

AC_CHECK_HEADER(dns_sd.h, dnssdincludefound="yes", dnssdincludefound="no")
if test "$dnssdincludefound" = "yes"
then
  CFLAGS_LIB_DNSSD=""
else
  AC_MSG_ERROR([Cannot find the dns-sd include files : dns_sd.h"])
fi
########################################################

LDFLAGS="$LDFLAGS -fPIC -shared"

AC_SUBST(CXX)
AC_SUBST(RM)
AC_SUBST(MKDIRHIER)
AC_SUBST(INSTALL)
AC_SUBST(UNINSTALL)

AC_SUBST(DOCDIR_CONTROL)
AC_SUBST(DOC_TAG_SYSTEM)
AC_SUBST(DOC_TAG_COM)

AC_SUBST(RAVITOOL)
AC_SUBST(HOST)

AC_SUBST(RAVIMOD)
AC_SUBST(RAVI_MAKE)
AC_SUBST(RAVI_INSTALL)
AC_SUBST(RAVI_UNINSTALL)

AC_SUBST(CFLAGS_LIB_COM)
AC_SUBST(LDFLAGS_LIB_COM)

AC_SUBST(CFLAGS_LIB_XML2)
AC_SUBST(LDFLAGS_LIB_XML2)

AC_SUBST(CFLAGS_LIB_DNSSD)
AC_SUBST(LDFLAGS_LIB_DNSSD)


AC_OUTPUT(Makefile.conf \
			primacontrol-config \
			Doxyfile)

chmod a+x primacontrol-config

