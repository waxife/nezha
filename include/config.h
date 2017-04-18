/**
 *  @file    config.h
 *  @brief   system config.h
 *  $Id: config.h,v 1.9 2014/05/21 11:14:12 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.9 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 *
 *  @date 2014/01/15    Jedy    New file
 *
 */


#ifndef _CONFIG_H
#define _CONFIG_H

//#define CONFIG_STACK_ADDR_RSV_BUF       0x88020000
#define CONFIG_RESERVE_CODEC_BUFFER     1
#define CONFIG_FAT_SEG_SIZE             1
#define CONFIG_AUMSG_CODEC_FORMAT       4
#define CONFIG_AUMSG_BUFDES_SIZE        2048
#define CONFIG_NORFS_RECORD_MAX_SIZE    (0x10000-16)

#define CONFIG_JPEG_QUALITY             75
#define CONFIG_DEF_MJPEG_QUALITY        50



#define CONFIG_MEMPOOL_MAX_ELEMENT      32

#ifndef __ASSEMBLY__
extern const unsigned char  config_console_enable;
extern const unsigned char  config_debug;
extern const unsigned long  config_debug_level;
extern const unsigned long  config_sys_uart_baud;
extern const char config_code_nor_layout[];
extern const unsigned long  config_watchdog_timeout;
extern const unsigned long  config_protected;
extern const unsigned long  config_ringtong_num;
extern const unsigned long  config_ringtong_unitsize;
extern const unsigned long config_rtc_freq_int;
extern const unsigned long config_rtc_freq_prec;
extern const unsigned int  config_jpeg_threshold;
extern const unsigned char config_osd2_only;
#endif /* __ASSEMBLY__ */

#endif
