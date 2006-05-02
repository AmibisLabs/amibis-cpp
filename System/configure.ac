AC_INIT(Thread.cpp)

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

DOCDIR_SYSTEM="$prefix/share/doc/System"

########################################################

LDFLAGS="$LDFLAGS -fPIC -shared"


AC_SUBST(CXX)
AC_SUBST(RM)
AC_SUBST(MKDIRHIER)
AC_SUBST(INSTALL)
AC_SUBST(UNINSTALL)

AC_SUBST(DOCDIR_SYSTEM)

AC_SUBST(RAVITOOL)
AC_SUBST(HOST)

AC_SUBST(RAVIMOD)
AC_SUBST(RAVI_MAKE)
AC_SUBST(RAVI_INSTALL)
AC_SUBST(RAVI_UNINSTALL)

AC_OUTPUT(Makefile.conf \
	primasystem-config)

chmod a+x primasystem-config

