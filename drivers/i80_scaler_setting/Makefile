#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  Makefile of I80 system interface driver
#  $Id: Makefile,v 1.2 2014/01/08 03:26:22 jedy Exp $
#  $Author: jedy $
#  $Revision: 1.2 $
#
#  Copyright (c) 2010 Terawins Inc. All rights reserved.
# 
#  @date   2010/10/06  hugo New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)
-include $(TOPDIR)/sysconfig

src-y = out_i80_320x240.c

SOURCES = $(src-y)
LO_TARGET := i80_scaler_setting.lo

CFLAGS	+= -DCONFIG_OUTPUT_16BIT

include $(TOPDIR)/Rules.make

