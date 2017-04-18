/*
 *  @file	rtc.c
 *  @brief	Cheetah RTC driver
 *	$Id: rtc.c,v 1.9 2015/11/12 09:44:25 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.9 $
 *
 *	Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *			2011/04/22  C.N.Yu  New file
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <debug.h>
#include <io.h>
#include <time.h>
#include <config.h>

/* related registers move to stand-alone RTC module */
#define REG_RTC_BASE            (0xB0800000)
#define REG_RTC_SEC             (REG_RTC_BASE + 0x000)
#define REG_RTC_SEC_ALARM       (REG_RTC_BASE + 0x004)
#define REG_RTC_MIN             (REG_RTC_BASE + 0x008)
#define REG_RTC_MIN_ALARM       (REG_RTC_BASE + 0x00C)
#define REG_RTC_HOUR            (REG_RTC_BASE + 0x010)
#define REG_RTC_HOUR_ALARM      (REG_RTC_BASE + 0x014)
#define REG_RTC_DAY             (REG_RTC_BASE + 0x018)
#define REG_RTC_DATE            (REG_RTC_BASE + 0x01C)
#define REG_RTC_MONTH           (REG_RTC_BASE + 0x020)
#define REG_RTC_YEAR            (REG_RTC_BASE + 0x024)
#define REG_RTC_ALARM_CTL       (REG_RTC_BASE + 0x028)
#define REG_RTC_INTR            (REG_RTC_BASE + 0x02C)
#define REG_RTC_INTR_FLAG       (REG_RTC_BASE + 0x030)
#define REG_RTC_RTC_FREQ        (REG_RTC_BASE + 0x034)
#define REG_RTC_RTC_FREQ_PREC   (REG_RTC_BASE + 0x038)

#define REG_RTC_PROB_CTL        (REG_RTC_BASE + 0x040)
#define REG_RTC_PROB_FREQ       (REG_RTC_BASE + 0x044)
#define REG_RTC_PROB_SEC        (REG_RTC_BASE + 0x048)

#if 0
void _rtc_get_time(struct rtc_time *time)
{
    while (1) {
        time->sec = readl(REG_RTC_SEC) & 0x3F;
        time->min = readl(REG_RTC_MIN) & 0x3F;
        time->hour = readl(REG_RTC_HOUR) & 0x1F;
        time->date = readl(REG_RTC_DATE) & 0x1F;
        time->mon = readl(REG_RTC_MONTH) & 0x0F;
        time->year = (readl(REG_RTC_YEAR) & 0x7F) + 2000;
        if (time->sec == 59 && time->sec != (readl(REG_RTC_SEC) & 0x3F) )
            continue;
        else
            break;
    }
}

int _rtc_set_time(struct rtc_time *time)
{
    int i = 0, time_ok = 0;

    rtc_reset();

    do {
        struct rtc_time cur_time;
        int dummy;

        writel(readl(REG_RTC_INTR) | 0x80, REG_RTC_INTR);
        writel(time->year - 2000, REG_RTC_YEAR);
        writel(time->year - 2000, REG_RTC_YEAR);
        writel(time->mon, REG_RTC_MONTH);
        writel(time->mon, REG_RTC_MONTH);
        writel(time->date, REG_RTC_DATE);
        writel(time->date, REG_RTC_DATE);
        writel(time->wday, REG_RTC_DAY);
        writel(time->wday, REG_RTC_DAY);
        writel(time->hour, REG_RTC_HOUR);
        writel(time->hour, REG_RTC_HOUR);
        writel(time->min, REG_RTC_MIN);
        writel(time->min, REG_RTC_MIN);
        writel(time->sec, REG_RTC_SEC);
        writel(time->sec, REG_RTC_SEC);
        writel(readl(REG_RTC_INTR) & ~0x80, REG_RTC_INTR);
    
        /* work-around for a T373 RTC bug. 2000 times from APB(30Mhz) and
           RTC clock(32.768Khz) */
        for (dummy = 0; dummy < 2000; dummy++)
            readl(REG_RTC_SEC);
    
        i++;
        rtc_get_time(&cur_time);
   
        if (time->year == cur_time.year && time->mon == cur_time.mon &&
            time->date == cur_time.date && time->hour == cur_time.hour &&
            time->min == cur_time.min && time->sec == cur_time.sec) {
            time_ok = 1;
        } else {
            dbg(0, "set : %04d/%02d/%02d %02d:%02d:%02d\n",
                time->year, time->mon, time->date, time->hour, time->min, time->sec);
            dbg(0, "get : %04d/%02d/%02d %02d:%02d:%02d\n",
                cur_time.year, cur_time.mon, cur_time.date, cur_time.hour, cur_time.min, cur_time.sec);
        }
    } while (i < 10 && !time_ok);

    if (time_ok)
        return 0;
    else
        return -1;
}
#endif

int rtc_reset(void)
{
    printf("Reset RTC\n");

    writel(0x01, SYSMGM_BASE);
    writel(config_rtc_freq_int, REG_RTC_RTC_FREQ);
    writel(config_rtc_freq_prec, REG_RTC_RTC_FREQ_PREC);

    return 0;
}



static void _rtc_get_time(struct tm *time)
{
    int i;
 
    for (i = 0; i < 10; i++) {
        time->tm_sec = readl(REG_RTC_SEC) & 0x3F;
        time->tm_min = readl(REG_RTC_MIN) & 0x3F;
        time->tm_hour = readl(REG_RTC_HOUR) & 0x1F;
        time->tm_mday = readl(REG_RTC_DATE) & 0x1F;
        time->tm_wday = (readl(REG_RTC_DAY) & 0x7);
        time->tm_mon = (readl(REG_RTC_MONTH) & 0x0F) - 1;
        time->tm_year = (readl(REG_RTC_YEAR) & 0x7F) + 2000 - 1970;
        if (time->tm_sec == 59 && time->tm_sec != (readl(REG_RTC_SEC) & 0x3F) )
            continue;
        else
            break;
    }

    time->tm_wday = (time->tm_wday == 7) ? 0 : time->tm_wday;

}

static void detect_rtc_alive(void)
{
    static char is_alive = 0;
    unsigned long l, freq = 0, freq2 = 0;
    int i;
    struct tm time;

    if (is_alive)
        return;

    /* probe fine tune frequence integer */
    freq = readl(REG_RTC_RTC_FREQ);
    if (freq != config_rtc_freq_int) {
        printf("reset rtc, read RTC_Freq 0x%lX not 0x%lX\n", freq, config_rtc_freq_int);
        goto RESET_EXIT;
    }

    /* probe fine tune frequence precision */
    freq = readl(REG_RTC_RTC_FREQ_PREC);
    if (freq != config_rtc_freq_prec) {
        printf("reset rtc, read RTC_Precision 0x%lX not 0x%lX\n", freq, config_rtc_freq_prec);
        goto RESET_EXIT;
    }

    /* probe frequency */
    writel(7, REG_RTC_PROB_CTL); 
    for (i = 0; i < 100; i++) {
        l = readl(REG_RTC_PROB_CTL);
        if ((l & (1 << 3)) == 0) { /* not busy */
            freq = readl(REG_RTC_PROB_FREQ);
            break;
        }
        usleep(10);
    }
    
    usleep(100);

    writel(7, REG_RTC_PROB_CTL);
    for (i = 0; i < 100; i++) {
        l = readl(REG_RTC_PROB_CTL);
        if ((l & (1 << 3)) == 0) { /* not busy */
            freq2 = readl(REG_RTC_PROB_FREQ);
            break;
        }
        usleep(10);
    }                

    writel(0, REG_RTC_PROB_CTL);

    if (freq == freq2) {
        printf("reset rtc f= %ld f2 = %ld\n", freq, freq2);
        goto RESET_EXIT;
    }
    
    goto EXIT;

RESET_EXIT:
    _rtc_get_time(&time);
    if (rtc_set_time(&time) < 0) {
        rtc_reset();
        usleep(1500);
        _rtc_get_time(&time);
        rtc_set_time(&time);
    }

    return;
EXIT:
    is_alive = 1;
}


void rtc_get_time(struct tm *time)
{
    detect_rtc_alive();
    _rtc_get_time(time);
}

int rtc_set_time(struct tm *time)
{
    int i = 0;
    time_t t;
    struct tm *tm, cur_time;
    unsigned intr;
    int day;

    if((time->tm_sec>59)||(time->tm_sec<0)){
        ERROR("time->tm_sec=%d > 59\n", time->tm_sec);
        return -1;
    }
    if((time->tm_min>59)||(time->tm_min<0)){
        ERROR("time->tm_min=%d > 59\n", time->tm_min);
        return -1;
    }
    if((time->tm_hour>23)||(time->tm_hour<0)){
        ERROR("time->tm_hour=%d > 23\n", time->tm_hour);
        return -1;
    }
    t = mktime(time);
    tm = localtime(&t);

    if (tm->tm_sec != time->tm_sec ||
        tm->tm_min != time->tm_min ||
        tm->tm_hour != time->tm_hour ||
        tm->tm_mday != time->tm_mday ||
        tm->tm_mon != time->tm_mon ||
        tm->tm_year != time->tm_year) {
        ERROR("Invalied arugment\n");
        printf("%04d, %04d\n", tm->tm_year, time->tm_year);
        return -1;
    }

    if (tm->tm_year + 1970 < 2000) {
        ERROR("rtc year must >= 2000\n");
        return -1;
    }

    time->tm_wday = tm->tm_wday;
    //day = (tm->tm_wday == 0) ? 7 : tm->tm_wday;
    day = tm->tm_wday;

    rtc_reset();

    for (i = 0; i < 100; i++) {

        writel(readl(REG_RTC_INTR) | 0x80, REG_RTC_INTR);
        intr = readl(REG_RTC_INTR);
        writel(time->tm_year + 1970 - 2000, REG_RTC_YEAR);
        writel(time->tm_mon + 1, REG_RTC_MONTH);
        writel(time->tm_mday, REG_RTC_DATE);
        writel(day, REG_RTC_DAY);
        writel(time->tm_hour, REG_RTC_HOUR);
        writel(time->tm_min, REG_RTC_MIN);
        writel(time->tm_sec, REG_RTC_SEC);
        writel(readl(REG_RTC_INTR) & ~0x80, REG_RTC_INTR);
        cur_time.tm_year = (readl(REG_RTC_YEAR) & 0x7F) + 2000 - 1970;        
        cur_time.tm_mon = (readl(REG_RTC_MONTH) & 0x0F) - 1;        
        cur_time.tm_mday = readl(REG_RTC_DATE) & 0x1F;        
        cur_time.tm_wday = (readl(REG_RTC_DAY) & 0x7) % 7;        
        cur_time.tm_hour = readl(REG_RTC_HOUR) & 0x1F;        
        cur_time.tm_min = readl(REG_RTC_MIN) & 0x3F;        
        cur_time.tm_sec = readl(REG_RTC_SEC) & 0x3F;        

        if (time->tm_year == cur_time.tm_year && time->tm_mon == cur_time.tm_mon &&
            time->tm_mday == cur_time.tm_mday && day == cur_time.tm_wday && 
            time->tm_hour == cur_time.tm_hour &&
            time->tm_min == cur_time.tm_min && time->tm_sec == cur_time.tm_sec) {
            return 0;
        }
        usleep(1500);
    }

    return -1;
}

void rtc_get_alarm_time(struct tm *time)
{
	time->tm_sec = readl(REG_RTC_SEC_ALARM) & 0x3F;
	time->tm_min = readl(REG_RTC_MIN_ALARM) & 0x3F;
	time->tm_hour = readl(REG_RTC_HOUR_ALARM) & 0x1F;
    time->tm_wday = readl(REG_RTC_ALARM_CTL) & 0x7F;
}

int rtc_set_alarm_time(struct tm *time, struct tm *al_time)
{
    int i = 0;
    time_t t;
    struct tm *tm, cur_time, alarm_time;
    unsigned intr;
    int day;
    int alarm_ctl = 0;

    t = mktime(time);
    tm = localtime(&t);
    if (tm->tm_sec != time->tm_sec ||
        tm->tm_min != time->tm_min ||
        tm->tm_hour != time->tm_hour ||
        tm->tm_mday != time->tm_mday ||
        tm->tm_mon != time->tm_mon ||
        tm->tm_year != time->tm_year) {
        ERROR("Invalied arugment\n");
        return -1;
    }

    if (tm->tm_year + 1970 < 2000) {
        ERROR("rtc year must >= 2000\n");
        return -1;
    }

    time->tm_wday = tm->tm_wday;
    day = (tm->tm_wday == 0) ? 7 : tm->tm_wday;

    alarm_ctl = (al_time->tm_wday & 0x7F) | (readl(REG_RTC_ALARM_CTL) & 0x80);

    rtc_reset();

    for (i = 0; i < 100; i++) {
    	/* set 'set' bit to 1
    	 * permits to set clock and alarm (Reg 000 ~ 0024h) */
        writel(readl(REG_RTC_INTR) | 0x80, REG_RTC_INTR);
        intr = readl(REG_RTC_INTR);
        /* set RTC time,
         * must set RTC and Alarm at the same time,
         * maybe this condition is normal,
         * or maybe is ... bug ... */
        writel(time->tm_year + 1970 - 2000, REG_RTC_YEAR);
        cur_time.tm_year = (readl(REG_RTC_YEAR) & 0x7F) + 2000 - 1970;
        writel(time->tm_mon + 1, REG_RTC_MONTH);
        cur_time.tm_mon = (readl(REG_RTC_MONTH) & 0x0F) - 1;
        writel(time->tm_mday, REG_RTC_DATE);
        cur_time.tm_mday = readl(REG_RTC_DATE) & 0x1F;
        writel(day, REG_RTC_DAY);
        cur_time.tm_wday = (readl(REG_RTC_DAY) & 0x7) % 7;
        writel(time->tm_hour, REG_RTC_HOUR);
        cur_time.tm_hour = readl(REG_RTC_HOUR) & 0x1F;
        writel(time->tm_min, REG_RTC_MIN);
        cur_time.tm_min = readl(REG_RTC_MIN) & 0x3F;
        writel(time->tm_sec, REG_RTC_SEC);
        cur_time.tm_sec = readl(REG_RTC_SEC) & 0x3F;
        /* set Alarm time */
        writel(al_time->tm_hour, REG_RTC_HOUR_ALARM);
        alarm_time.tm_hour = readl(REG_RTC_HOUR_ALARM) & 0x1F;
        writel(al_time->tm_min, REG_RTC_MIN_ALARM);
        alarm_time.tm_min = readl(REG_RTC_MIN_ALARM) & 0x3F;
        writel(al_time->tm_sec, REG_RTC_SEC_ALARM);
        alarm_time.tm_sec = readl(REG_RTC_SEC_ALARM) & 0x3F;
        writel(alarm_ctl, REG_RTC_ALARM_CTL);
        alarm_time.tm_wday = readl(REG_RTC_ALARM_CTL) & 0x7F;

    	/* set 'set' bit to 0, and clock and alarm values are update */
        writel(readl(REG_RTC_INTR) & (~0x80), REG_RTC_INTR);

        if (time->tm_year == cur_time.tm_year && time->tm_mon == cur_time.tm_mon &&
            time->tm_mday == cur_time.tm_mday && day == cur_time.tm_wday &&
            time->tm_hour == cur_time.tm_hour &&
            time->tm_min == cur_time.tm_min && time->tm_sec == cur_time.tm_sec) {
        	if (al_time->tm_hour == alarm_time.tm_hour && al_time->tm_min == alarm_time.tm_min &&
        		al_time->tm_sec == alarm_time.tm_sec && al_time->tm_wday == alarm_time.tm_wday) {
        		return 0;
			}
        }
    }

    return -1;
}

void rtc_set_alarm(int enable)
{
    if (enable)
        writel(readl(REG_RTC_ALARM_CTL) | 0x80, REG_RTC_ALARM_CTL);
    else
        writel(readl(REG_RTC_ALARM_CTL) & ~0x80, REG_RTC_ALARM_CTL);
}
