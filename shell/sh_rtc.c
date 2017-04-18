/**
 *  @file   sh_rtc.c
 *  @brief  RTC test program
 *  $Id: sh_rtc.c,v 1.10 2014/07/16 02:35:56 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.10 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/04/22  C.N.Yu    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <serial.h>
#include <ctype.h>
#include <debug.h>
#include <time.h>
#include <irq.h>
#include <interrupt.h>

/* related registers move to stand-alone RTC module */
#define REG_RTC_BASE        (0xB0800000)
#define REG_RTC_SEC         (REG_RTC_BASE + 0x000)
#define REG_RTC_SEC_ALARM   (REG_RTC_BASE + 0x004)
#define REG_RTC_MIN         (REG_RTC_BASE + 0x008)
#define REG_RTC_MIN_ALARM   (REG_RTC_BASE + 0x00C)
#define REG_RTC_HOUR        (REG_RTC_BASE + 0x010)
#define REG_RTC_HOUR_ALARM  (REG_RTC_BASE + 0x014)
#define REG_RTC_DAY         (REG_RTC_BASE + 0x018)
#define REG_RTC_DATE        (REG_RTC_BASE + 0x01C)
#define REG_RTC_MONTH       (REG_RTC_BASE + 0x020)
#define REG_RTC_YEAR        (REG_RTC_BASE + 0x024)
#define REG_RTC_ALARM_CTL   (REG_RTC_BASE + 0x028)
#define REG_RTC_INTR        (REG_RTC_BASE + 0x02C)
#define REG_RTC_INTR_FLAG   (REG_RTC_BASE + 0x030)
#define REG_RTC_RTC_FREQ    (REG_RTC_BASE + 0x034)

#define REG_RTC_PROB_CTL    (REG_RTC_BASE + 0x040)
#define REG_RTC_PROB_FREQ   (REG_RTC_BASE + 0x044)
#define REG_RTC_PROB_SEC    (REG_RTC_BASE + 0x048)

char get_time_usage[] = "get_time";
char set_time_usage[] = "set_time [year] [month] [date] [hour] [min] [sec]";
char rtc_test_usage[] = "rtc_test";

char get_alarmtime_usage[] = "get_alarmtime";
char set_alarmtime_usage[] = "set_alarmtime [hour] [min] [sec] [week days(0: everyday, bit 6~0 : Mon~Sun)]";
char set_alarm_usage[] = "set_alarm";

command_init(sh_get_time, "get_time", get_time_usage);
command_init(sh_set_time, "set_time", set_time_usage);
command_init(sh_rtc_reset, "rtc_reset", "rtc_reset");
command_init(sh_rtc_test, "rtc_test", rtc_test_usage);
command_init(sh_time, "time", "time");
command_init(sh_rtc_tt, "rtc_tt", "rtc_tt");
command_init(sh_localtime_tt, "localtime_tt", "localtime_tt");
command_init(sh_adjdate_tt, "adjdate_tt", "adjdate_tt");

command_init(sh_get_alarmtime, "get_alarmtime", get_alarmtime_usage);
command_init(sh_set_alarmtime, "set_alarmtime", set_alarmtime_usage);
command_init(sh_set_alarm, "set_alarm", set_alarm_usage);

static const char *wday[] = { "Sun", "Mon", "Tue", "Wed", "Thr", "Fri", "Sat" };

static void printtime(struct tm *etm, struct tm *tm, time_t t)
{
    printf("Exptect %04d/%02d/%02d(%s) %02d:%02d:%02d\n", 
            etm->tm_year + 1970, etm->tm_mon + 1, etm->tm_mday, wday[etm->tm_wday], 
            etm->tm_hour, etm->tm_min, etm->tm_sec);
    printf("        %04d/%02d/%02d(%s) %02d:%02d:%02d (%ld)\n", 
            tm->tm_year + 1970, tm->tm_mon + 1, tm->tm_mday, wday[tm->tm_wday], 
            tm->tm_hour, tm->tm_min, tm->tm_sec, t);
}

static void chktime(int year, int mon, int day)
{
    struct tm time;
    struct tm *tm;
    time_t t;

    time.tm_year = year - 1970;
    time.tm_mon  = mon - 1;
    time.tm_mday = day;
    time.tm_hour = time.tm_min = time.tm_sec = 0;

    t = mktime(&time);
    tm = localtime(&t);
    
    if (time.tm_year != tm->tm_year || 
        time.tm_mon != tm->tm_mon   ||
        time.tm_mday != tm->tm_mday) {
        printtime(&time, tm, t);
    } else {
       printf(" (Pass) %04d/%02d/%02d(%s) %02d:%02d:%02d (%ld)\n", 
            tm->tm_year + 1970, tm->tm_mon + 1, tm->tm_mday, wday[tm->tm_wday], 
            tm->tm_hour, tm->tm_min, tm->tm_sec, t);
    }   


}

static int sh_localtime_tt(int argc, char **argv, void **result, void *sys_ctx)
{
    chktime(1972, 1, 1);
    chktime(2040, 1, 1);
    chktime(2107, 12, 31);
    chktime(2106, 12, 31);
    chktime(1975, 2, 28);
    chktime(1978, 2, 28);
    chktime(1979, 2, 28);
    chktime(1980, 1, 1);
    chktime(1980, 2, 28);
    chktime(1980, 2, 29);
    chktime(1980, 3, 1);
    chktime(1981, 2, 28);
    chktime(1981, 2, 29);
    chktime(1981, 3, 1);
    chktime(2000, 2, 28);
    chktime(2000, 2, 29);
    chktime(2000, 3, 1);
    chktime(2014, 2, 11);
    chktime(2040, 2, 11);
    chktime(2060, 2, 28);
    chktime(2060, 2, 29);
    chktime(2060, 3, 1);
    chktime(2100, 2, 28);
    chktime(2100, 2, 29);
    chktime(2100, 3, 1);

    printf("\nFollowing is error case\n");
    chktime(1970, 2, 28);
    chktime(1970, 1, 1);
    chktime(1971, 1, 1);
    chktime(1971, 12, 31);
    chktime(2108, 1, 1);
    chktime(2109, 12, 31);


    return 0;
}


static void adjdate(int year, int mon, int day, int hour, int min, int sec)
{
    struct tm time;

    time.tm_year = year - 1970;
    time.tm_mon = mon - 1;
    time.tm_mday = day;
    time.tm_hour = hour;
    time.tm_min = min;
    time.tm_sec = sec;

    madjtime(&time, &time);

    printf("adj date %04d/%02d/%02d (---) %02d:%02d:%02d\n", year, mon, day, hour, min, sec);
    printf("      to %04d/%02d/%02d (%s) %02d:%02d:%02d\n\n", 
        time.tm_year + 1970, time.tm_mon + 1, time.tm_mday, wday[time.tm_wday], 
        time.tm_hour, time.tm_min, time.tm_sec);
}

static int sh_adjdate_tt(int argc, char **argv, void **result, void *sys_ctx)
{
    adjdate(2000, 1, 1, 23, 59, 59);
    adjdate(2000, 1, 1, 23, 59, 60);
    adjdate(2000, 1, 1, 23, 59, -1);
    adjdate(2000, 1, 1, 23, 60, 59);
    adjdate(2000, 1, 1, 23, -1, 59);
    adjdate(2000, 1, 1, 24, 59, 59);
    adjdate(2000, 1, 1, -1, 59, 59);
    adjdate(2000, 1, 0, 23, 59, 59);
    adjdate(2000, 1, 32, 23, 59, 59);
    adjdate(2000, 0, 1, 23, 59, 59);
    adjdate(2000, 13, 1, 23, 59, 59);
    adjdate(1999, 1, 1, 23, 59, 59);
    adjdate(2100, 1, 1, 23, 59, 59);


    adjdate(2000, 2, 0, 23, 59, 59);
    adjdate(2000, 2, 29, 23, 59, 59);
    adjdate(2000, 2, 30, 23, 59, 59);

    adjdate(2000, 3, 0, 23, 59, 59);
    adjdate(2000, 3, 32, 23, 59, 59);

    adjdate(2014, 2, 0, 23, 59, 59);
    adjdate(2014, 2, 29, 23, 59, 59);

    adjdate(2014, 9, 0, 23, 59, 59);
    adjdate(2014, 9, 31, 23, 59, 59);

    adjdate(2014, 12, 0, 23, 59, 59);
    adjdate(2014, 12, 32, 23, 59, 59);

    return 0;
}


static int sh_rtc_tt(int argc, char **argv, void **result, void *sys_ctx)
{
    time_t t;
    int i;
    int rc;
    int err = 0;
    struct tm *tm;

    t = time();

    for (i = 0; i < 100000; i++) {
        t += 375;

        rtc_reset();
        tm = localtime(&t);
        rc = rtc_set_time(tm);

        if (i% 100 == 0) {
            printf("[%d] pass %d err\n", i, err);
        }

    }

    return 0;
}




static int sh_rtc_reset(int argc, char **argv, void **result, void *sys_ctx)
{
    rtc_reset();
    return 0;
}
static int sh_time(int argc, char **argv, void **result, void *sys_ctx)
{
    time_t t;
    struct tm *tm;

    t = time();
    
    tm = localtime(&t);

    printf("%04d/%02d/%02d(%s) %02d:%02d:%02d (%ld)\n", 
            tm->tm_year + 1970, tm->tm_mon + 1, tm->tm_mday, wday[tm->tm_wday], 
            tm->tm_hour, tm->tm_min, tm->tm_sec, t);
    return 0;
}

static int sh_get_time(int argc, char **argv, void **result, void *sys_ctx)
{
    struct tm time;

    rtc_get_time(&time);

    printf("%04d/%02d/%02d(%s) %02d:%02d:%02d\n", 
            time.tm_year + 1970, time.tm_mon + 1, time.tm_mday, wday[time.tm_wday], 
            time.tm_hour, time.tm_min, time.tm_sec);

    return 0;
}

static int sh_set_time(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;
    struct tm time;
    struct tm *tm;
    time_t t;

    if (argc < 7) {
        printf("Invaild argument.\n");
        printf("Usage: %s\n", set_time_usage);
        rc =  -1;
        goto EXIT;
    }

    if (atoi(argv[1]) < 2000) {
        printf("Year must >= 2000\n");
        goto EXIT;
    }

    time.tm_year = atoi(argv[1]) - 1970;
    time.tm_mon  = atoi(argv[2]) - 1;
    time.tm_mday = atoi(argv[3]);
    time.tm_hour = atoi(argv[4]);
    time.tm_min = atoi(argv[5]);
    time.tm_sec = atoi(argv[6]);

    t = mktime(&time);
    tm = localtime(&t);

    printf("Set time as %04d/%02d/%02d %02d:%02d:%02d\n", 
        tm->tm_year + 1970, tm->tm_mon + 1, tm->tm_mday,
        tm->tm_hour, tm->tm_min, tm->tm_sec);     
    rc = rtc_set_time(tm);
    if (rc < 0) {
        printf("Set time failed - %d\n", rc);
        goto EXIT;
    }

EXIT:
    return rc;
}

static int sh_rtc_test(int argc, char **argv, void **result, void *sys_ctx)
{
    struct tm time;
    unsigned int cnt = 0;
    int ch = 0;

    while (1) {
        writel(cnt, 0xB080010C);
        if (readl(0xB080010C) != cnt) {
            printf("RTC dead (%d)\n", cnt);
            break;
        }

        cnt++;

        ch = getb2();
        if (ch == 'p') {
            printf("RTC alive - %d\n", cnt);
            rtc_get_time(&time);
            printf("%04d/%02d/%02d %02d:%02d:%02d\n", 
                time.tm_year + 1970, time.tm_mon + 1, time.tm_mday, 
                time.tm_hour, time.tm_min, time.tm_sec);
        } else if (ch == 'q') {
            break;
        }
    }

    printf("End of RTC test\n");

    return 0;
}

static int sh_get_alarmtime(int argc, char **argv, void **result, void *sys_ctx)
{
	int i = 0;
	struct tm time;

    rtc_get_alarm_time(&time);

    printf("Alarm time %02d:%02d:%02d", time.tm_hour, time.tm_min, time.tm_sec);

    for(i = 0; i < 7; i++) {
		if(time.tm_wday == 0) {
			printf("(every day)");
			break;
		} else if (time.tm_wday & (1 << i)) {
			printf("(%s)",  wday[((i+1) % 7)]);
		}
	}
	printf("\n");

    return 0;
}

static int sh_set_alarmtime(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;
    int i = 0;
    time_t t;
    struct tm *tm;
    struct tm time, al_time;

    if (argc < 5) {
        printf("Invaild argument.\n");
        printf("Usage: %s\n", set_alarmtime_usage);
        rc =  -1;
        goto EXIT;
    }

    rtc_get_time(&time);

    al_time.tm_hour = atoi(argv[1]);
    al_time.tm_min = atoi(argv[2]);
    al_time.tm_sec = atoi(argv[3]);
    al_time.tm_wday = time.tm_wday;
    al_time.tm_year = time.tm_year;
	al_time.tm_mon = time.tm_mon;
	al_time.tm_mday = time.tm_mday;

	t = mktime(&al_time);

	t = mktime(&time);
    tm = localtime(&t);

    al_time.tm_wday = atoi(argv[4]);

    printf("Set alarm as %02d:%02d:%02d",
    	al_time.tm_hour, al_time.tm_min, al_time.tm_sec);
    for(i = 0; i < 7; i++) {
    	if(al_time.tm_wday == 0) {
    		printf("(every day)");
    		break;
    	} else if (al_time.tm_wday & (1 << i)) {
    		printf("(%s)",  wday[((i+1) % 7)]);
    	}
    }
    printf("\n");

    rc = rtc_set_alarm_time(tm, &al_time);
    if (rc < 0) {
        printf("Set alarm time failed - %d\n", rc);
        goto EXIT;
    }

EXIT:
    return rc;
}

static int sh_set_alarm(int argc, char **argv, void **result, void *sys_ctx)
{
    if (argc < 2) {
        printf("Invaild argument.\n");
        printf("Usage: %s\n", set_alarm_usage);
        return  -1;
    }

    rtc_set_alarm(atoi(argv[1]));

    return 0;
}
