AC_INIT(TcpServer.cpp)

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
DOCDIR_COM="$prefix/share/doc/Com"

###################### LIB PRIMA SYSTEM ############################
AC_CHECK_PROG(primasystemconfig, primasystem-config, yes, no)
if test "$primasystemconfig" = "no"
then
  AC_MSG_ERROR([Cannot find primasystem-config (maybe not in your path)])
else
  CFLAGS_LIB_SYSTEM=`primasystem-config --cflags`
  LDFLAGS_LIB_SYSTEM=`primasystem-config --libs`
  DOC_TAG_SYSTEM=`primasystem-config --doc-tag`
fi
########################################################

LDFLAGS="$LDFLAGS -fPIC -shared"

AC_SUBST(CXX)
AC_SUBST(RM)
AC_SUBST(MKDIRHIER)
AC_SUBST(INSTALL)
AC_SUBST(UNINSTALL)

AC_SUBST(RAVITOOL)
AC_SUBST(HOST)

AC_SUBST(RAVIMOD)
AC_SUBST(RAVI_MAKE)
AC_SUBST(RAVI_INSTALL)
AC_SUBST(RAVI_UNINSTALL)

AC_SUBST(DOCDIR_COM)
AC_SUBST(DOC_TAG_SYSTEM)

AC_SUBST(CFLAGS_LIB_SYSTEM)
AC_SUBST(LDFLAGS_LIB_SYSTEM)


AC_OUTPUT(Makefile.conf \
			primacom-config \
			Doxyfile)

chmod a+x primacom-config

