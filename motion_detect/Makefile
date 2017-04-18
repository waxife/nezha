#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  T582 demo for door phone
#  $Id: Makefile,v 1.1 2015/07/15 09:51:40 kevin Exp $
#  $Author: kevin $
#  $Revision: 1.1 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
#
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR := $(PROJ_NEZHA_DIR)
include .config

SOURCES= main.c  page_video_view.c srv_evt_cmd.c ui_draw.c  
ALL_SUBOBJS= task/task.la OSD/OSD.la disp/disp.lo $(SYSLIB)

RESOURCE=

DFU= motion_detect.dfu

all: mkapp 

include $(TOPDIR)/Rules.make

clean: appclean

