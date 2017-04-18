/**
 *  @file   sh_irqtest.c
 *  @brief  irq test command
 *  $Id: sh_irqtest.c,v 1.3 2014/07/11 11:08:48 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/04/30  hugo      New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <timer.h>
#include <sys.h>
#include <ctype.h>
#define DBG_LEVEL   4
#include <debug.h>
#include <irq.h>
#include <interrupt.h>

command_init (sh_irqtest, "irqtest", "irqtest <count>");

static unsigned int volatile _count = 0;
static unsigned int fgCnt, bgCnt;
static unsigned int fgTime, bgTime;

static void
ostimer_isr (int irq, void *dev_id, struct pt_regs *regs)
{
    unsigned int tick = read_c0_count32();
    bgCnt++;

    _count--;
    if (_count == 0) {
        writel (readl(T30X_OSTIMER_OSIMR) | OSIMR(2), T30X_OSTIMER_OSIMR);
        writel (OSSR(2), T30X_OSTIMER_OSSR);
    }

    bgTime += (read_c0_count32() - tick) / (sys_cpu_clk/2/1000);
}

static int
sh_irqtest (int argc, char *argv[], void **result, void *sys_ctx)
{
    struct irqaction ostimer_irqaction;
    unsigned int tick, tick1, tick2, allTime;
    int rc;

    _count = argc > 1 ? atoi (argv[1]) : 10000;

    fgCnt = bgCnt = 0;
    fgTime = bgTime = 0;

    ostimer_irqaction.handler = ostimer_isr;
    ostimer_irqaction.flags = SA_INTERRUPT;
    ostimer_irqaction.mask = 0;
    ostimer_irqaction.name = "OSTIMER";
    ostimer_irqaction.next = NULL;
    ostimer_irqaction.dev_id = NULL;

    rc = setup_irq (IRQ_TIMER, &ostimer_irqaction);
    if (rc < 0) {
        ERROR ("setup_irq(IRQ_TIMER), rc=%d\n", rc);
        goto EXIT;
    }


    writel (OSSR(2), T30X_OSTIMER_OSSR);
    writel (readl(T30X_OSTIMER_OSCR)+1, T30X_OSTIMER_OSMR2);

    tick1 = read_c0_count32();
    writel (readl(T30X_OSTIMER_OSIMR) & ~OSIMR(2), T30X_OSTIMER_OSIMR);

    while (_count != 0) {
        tick = read_c0_count32();
        fgCnt++;
        fgTime += (read_c0_count32() - tick) / (sys_cpu_clk/2/1000);
    }

    writel (readl(T30X_OSTIMER_OSIMR) | OSIMR(2), T30X_OSTIMER_OSIMR);
    tick2 = read_c0_count32();

    disable_irq (IRQ_TIMER);
    free_irq (IRQ_TIMER, NULL);

    allTime = (tick2 - tick1) / (sys_cpu_clk/2/1000);

    printf ("Cnt fg:%d, bg:%d\n", fgCnt, bgCnt);
    printf ("Time fg:%u, bg: %u\n", fgTime, bgTime);
    printf ("Time: %u.%03us\n", allTime / 1000, allTime % 1000);
    printf ("INT Freq: %u.%01u\n",
            1000 * bgCnt / allTime,
            (10 * 1000 * bgCnt / allTime) % 10);

    return 0;

EXIT:
    print_usage (sh_irqtest);
    return -1;
}
