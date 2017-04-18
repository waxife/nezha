#----------------------------------------------------------------------
#  @file  Makefile
#  @brief makefile for resource remap
#  $Id: Makefile,v 1.1 2014/01/08 03:31:32 jedy Exp $
#  $Author: jedy $
#  $Revision: 1.1 $
#
#  Copyright (c) 2012 Terawins Inc. All right reserved.
#
#  @date   2012/11/28  YC     New file.
#----------------------------------------------------------------------

PWD=$(shell pwd)
SUBDIR=$(patsubst $(TOPDIR)%,%, $(PWD))

SOURCES	= resrp.c
OBJECTS = $(SOURCES:.c=.o)
REL_SOURCES = resrp.c
REL_HEADERS = rc.h

CFLAGS = -Wall -O2

resrp: resrp.o

clean:
	rm -f resrp *.o

rel_install_sub:
	install -d $(INSTALL_PATH)/$(SUBDIR)
	install -m 0644 $(REL_SOURCES) $(INSTALL_PATH)/$(SUBDIR)
ifneq ($(strip $(REL_HEADERS)),)
	install -m 0644 $(REL_HEADERS) $(INSTALL_PATH)/$(SUBDIR)
endif
	install -m 0644 Makefile $(INSTALL_PATH)/$(SUBDIR)

