#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  sys 
#  $Id: Makefile,v 1.15 2014/08/05 05:14:53 nick1516 Exp $
#  $Author: nick1516 $
#  $Revision: 1.15 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
# 
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)

-include $(TOPDIR)/sysconfig

S_SUBDIRS = serial/ ov/ sd/ audio/ input/ nor/ ttl/ rtc/ spi/ cvbs/ 
S_SUBDIRS += watchdog/ upgrade/ mvfile/ iichost/ 
S_SUBDIRS += osd2/ spiosd/ spirw/ scaler/ calibration/
S_SUBDIRS += gpio/ gpioToiic/ mempool/ touch/ ir/ sarkey/ cq/ volume/ i80/
//S_SUBDIRS += sdio/ wlan/
S_SUBDIRS += $(subdir-y)
TARGET := drivers.la

all: make_sub $(TARGET)

include $(TOPDIR)/Rules.make


