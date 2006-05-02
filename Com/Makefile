# -*- Makefile -*-

include Makefile.conf

CPPFLAGS=-I. $(CFLAGS_LIB_SYSTEM)
HFILESDIR=Com/


SRCNAME = MsgSocket MsgSocketException \
	ComTools \
	TcpServer TcpClient \
	Message   MsgManager \
	UdpExchange TcpUdpClientServer

LIBNAME= libcom.$(LIBEXT)
MODULENAME = modCom


LIBS=-lpthread $(LDFLAGS_LIB_SYSTEM)

LIBS_MODULE = -L. -lcom

DCL_FILE = -u ${MODULENAME}.dcl

CONFIG_SH=primacom-config

all: libs ${RAVI_MAKE}
install: install-hfiles install-lib ${RAVI_INSTALL} install-config
uninstall: uninstall-hfiles uninstall-lib ${RAVI_UNINSTALL} uninstall-config


include Makefile.rules
