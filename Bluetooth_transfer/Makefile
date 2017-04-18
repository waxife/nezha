#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  T582 demo for door phone
#  $Id: Makefile,v 1.2 2016/07/22 10:50:26 joec Exp $
#  $Author: joec $
#  $Revision: 1.2 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
#
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR := $(PROJ_NEZHA_DIR)
include .config

SOURCES= main.c hello.c serialafc_bt.c
ALL_SUBOBJS= OSD/OSD.la disp_ili9163/disp.lo ring/ring.lo $(SYSLIB)

RESOURCE=t582_res.bin

DFU= hello.dfu

all: mkapp 

include $(TOPDIR)/Rules.make

clean: appclean

