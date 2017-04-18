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

src-y = i80.c gui.c pattern.c

ifeq ($(CONFIG_ILI9320),y)
src-y += ili9320.c
endif

ifeq ($(CONFIG_ILI9327),y)
src-y += ili9327.c
endif

ifeq ($(CONFIG_HX8347B), y)
src-y += hx8347b.c
endif

SOURCES = $(src-y)
LO_TARGET := i80_kirin.lo

CFLAGS	+= -DCONFIG_OUTPUT_16BIT

include $(TOPDIR)/Rules.make

