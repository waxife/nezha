/**
 *  @file   time.c
 *  @brief  get time in seconds
 *  $Id: time.c,v 1.3 2014/07/17 02:32:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#include <time.h>
#include <stdlib.h>
#include <debug.h>
#include <sys.h>
#include <unistd.h>

time_t
time (void)
{
    struct tm sys_tm;
    rtc_get_time(&sys_tm);

    return mktime(&sys_tm);
}

u_long read_msec(void)
{
    u_long t = read_c0_count32();

    return (t/MSEC_INTERVAL);
}

u_long read_usec(void)
{
    u_long t = read_c0_count32();
    return (t/USEC_INTERVAL);
}

u_long read_sec(void)
{
    unsigned long long t = read_cpu_count();

    return hz2sec(t);
}

