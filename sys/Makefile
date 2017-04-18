#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  sys 
#  $Id: Makefile,v 1.11 2014/08/06 07:07:36 jedy Exp $
#  $Author: jedy $
#  $Revision: 1.11 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
# 
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)
-include $(TOPDIR)/sysconfig

SOURCES= crt0.S init.c entry.S mipsIRQ_srs.S mipssetjmp.S irq.c t300_irq.c trap.c c-r4k.c sd_upgrade.c sys.c main_drive.c march.S eyecatch.c tw_widget_sys.c threads.c spinlock.c cpu_wait.c t300_ostimer.c

include $(TOPDIR)/Rules.make
all: $(TARGET)

t300_irq.s: t300_irq.c
	$(CC) $(CFLAGS) -S -o $@ $< 

