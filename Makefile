#----------------------------------------------------------------------
#  @file  Makefile
#  @brief makefile for tarzan2
#  $Id: Makefile,v 1.18 2016/07/22 09:39:32 joec Exp $
#  $Author: joec $
#  $Revision: 1.18 $
#
#  Copyright (c) 2010 Terawins Inc. All right reserved.
#
#  @date   2010/09/09  gary     New file. (from TBoot v3.11-beta)
#
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)
-include $(TOPDIR)/sysconfig

PROJDIRS=bob

SHELLLIB=$(SHELLALLLIB)

all: mkproj

clean: appclean

include $(TOPDIR)/Rules.make
