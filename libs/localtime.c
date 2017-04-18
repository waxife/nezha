/**
 *  @file   localtime.c
 *  @brief  POSIX like localtime function
 *  $Id: localtime.c,v 1.7 2014/07/16 02:35:56 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */


#include <time.h>
#include <stdio.h>

#define SECSPERMIN	60L
#define MINSPERHOUR	60L
#define HOURSPERDAY	24L
#define SECSPERHOUR	(SECSPERMIN * MINSPERHOUR)
#define SECSPERDAY	(SECSPERHOUR * HOURSPERDAY)
#define DAYSPERWEEK	7
#define MONSPERYEAR	12

#define YEAR_BASE	1970
#define EPOCH_YEAR  2040
#define EPOCH_WDAY  0

#define isleap(y) ((((y) % 4) == 0 && ((y) % 100) != 0) || ((y) % 400) == 0)

static const int mon_lengths[2][MONSPERYEAR] = {
  {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31},
  {31, 29, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31}
} ;

static const int year_lengths[2] = {
  365,
  366
} ;


/**************************************************************
  FUNCTION: adjtime
 **************************************************************/
struct tm *madjtime(struct tm *tm, struct tm *res)
{
    int year, mday;
    time_t t;

    year = tm->tm_year + 1970;

    res->tm_sec = (tm->tm_sec + 60) % 60;
    res->tm_min = (tm->tm_min + 60) % 60;
    res->tm_hour = (tm->tm_hour + 24) % 24;
    
    if (year < 2000) year = 2099;
    if (year > 2099) year = 2000;

    res->tm_mon = (tm->tm_mon +12) % 12;
    
    if (isleap(year)) {
        mday = mon_lengths[1][res->tm_mon];
    } else {
        mday = mon_lengths[0][res->tm_mon];
    }
    
    res->tm_mday = (((tm->tm_mday - 1) + mday) % mday) + 1;

    res->tm_year = year - 1970;

    t = mktime(res);    
    return localtime_r(&t, res);
}

/**************************************************************
	FUNCTION : localtime()
**************************************************************/

struct tm *localtime_r(const time_t *tim_p, struct tm *res)
{
    long days, rem;
    int y;
    int yleap;
    const int *ip;

    days = ((long) *tim_p) / SECSPERDAY;
    rem = ((long) *tim_p) % SECSPERDAY;
    while (rem < 0) {
        rem += SECSPERDAY;
        --days;
    }

    while (rem >= SECSPERDAY) {
        rem -= SECSPERDAY;
        ++days;
    }
 
    /* compute hour, min, and sec */  
    res->tm_hour = (int) (rem / SECSPERHOUR);
    rem %= SECSPERHOUR;
    res->tm_min = (int) (rem / SECSPERMIN);
    res->tm_sec = (int) (rem % SECSPERMIN);

    /* compute day of week */
    if ((res->tm_wday = ((EPOCH_WDAY + days) % DAYSPERWEEK)) < 0)
        res->tm_wday += DAYSPERWEEK;

    /* compute year & day of year */
    y = EPOCH_YEAR;
    if (days >= 0) {
        for (;;) {
	        yleap = isleap(y);
            if (days < year_lengths[yleap])
	            break;
	        y++;
	        days -= year_lengths[yleap];
	    }
    } else {
        do {
	        --y;
	        yleap = isleap(y);
	        days += year_lengths[yleap];
	    } while (days < 0);
    }

    res->tm_year = y - YEAR_BASE;
    res->tm_yday = days;
    ip = mon_lengths[yleap];
    for (res->tm_mon = 0; days >= ip[res->tm_mon]; ++res->tm_mon)
        days -= ip[res->tm_mon];
    res->tm_mday = days + 1;

    /* set daylight saving time flag */
    res->tm_isdst = -1;

    return (res);
}

struct tm *localtime(const time_t *tim_p)
{
    static struct tm now;

    return localtime_r(tim_p, &now);
}




