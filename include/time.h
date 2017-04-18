/**
 *  @file   time.h
 *  @brief  POSIX like time function
 *  $Id: time.h,v 1.8 2014/07/17 02:32:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#ifndef __TIME_H
#define __TIME_H


typedef long  time_t;

struct tm {
    int     tm_sec;     /* seconds */
    int     tm_min;     /* minutes */
    int     tm_hour;    /* hours */
    int     tm_mday;    /* day of the month */
    int     tm_mon;     /* month */
    int     tm_year;    /* year */
    int     tm_wday;    /* day of the week */
    int     tm_yday;    /* day in the year */
    int     tm_isdst;   /* daylight saving time */
    long    tm_gmtoff;
    const char * tm_zone;
};

#ifdef __mips
struct timeval {
    long        tv_sec;     /* seconds */
    long        tv_usec;    /* microseconds */
};
#else
#include </usr/include/sys/time.h>
#endif

extern struct tm *localtime(const time_t *timep);
extern struct tm *localtime_r(const time_t *timep, struct tm *res);
extern time_t mktime(struct tm *tm);
extern struct tm *madjtime(struct tm *tm, struct tm *res);
time_t time(void);

unsigned long read_msec(void);
unsigned long read_usec(void);
unsigned long read_sec(void);

/**
 * @func    rtc_get_time
 * @brief   get real time
 * @param   time        get real time data structure
 * @return  0 success, or return negative error code.
 */
void rtc_get_time(struct tm *time);

/**
 * @func    rtc_set_time
 * @brief   set real time
 * @param   time        set real time data structure
 * @return  0 success, or return negative error code.
 */
int rtc_set_time(struct tm *time);

/**
 * @func    rtc_reset
 * @brief   reset RTC
 * @param 
 * @return  0 success, or return negative error code.
 */
int rtc_reset(void);

/**
 * @func    rtc_get_alarm_time
 * @brief   get alarm time
 * @param   time        get alarm time data structure
 * @return  none
 */
void rtc_get_alarm_time(struct tm *time);

/**
 * @func    rtc_set_alarm_time
 * @brief   set alarm time
 * @param   time        set alarm time data structure
 * @return  0 success, or return negative error code.
 */
int rtc_set_alarm_time(struct tm *time, struct tm *al_time);

/**
 * @func    rtc_set_alarm
 * @brief   enable alarm time
 * @param   enable    enable alarm
 * @return  none
 */
void rtc_set_alarm(int enable);


#endif /* __TIME_H */
