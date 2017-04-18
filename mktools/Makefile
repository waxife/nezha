#----------------------------------------------------------------------
#  @file  Makefile
#  @brief makefile for mktools
#  $Id: Makefile,v 1.2 2014/05/21 11:22:56 dos1236 Exp $
#  $Author: dos1236 $
#  $Revision: 1.2 $
#
#  Copyright (c) 2010 Terawins Inc. All right reserved.
#
#  @date   2010/09/09  gary     New file. (from TBoot v3.11-beta)
#
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_ROOT)/src/bootldr/nezha

all:
	@make --no-print-directory -C dfu
	@make --no-print-directory -C resrp
	@make --no-print-directory -C resrp_osd

clean:
	@make --no-print-directory -C dfu clean
	@make --no-print-directory -C resrp clean
	@make --no-print-directory -C resrp_osd clean


