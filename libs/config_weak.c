/**
 *  @file   config_weak.c
 *  @brief  weak variable for default config
 *  $Id: config_weak.c,v 1.6 2014/05/21 11:13:42 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/02/12  jedy
 *
 */

#define _weak_  __attribute__((weak))

/* debug control */
/* SPIOSD RESOURCE SELECT */
const unsigned char config_osd2_only _weak_         =0;/*0: spiosd 1:osd2*/
/* debug*/
const unsigned char config_debug _weak_             =0; /* disable */
const unsigned long config_debug_level _weak_       =0;
/* console control */
const unsigned char config_console_enable _weak_    =1; /* enable */
const unsigned long config_sys_uart_baud _weak_     =38400;
/* nor flash layout */
const char config_code_nor_layout[] _weak_          ="NOR=(64,0-63)";
/* watchdog control */
const unsigned long config_watchdog_timeout _weak_  =0; /* disable */

/* firmware protect control */ 
const unsigned long config_protected _weak_         =0;

/* number of ringtong */
const unsigned long config_ringtong_num _weak_      =3;
const unsigned long config_ringtong_unitsize _weak_ =65536;

/* RTC fine tune parameter  */
const unsigned long config_rtc_freq_int _weak_      =0x7FFF;
const unsigned long config_rtc_freq_prec _weak_     =0;
/* JPEG SNR THRESHOLD */
const unsigned int  config_jpeg_threshold  _weak_   =0x1E;
/* end config_weak.c  */

