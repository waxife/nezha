/**
 *  @file   mktime.c
 *  @brief  POSIX like mktime function
 *  $Id: mktime.c,v 1.5 2014/02/12 05:21:51 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

#define _SEC_IN_MINUTE	60
#define _SEC_IN_HOUR	3600
#define _SEC_IN_DAY	    86400

static const int DAYS_IN_MONTH[12] =
    {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};

#define _DAYS_IN_MONTH(x) ((x == 1) ? days_in_feb : DAYS_IN_MONTH[x])

static const int _DAYS_BEFORE_MONTH[12] =
    {0, 31, 59, 90, 120, 151, 181, 212, 243, 273, 304, 334};

#define isleap(y) ((((y + 1970) % 4) == 0 && ((y + 1970) % 100) != 0) || ((y + 1970) % 400) == 0)

#define _DAYS_IN_YEAR(year) (isleap(year)  ? 366 : 365)

/* validate the [time] */
static void ValidateTM(struct tm *time);


time_t mktime (struct tm *tim_p)
{
    time_t tim = 0;
    long days = 0;
    int year;

    /* validate [time_p] structure so that it would match
        in tm_min(0~59), tm_mday(1~31), etc */
    ValidateTM(tim_p);

    /* compute hours, minutes, seconds */
    tim += tim_p->tm_sec + (tim_p->tm_min * _SEC_IN_MINUTE) +
        (tim_p->tm_hour * _SEC_IN_HOUR);

    /* compute days in year */
    days += tim_p->tm_mday - 1;
    days += _DAYS_BEFORE_MONTH[tim_p->tm_mon];
    if (tim_p->tm_mon > 1 && _DAYS_IN_YEAR (tim_p->tm_year) == 366)
        days++;

    /* compute day of the year */
    tim_p->tm_yday = days;

    if (tim_p->tm_year + 1970 > 2107)
        return (time_t) 0x7fffffff;
    if (tim_p->tm_year + 1970 < 1972)
        return (time_t) -0x7fffffff;

    /* compute days in other years */
    if (tim_p->tm_year > 70) {
        for (year = 70; year < tim_p->tm_year; year++)
	        days += _DAYS_IN_YEAR (year);
    } else if (tim_p->tm_year < 70) {
        for (year = 69; year > tim_p->tm_year; year--)
	        days -= _DAYS_IN_YEAR (year);
        days -= _DAYS_IN_YEAR (year);
    }

    /* compute day of the week */
    if ((tim_p->tm_wday = (days + 4) % 7) < 0)
        tim_p->tm_wday += 7;

    /* compute total seconds */
    tim += (days * _SEC_IN_DAY);

    return tim;
}


static void ValidateTM(struct tm *tim_p)
{
    div_t res;
    int days_in_feb = 28;

    /* calculate time & date to account for out of range values */
    if (tim_p->tm_sec < 0 || tim_p->tm_sec > 59) {
        res = div (tim_p->tm_sec, 60);
        tim_p->tm_min += res.quot;
        if ((tim_p->tm_sec = res.rem) < 0)
	        tim_p->tm_sec += 60;
    }

    if (tim_p->tm_min < 0 || tim_p->tm_min > 59) {
        res = div (tim_p->tm_min, 60);
        tim_p->tm_hour += res.quot;
        if ((tim_p->tm_min = res.rem) < 0)
	        tim_p->tm_min += 60;
    }

    if (tim_p->tm_hour < 0 || tim_p->tm_hour > 23) {
        res = div (tim_p->tm_hour, 24);
        tim_p->tm_mday += res.quot;
        if ((tim_p->tm_hour = res.rem) < 0)
	        tim_p->tm_hour += 24;
    }

    if (tim_p->tm_mon > 11) {
        res = div (tim_p->tm_mon, 12);
        tim_p->tm_year += res.quot;
        if ((tim_p->tm_mon = res.rem) < 0)
	        tim_p->tm_mon += 12;
    }

    if (_DAYS_IN_YEAR (tim_p->tm_year) == 366)
        days_in_feb = 29;

    if (tim_p->tm_mday < 0) {
        while (tim_p->tm_mday < 0) {
	        tim_p->tm_mday += _DAYS_IN_MONTH (tim_p->tm_mon);
            if (--tim_p->tm_mon == -1) {
	            tim_p->tm_year--;
	            tim_p->tm_mon = 12;
	            days_in_feb = ((_DAYS_IN_YEAR (tim_p->tm_year) == 366) ? 29 : 28);
	        }
	    }
    } else {
        while (tim_p->tm_mday > _DAYS_IN_MONTH (tim_p->tm_mon)) {
	        tim_p->tm_mday -= _DAYS_IN_MONTH (tim_p->tm_mon);
	        if (++tim_p->tm_mon == 12) {
	            tim_p->tm_year++;
	            tim_p->tm_mon = 0;
	            days_in_feb = ((_DAYS_IN_YEAR (tim_p->tm_year) == 366) ?  29 : 28);
	        }
	    }
    }
}

