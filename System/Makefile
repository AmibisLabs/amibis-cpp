# -*- Makefile -*-

include Makefile.conf

CPPFLAGS=-I.
HFILESDIR=System/


SRCNAME = Thread Mutex Event AtomicCounter\
	Socket SocketException \
	SimpleList SimpleString \
	RecycleSimpleList MultipleReferencedData \
	SimpleException SimpleListException Portage

LIBNAME= libprimasystem.$(LIBEXT)
MODULENAME = modPrimaSystem


LIBS=-lpthread

LIBS_MODULE = -L. -lprimasystem

DCL_FILE = -u ${MODULENAME}.dcl

CONFIG_SH=primasystem-config

all: libs ${RAVI_MAKE}
install: install-hfiles install-lib ${RAVI_INSTALL} install-config
uninstall: uninstall-hfiles uninstall-lib ${RAVI_UNINSTALL} uninstall-config


include Makefile.rules
