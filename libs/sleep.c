/**
 *  @file   sleep.c
 *  @brief  POSIX like sleep/usleep function
 *  $Id: sleep.c,v 1.5 2014/02/11 11:59:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <timer.h>
#include <serial.h>
#include <vt100.h>
#include <mipsregs.h>
#include <sys.h>

unsigned long long read_cpu_count()
{
    static unsigned long long timer = 0;
    static unsigned long last;
    unsigned long t;
    if (timer == 0) {
        last = read_c0_count32();
        timer = 1;
    } else {
        t = read_c0_count32();
        timer += (t - last);
        last = t;
    }

    return timer;
}

int
sleep (unsigned time)
{
    unsigned int mark;
    unsigned int i;

    mark = read_c0_count32();

    for (i = 0; i < time; i++) {
        while(1) {
            if ((signed)(read_c0_count32() - mark) >= SEC_INTERVAL)
                break;
        }
        mark += SEC_INTERVAL; 
    }

	return 0;	
}

int 
msleep (unsigned mtime)
{
    unsigned int mark;

    if (mtime > 1000 /* 1 sec */)
        return sleep((mtime + 500) / 1000);

    mark = read_c0_count32();
    mark += mtime * (MSEC_INTERVAL);
    while(1) {
        if ((signed)(read_c0_count32()-mark) >= 0)
            break;
    }

    return 0;

}

int
usleep (unsigned utime)
{
    unsigned int mark;

    if (utime > 1000 /* 1 msec */) 
        return msleep((utime + 500)/1000);

    mark = read_c0_count32();
    mark += utime * (USEC_INTERVAL);
    while(1) {
        if ((signed)(read_c0_count32() - mark) >= 0)
            break;
    }

    return 0;
}
