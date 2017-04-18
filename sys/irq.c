/**
 *  @file   irq.c
 *  @brief  here define interrupt dispatcher and init_IRQ
 *  $Id: irq.c,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/12  New file.
 *
 */


#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <io.h>
#include "irq.h"
#include "interrupt.h"


irq_desc_t      irq_desc[NR_IRQS];
unsigned int    irqs[NR_IRQS];

/* these functions are at t300_irq.c */
#define shutdown_t300_irq   disable_t300_irq
extern unsigned int __mips32__ startup_t300_irq(unsigned int irq);
extern void __mips32__ enable_t300_irq(unsigned int irq_nr);
extern void __mips32__ disable_t300_irq(unsigned int irq_nr);
extern void __mips32__ end_t300_irq(unsigned int irq);

/* this was setup_x86_irq but it seems pretty generic */
int setup_irq(unsigned int irq, struct irqaction * new)
{
	int shared = 0;
	// unsigned long flags;
	struct irqaction *old, **p;
	irq_desc_t *desc = irq_desc + irq;

	/*
	 * The following block of code has to be executed atomically
	 */
	p = &desc->action;
	if ((old = *p) != NULL) {
		/* Can't share interrupts unless both agree to */
		if (!(old->flags & new->flags & SA_SHIRQ)) {
			return -1;
		}

		/* add new interrupt at end of irq queue */
		do {
			p = &old->next;
			old = *p;
		} while (old);
		shared = 1;
	}

	*p = new;

	if (!shared) {
		desc->depth = 0;
		desc->status &= ~(IRQ_DISABLED | IRQ_AUTODETECT | IRQ_WAITING | IRQ_INPROGRESS);
		startup_t300_irq(irq);
	}

	// register_irq_proc(irq);
	return 0;
}


/**
 *	request_irq - allocate an interrupt line
 *	@irq: Interrupt line to allocate
 *	@handler: Function to be called when the IRQ occurs
 *	@irqflags: Interrupt type flags
 *	@devname: An ascii name for the claiming device
 *	@dev_id: A cookie passed back to the handler function
 *
 *	This call allocates interrupt resources and enables the
 *	interrupt line and IRQ handling. From the point this
 *	call is made your handler function may be invoked. Since
 *	your handler function must clear any interrupt the board
 *	raises, you must take care both to initialise your hardware
 *	and to set up the interrupt handler in the right order.
 *
 *	Dev_id must be globally unique. Normally the address of the
 *	device data structure is used as the cookie. Since the handler
 *	receives this value it makes sense to use it.
 *
 *	If your interrupt is shared you must pass a non NULL dev_id
 *	as this is required when freeing the interrupt.
 *
 *	Flags:
 *
 *	SA_SHIRQ		Interrupt is shared
 *
 *	SA_INTERRUPT		Disable local interrupts while processing
 *
 *	SA_SAMPLE_RANDOM	The interrupt can be used for entropy
 *
 */

int request_irq(unsigned int irq, 
        void (*handler)(int, void *, struct pt_regs *), 
        unsigned long irqflags, const char *devname, void *dev_id)
{
	int retval;
	struct irqaction * action = NULL;
#if 1
    static int one_irq = 0;
    static struct irqaction irq_action;
#endif

	/*
	 * Sanity-check: shared interrupts should REALLY pass in
	 * a real dev-ID, otherwise we'll have trouble later trying
	 * to figure out which interrupt is which (messes up the
	 * interrupt freeing logic etc).
	 */
	if (irqflags & SA_SHIRQ) {
		if (!dev_id)
			printf("Bad boy: %s (at 0x%x) called us without a dev_id!\n", devname, (&irq)[-1]);
	}

	if (irq >= NR_IRQS)
		return -1;
	if (!handler)
		return -1;

#if 0
	action = (struct irqaction *)
			malloc(sizeof(struct irqaction));
#else
    if ( !one_irq ) {
        action = &irq_action;
        one_irq = 1;
    }
#endif
    if (!action) {
        printf("only one irq support!\n");
		return -1;
    }

	action->handler = handler;
	action->flags = irqflags;
	action->mask = 0;
	action->name = devname;
	action->next = NULL;
	action->dev_id = dev_id;

	retval = setup_irq(irq, action);
#if 0
	if (retval)
		free(action);
#else
    if ( retval )
        one_irq = 0;
#endif

	return retval;
}

/**
 *	free_irq - free an interrupt
 *	@irq: Interrupt line to free
 *	@dev_id: Device identity to free
 *
 *	Remove an interrupt handler. The handler is removed and if the
 *	interrupt line is no longer in use by any driver it is disabled.
 *	On a shared IRQ the caller must ensure the interrupt is disabled
 *	on the card it drives before calling this function. The function
 *	does not return until any executing interrupts for this IRQ
 *	have completed.
 *
 *	This function may be called from interrupt context.
 *
 *	Bugs: Attempting to free an irq in a handler for the same irq hangs
 *	      the machine.
 */

void free_irq(unsigned int irq, void *dev_id)
{
	irq_desc_t *desc;
	struct irqaction **p;

	if (irq >= NR_IRQS)
		return;

	desc = irq_desc + irq;
	p = &desc->action;
	for (;;) {
		struct irqaction * action = *p;
		if (action) {
			struct irqaction **pp = p;
			p = &action->next;
			if (action->dev_id != dev_id)
				continue;

			/* Found it - now remove it from the list of entries */
			*pp = action->next;
			if (!desc->action) {
				desc->status |= IRQ_DISABLED;
                shutdown_t300_irq(irq);
			}

#if 0
			free(action);
#endif
			return;
		}
		printf("Trying to free free IRQ%d\n",irq);
		return;
	}
}


void disable_irq(int irq)
{
	irq_desc_t *desc = irq_desc + irq;

    if (!desc->depth++) {
        desc->status |= IRQ_DISABLED;
        disable_t300_irq(irq);
    }
}

void enable_irq(int irq)
{
	irq_desc_t *desc = irq_desc + irq;

  	switch (desc->depth) {
	case 1: {
		unsigned int status = desc->status & ~IRQ_DISABLED;
		desc->status = status;
		if ((status & (IRQ_PENDING | IRQ_REPLAY)) == IRQ_PENDING) {
			desc->status = status | IRQ_REPLAY;
			// hw_resend_irq(desc->handler,irq);
		}
		enable_t300_irq(irq);
		/* fall-through */
	}
	default:
		desc->depth--;
		break;
	case 0:
		printf("enable_irq(%u) unbalanced from %p\n", irq,
		       __builtin_return_address(0));
	}
}

