
#include <config.h>
#include <stdio.h>
#include <sys.h>
#include <mipsregs.h>
#include <interrupt.h>

static unsigned long r4k_offset; /* Amount to increment compare reg each time */
static unsigned long r4k_cur;    /* What counter should be at next timer irq */

#define HZ  100
volatile unsigned int jiffies = 0;

static inline void ack_r4ktimer(unsigned long newval)
{
	write_c0_compare(newval);
}

__mips32__
void mips_timer_interrupt(struct pt_regs *regs)
{
    jiffies++;
    r4k_cur += r4k_offset;
	ack_r4ktimer(r4k_cur);
}

/*
 * Figure out the r4k offset, the amount to increment the compare
 * register for each time tick.
 */
__mips32__
static unsigned long cal_r4koff(void)
{
	/*
	 * The SEAD board doesn't have a real time clock, so we can't
	 * really calculate the timer offset.
	 * For now we hardwire the SEAD board frequency to 12MHz.
	 */
    return(sys_cpu_clk/(HZ*2));
}

__mips32__
void mips_time_init(void)
{
    r4k_offset = cal_r4koff();

    r4k_cur = (read_c0_count() + r4k_offset);
    write_c0_compare(r4k_cur);
    set_c0_status(IE_IRQ5);
    set_c0_cause(1 << 26);	/* performance coutner interrupt */
}

__mips32__
void time_init(void)
{
    mips_time_init();
}


