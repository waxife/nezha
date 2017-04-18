#----------------------------------------------------------------------
#  @file   Makefile
#  @brief
#  $Id: Makefile,v 1.1 2014/03/14 12:11:36 ken Exp $
#  $Author: ken $
#  $Revision: 1.1 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
#
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR := $(PROJ_NEZHA_DIR)
include .config

SOURCES= main.c page_ST_LOGO.c  file_manip.c srv_evt_cmd.c lsf_manip.c userdata.c
ALL_SUBOBJS= task/task.la OSD/OSD.la disp/disp.lo $(SYSLIB)

RESOURCE=t582_res.bin

DFU= bob.dfu

all: mkapp 

include $(TOPDIR)/Rules.make

clean: appclean

