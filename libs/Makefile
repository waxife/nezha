#----------------------------------------------------------------------
#  @file   Makefile
#  @brief  libs 
#  $Id: Makefile,v 1.27 2015/04/02 02:14:12 cnyu Exp $
#  $Author: cnyu $
#  $Revision: 1.27 $
#
#  Copyright (c) 2006 Terawins Inc. All rights reserved.
# 
#  @date   2007/02/27   jedy    New file.
#----------------------------------------------------------------------
TOPDIR:=$(PROJ_NEZHA_DIR)
-include $(TOPDIR)/sysconfig

SOURCES	= get_param.c bitops.c ctype.c div.c doprnt2.c getchar.c gets.c \
		memchr.c memcmp.c memcpy.c memdump.c memset.c memchk.c \
		printf.c putchar.c puts.c qsort.c sleep.c stdlib.c getopt.c \
		strcat.c strchr.c strcmp.c strcpy.c strlen.c strncat.c \
        strncmp.c strncpy.c strpbrk.c strsep.c strstr.c strtol.c \
		crc16.c crc32.c \
        sys_libs.c mktime.c localtime.c time.c div64.c \
        registry2.c video_det.c ls.c mconfig.c str2lower.c str2upper.c \
		keymap.c config_weak.c stdatomic.c heap.c uartbridge.c mm.c
#		lshldi3.c lshrdi3.c ashldi3.c ashrdi3.c negdi2.c cmpdi2.c 
#		floatdidf.c fixunsdfdi.c
		

SOURCES	+= get_eq.c iccontrol.c rc.c

SOURCES += ls.c mconfig.c sys_libs.c video_det.c adjvin.c adjvin_weak.c
TARGET := libs.la

include $(TOPDIR)/Rules.make

