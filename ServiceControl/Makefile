# -*- Makefile -*-

include Makefile.conf

CPPFLAGS+= ${CFLAGS_LIB_COM} ${CFLAGS_LIB_XML2} ${CFLAGS_LIB_DNSSD} -I.
HFILESDIR=ServiceControl/

SRCNAME = ControlUtils \
	Attribut InOutputAttribut VariableAttribut IntVariableAttribut \
	ControlClient \
	Service ServiceProperties BrowseForService \
	OmiscidServices  WaitForServices\
	ServicesCommon ControlServer XMLTreeParser\
	RaviControlServer ServiceFromXML

LIBNAME= libcontrol.$(LIBEXT)
MODULENAME = modControl

LIBS = -D FOR_LIB ${LDFLAGS_LIB_COM} ${LDFLAGS_LIB_DNSSD} ${LDFLAGS_LIB_XML2}
LIBS_MODULE = -L. -lcontrol

DCL_FILE = -u ${MODULENAME}.dcl

CONFIG_SH=primacontrol-config

all: libs ${RAVI_MAKE}

install: install-hfiles install-lib ${RAVI_INSTALL} install-config
uninstall: uninstall-hfiles uninstall-lib ${RAVI_UNINSTALL} uninstall-config



include Makefile.rules
