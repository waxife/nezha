#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  OmniVision Camera Chips drivers Makefile
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

src-y = sccb.c
src-$(CONFIG_OV7670) += ov7670.c
src-$(CONFIG_OV7740) += ov7740.c
src-$(CONFIG_OV7725) += ov7725.c

SOURCES = $(src-y)

LO_TARGET := ov.lo

include $(TOPDIR)/Rules.make

