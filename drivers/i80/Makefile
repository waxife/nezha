#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  Makefile of I80 system interface driver
#  $Id: Makefile,v 1.1 2014/03/14 09:25:21 kevin Exp $
#  $Author: kevin $
#  $Revision: 1.1 $
#
#  Copyright (c) 2010 Terawins Inc. All rights reserved.
# 
#  @date   2010/10/06  hugo New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)
-include $(TOPDIR)/sysconfig

src-y = i80.c gui.c pattern.c

SOURCES = $(src-y)
LO_TARGET := i80.lo

CFLAGS	+= -DCONFIG_OUTPUT_16BIT

include $(TOPDIR)/Rules.make

