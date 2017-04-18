/**
 *  @file   stopwatch.h
 *  @brief  stopwatch
 *  $Id: stopwatch.h,v 1.2 2014/01/27 02:27:48 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/04/26  New file.
 *
 */
#include <config.h>
#include <mipsregs.h>
#include <sys.h>

#ifndef __STOPWATCH_H
#define __STOPWATCH_H

struct stopwatch_t {
    unsigned long   start;
    unsigned long   accumlate;
};

#define CPU_HZ          (sys_cpu_clk/2)
#define CPU_KHZ         (sys_cpu_clk/2000)

#define new_stopwatch(x)    struct stopwatch_t x = {0, 0}
#define mark_start(x)   x.start = read_c0_count32()
#define mark_stop(x)    x.accumlate = x.accumlate + (read_c0_count32() - x.start)
#define mark_show(x)    printf("elapse %ld.%03ld seconds\n", x.accumlate/CPU_HZ, (x.accumlate/CPU_KHZ)%1000)

#endif /* __STOPWATCH_H */

