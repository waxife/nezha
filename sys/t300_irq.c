/**
 *  @file   t300_irq.c
 *  @brief  here define interrupt dispatcher and init_IRQ
 *  $Id: t300_irq.c,v 1.6 2014/08/21 04:15:09 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/12  New file.
 *
 */


#include <config.h>
#include <stdio.h>
#include <io.h>
#include <irq.h>
#include <sys.h>
#include <mipsregs.h>

#define INTC_BASE       (0xb1400000)
#define INTC_IES        (INTC_BASE + 0x00)
#define INTC_IMASK      (INTC_BASE + 0x04)
#define INTC_IPOLARITY  (INTC_BASE + 0x08)
#define INTC_IPENDING   (INTC_BASE + 0x0C)
#define INTC_OPOLARITY  (INTC_BASE + 0x10)



/* default POLARITY */
#define POL_JPEGD       (1 << 0)
#define POL_DMAC        (1 << 1)
#define POL_CF          (1 << 2)
#define POL_SD          (1 << 3)
#define POL_MS          (1 << 4)
#define POL_XD          (1 << 5)
#define POL_NOR         (1 << 6)
#define POL_USB         (1 << 7)    /* should be modified */
#define POL_AHB2APB     (1 << 8)    
#define POL_SCALER      (1 << 9)
#define POL_RTC         (0 << 10)
#define POL_TIMER       (0 << 11)
#define POL_UART        (1 << 12)
#define POL_I2S         (1 << 13)
#define POL_GPIO        (0 << 14)
#define POL_NAND        (1 << 15)
#define POL_DIR         (0 << 16)
#define POL_AFCUART     (1 << 29)


#define UNUSE_BITS			0xfffe0000

#define DEFAULT_POLARITY  ( UNUSE_BITS |  (POL_JPEGD | POL_DMAC | POL_CF | POL_SD | \
                          POL_MS | POL_XD | POL_NOR | POL_USB | \
                          POL_AHB2APB | POL_SCALER | POL_RTC | POL_TIMER | \
                          POL_UART | POL_I2S | POL_GPIO | POL_NAND  | POL_DIR | POL_AFCUART ) )


#define asmlinkage
extern asmlinkage void mipsIRQ_srs(void);

#if 0
#define DEBUG_INT(x...) printf(x)
#else
#define DEBUG_INT(x...)
#endif

unsigned int preempt_int_mask = 0xFFFFFFFF;
unsigned int save_int_mask = 0;

void __mips32__
disable_t300_irq(unsigned int irq_nr)
{
    unsigned int mask;
    mask = readl(INTC_IMASK);
    mask |= ( 1 << irq_nr);
    writel(mask, INTC_IMASK);
}

void __mips32__
enable_t300_irq(unsigned int irq_nr)
{
    unsigned int mask;
    mask = readl(INTC_IMASK);
    mask &= ~ (1 << irq_nr);
    writel(mask, INTC_IMASK);
}

unsigned int __mips32__
startup_t300_irq(unsigned int irq)
{
	enable_t300_irq(irq);
	return 0; /* never anything pending */
}

#define shutdown_t300_irq	disable_t300_irq

void __mips32__
mask_and_ack_t300_irq(unsigned int irq)
{
    if (!(preempt_int_mask & (1 << irq))) {
        /* preemptive interrupt */
        save_int_mask = readl(INTC_IMASK);
        writel(preempt_int_mask | (1 << irq), INTC_IMASK);
    } else {
        disable_t300_irq(irq);
    }

    writel((1 << irq), INTC_IES);    /* clear interrupt status */
    do {
        readl(INTC_IES);        /* force the effects of interrupt status clearing */
    } while(0);
}    

void __mips32__
end_t300_irq(unsigned int irq)
{

	if (!(irq_desc[irq].status & (IRQ_DISABLED|IRQ_INPROGRESS))) {
        if (!(preempt_int_mask & (1 << irq))) {
            writel(save_int_mask, INTC_IMASK);
        } else {
		    enable_t300_irq(irq);
        }
    }
}

static struct hw_interrupt_type t300_irq_type = {
	"T300",
#if 0
	startup_t300_irq,
	shutdown_t300_irq,
	enable_t300_irq,
	disable_t300_irq,
	mask_and_ack_t300_irq,
	end_t300_irq,
	NULL
#endif
};

#if 0
static inline int ls1bit32(unsigned int x)
{
	int b = 31, s;

	s = 16; if (x << 16 == 0) s = 0; b -= s; x <<= s;
	s =  8; if (x <<  8 == 0) s = 0; b -= s; x <<= s;
	s =  4; if (x <<  4 == 0) s = 0; b -= s; x <<= s;
	s =  2; if (x <<  2 == 0) s = 0; b -= s; x <<= s;
	s =  1; if (x <<  1 == 0) s = 0; b -= s;

	return b;
}
#else	/* Add IRQ Priority */
const static int irq_priority_tbl[] = {
	(1 << IRQ_JPEG_CODEC),
	(1 << IRQ_DMAC),
	(1 << IRQ_EI80_SLV),
	(1 << IRQ_SD),
	(1 << IRQ_I80_MAS),
	(1 << IRQ_PRE_REC),
	(1 << IRQ_NOR),
	(1 << IRQ_MOTION_DT),
	(1 << IRQ_AHB2APB),
	(1 << IRQ_TIMER),
	(1 << IRQ_GPIO),
	(1 << IRQ_UART),
	(1 << IRQ_AUD_CTRL),
	(1 << IRQ_UART_AFC),
	(1 << IRQ_SCALER),
	(1 << IRQ_RTC),
	(1 << 15), (1 << 16), (1 << 17), (1 << 18), (1 << 19),
	(1 << 20), (1 << 21), (1 << 22), (1 << 23), (1 << 24),
	(1 << 25), (1 << 26), (1 << 27), (1 << 28), (1 << 30),
	(1 << 31)
};
const static int irq_priority_tbl2[] = {
	IRQ_JPEG_CODEC,
	IRQ_DMAC,
	IRQ_EI80_SLV,
	IRQ_SD,
	IRQ_I80_MAS,
	IRQ_PRE_REC,
	IRQ_NOR,
	IRQ_MOTION_DT,
	IRQ_AHB2APB,
	IRQ_TIMER,
	IRQ_GPIO,
	IRQ_UART,
	IRQ_AUD_CTRL,
	IRQ_UART_AFC,
	IRQ_SCALER,
	IRQ_RTC,
	15, 16, 17, 18, 19,
	20, 21, 22, 23, 24,
	25, 26, 27, 28, 30,
	31
};
static inline int irq_scan(unsigned int x)
{
	int i = 0;

	for(i = 0; i < sizeof(irq_priority_tbl); i++) {
		if(x & irq_priority_tbl[i])
			return irq_priority_tbl2[i];
	}

	return 0;
}
#endif

static int __local_irq_count = 0;
static int irq_err_count = 0;

static inline void irq_enter(int cpu, int irq)
{
    __local_irq_count++;
}

static inline void irq_exit(int cpu, int irq)
{
    __local_irq_count--;
}

/*
 * This should really return information about whether
 * we should do bottom half handling etc. Right now we
 * end up _always_ checking the bottom half, which is a
 * waste of time and is not what some drivers would
 * prefer.
 */
static int __mips32__
handle_IRQ_event(unsigned int irq, struct pt_regs * regs, struct irqaction * action)
{
	int status;

	irq_enter(0, irq);

	status = 1;	/* Force the "do bottom halves" bit */

	if (!(action->flags & SA_INTERRUPT))
		sti();
	do {
		status |= action->flags;
		action->handler(irq, action->dev_id, regs);
		action = action->next;
	} while (action);
	cli();

	irq_exit(0, irq);

	return status;
}

void __mips32__
t300_hw0_irqdispatch(struct pt_regs *regs)
{
	struct irqaction *action;
	irq_desc_t *desc;
	int irq;
    unsigned long status;

//    __asm__  __volatile__ ("rdpgpr %0, $29" : "=r"(status));

	status = readl(INTC_IES);    /* get interrrupt status from INTC */

	/* if status == 0, then the interrupt has already been cleared */
	if (status == 0)
		return;

#if 0
	irq = ls1bit32(status);
#else	/* Add IRQ Priority */
	irq = irq_scan(status);
#endif
    desc = irq_desc + irq;
    irqs[irq]++;
    mask_and_ack_t300_irq(irq);

	DEBUG_INT("t300_hw0_irqdispatch: irq=%d\n", irq);

    
    status = desc->status & ~(IRQ_REPLAY | IRQ_WAITING);
    status |= IRQ_PENDING;

    /* 
     * If the IRQ is disabled for whatever reason, we cannot
     * use the action we have.
     */
    action = NULL;
    if (!(status & (IRQ_DISABLED | IRQ_INPROGRESS))) {
        action = desc->action;
        status &= ~IRQ_PENDING;     /* we commit to handling */
        status |= IRQ_INPROGRESS;   /* we are handling it */
    }
    
    desc->status = status;
    
	/*
	 * If there is no IRQ handler or it was disabled, exit early.
	 * Since we set PENDING, if another processor is handling
	 * a different instance of this same irq, the other processor
	 * will take care of it.
	 */
	if (!action) {
        irq_err_count++;
		goto out;    
    }
    
	/*
	 * Edge triggered interrupts need to remember
	 * pending events.
	 * This applies to any hw interrupts that allow a second
	 * instance of the same irq to arrive while we are in do_IRQ
	 * or in the handler. But the code here only handles the _second_
	 * instance of the irq, not the third or fourth. So it is mostly
	 * useful for irq hardware that does not mask cleanly in an
	 * SMP environment.
	 */
	for (;;) {
		handle_IRQ_event(irq, regs, action);

		if (!(desc->status & IRQ_PENDING))
			break;
		desc->status &= ~IRQ_PENDING;
	}
	desc->status &= ~IRQ_INPROGRESS;

out:
        
	/*
	 * The ->end() handler has to deal with interrupts which got
	 * disabled while the handler was running.
	 */
	 end_t300_irq(irq);

	return;
}

static void __mips32__
t300_hw_init(void)
{
    writel(0xffffffff, INTC_IMASK); /* all interrupt mask on */
    writel(DEFAULT_POLARITY&(~0x400000), INTC_IPOLARITY); /* IRQ 22 SYSMGR polarity 0 */
    writel(1, INTC_OPOLARITY);

    preempt_int_mask = 0xFFFFFFFF;
    save_int_mask = readl(INTC_IMASK);
}

void __mips32__
t300_irq_add_preempt(unsigned int irq)
{
    preempt_int_mask &= ~(1 << irq);
}


void __mips32__
t300_irq_rm_preempt(unsigned int irq)
{
    preempt_int_mask |= (1 << irq);
}


void __mips32__
init_IRQ(void)
{
	int i;
    extern void *set_except_vector(int n, void *addr);
    unsigned long val;

    /* intc init */
    t300_hw_init();

    /* setup shadow register set to 1 */
	val = read_c0_srsctl();
	if ( ((val>>26)&&0x0f) == 0 ) {
	    printf("Do not have 2nd set of shadow register, interrupt won't works!\n");
	}
	val |= (0x01<<12); /* ESS [15:12] */
	write_c0_srsctl(val);
	
	/* to use cp0 register $31 for handling nested exception */
	write_c0_desave(0);

	/* Now safe to set the exception vector. */
	set_except_vector(0, mipsIRQ_srs);

    set_c0_status(IE_IRQ0);
  
	for (i = 0; i <= NR_IRQS; i++) {
		irq_desc[i].status	= IRQ_DISABLED;
		irq_desc[i].action	= 0;
		irq_desc[i].depth	= 1;
		irq_desc[i].handler	= &t300_irq_type;
	}

}

