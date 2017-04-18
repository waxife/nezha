/**
 *  @file   irq.h
 *  @brief  here define irq number and irq functions
 *  $Id: interrupt.h,v 1.3 2014/03/25 05:12:01 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/12  New file.
 *
 */

#ifndef __INTERRUPT_H
#define __INTERRUPT_H

#define IRQ_JPEG_CODEC  0
#define IRQ_DMAC        1
#define IRQ_EI80_SLV    2
#define IRQ_SD          3
#define IRQ_I80_MAS     4
#define IRQ_PRE_REC     5
#define IRQ_NOR         6
#define IRQ_MOTION_DT   7
#define IRQ_AHB2APB     8
#define IRQ_SCALER      9
#define IRQ_RTC         10
#define IRQ_TIMER       11
#define IRQ_UART        12
#define IRQ_AUD_CTRL    13
#define IRQ_GPIO        14

//#define IRQ_TP			18
//#define IRQ_SYSMGR		22
#define IRQ_UART_AFC		29

#define NR_IRQS         30

struct pt_regs;

#define SA_INTERRUPT   0x20000000 
#define SA_SHIRQ       0x40000000
    
typedef void (*isr_fn)(int, void *, struct pt_regs *);

struct irqaction {
        isr_fn handler;
        unsigned long flags;
        unsigned long mask;
        const char *name;
        void *dev_id;
        struct irqaction *next;
};


extern int request_irq(unsigned int irq, 
    void (*handler)(int, void *, struct pt_regs *), 
    unsigned long flags, const char *dev_name, void *dev_id);

extern void free_irq(unsigned int irq, void *dev_id);
extern void disable_irq(int irq);
extern void enable_irq(int irq);

#endif /* __INTERRUPT_H */

