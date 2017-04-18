#----------------------------------------------------------------------
#  @file  Makefile
#  @brief makefile for dfu
#  $Id: Makefile,v 1.1 2014/01/08 03:26:22 jedy Exp $
#  $Author: jedy $
#  $Revision: 1.1 $
#
#  Copyright (c) 2006 Terawins Inc. All right reserved.
#
#  @date   2007/08/27  jedy     New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_ROOT)/src/bootldr/nezha
PWD=$(shell pwd)
SUBDIR=$(patsubst $(TOPDIR)%,%, $(PWD))

SOURCES	= dfu.c
OBJECTS = $(SOURCES:.c=.o)
CC = gcc

CFLAGS = -Wall -O2

dfu: dfu.o

clean:
	rm -f dfu *.o

#include $(TOPDIR)/Rules.make


