#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  shell 
#  $Id: Makefile,v 1.45 2015/07/15 13:57:04 cnyu Exp $
#  $Author: cnyu $
#  $Revision: 1.45 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
# 
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)

-include $(TOPDIR)/sysconfig

SOURCES	= story.c sh_story.c sh_dfu_install.c dfu_install.S sh_upgrade.c \
		  sh_get_param.c sh_dump_param.c \
          i2cget.c sh_i2cget.c \
          shell.c args.c termio.c env.c \
          sh_cache.c sh_ctag.c \
		  sh_mconfig.c \
          sh_echo.c sh_fat32.c sh_norfs.c sh_ls.c sh_go.c \
          sh_help.c sh_history.c \
          sh_mfc0.c sh_mtc0.c \
          sh_mcomp.c sh_mcopy.c sh_mdump.c sh_mfill.c \
          sh_mtest.c sh_nor.c \
          sh_ostimer.c \
          sh_read421.c sh_reset.c sh_sd.c\
          sh_script.c sh_shutdown.c sh_sleep.c sh_sysinfo.c \
          sh_write421.c \
          sh_aumsg.c sh_audio.c \
          sh_rx.c sh_sysclk.c \
          sh_rx.c sh_sysclk.c  \
          sh_version.c sh_vt.c \
          sh_keypad.c \
          sh_disp_func.c \
          sh_rtc.c \
          sh_vr_ctrl.c \
          sh_cvbs.c \
		  sh_timeout.c \
		  sh_watchdog.c \
		  sh_spi_cmd.c \
		  sh_cq_test.c \
		  sh_registry2.c \
		  sh_recfile.c sh_rec_cyclic.c sh_playfile.c \
		  sh_pic_codec.c sh_decpic3.c \
		  sh_mv_file.c \
		  sh_irqtest.c \
		  sh_stack.c \
		  sh_uartbridge.c \
		  sh_getopt.c \
		  sh_playi80.c
		
SOURCES +=  sh_gpio.c sh_scaler.c


LO_TARGET := shell.lo

SOURCES_TINY = story.c shell.c args.c termio.c env.c \
		  sh_story.c \
          sh_cache.c \
          sh_echo.c \
          sh_help.c sh_history.c \
          sh_read421.c sh_reset.c \
          sh_shutdown.c \
          sh_sysinfo.c  \
          sh_write421.c \
          sh_version.c sh_vt.c\
          sh_vr_ctrl.c sh_rtc.c \
	  sh_dfu_install.c dfu_install.S


S_OBJECTS_TINY = $(SOURCES_TINY:.S=.o)
OBJECTS_TINY = $(S_OBJECTS_TINY:.c=.o) 


all: shell.lo shell_tiny.lo

include $(TOPDIR)/Rules.make

.PHONY: ver_dep
ver_dep: sh_sysinfo.o sh_version.o shell.o
sh_sysinfo.o sh_version.o shell.o : $(TOPDIR)/Makefile

shell_tiny.lo: $(OBJECTS_TINY)
	$(LD) $(ENDIAN) -r -o $@ $^

