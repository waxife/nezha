/**
 *  @file   mipsIRQ_srs.S
 *  @brief  here define interrupt entries (with shadow register set support)
 *
 *  $Id: mipsIRQ_srs.S,v 1.2 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/20	gary  New file.
 *
 */


/*
 * Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 1999, 2000 MIPS Technologies, Inc.  All rights reserved.
 *
 * ########################################################################
 *
 *  This program is free software; you can distribute it and/or modify it
 *  under the terms of the GNU General Public License (Version 2) as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 *  for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  59 Temple Place - Suite 330, Boston MA 02111-1307, USA.
 *
 * ########################################################################
 *
 * Interrupt exception dispatch code.
 *
 */
#include <config.h>

#include <asm.h>
#include <mipsregs.h>
#include <regdef.h>
#include <stackframe.h>

/* A lot of complication here is taken away because:
 *
 * 1) We handle one interrupt and return, sitting in a loop and moving across
 *    all the pending IRQ bits in the cause register is _NOT_ the answer, the
 *    common case is one pending IRQ so optimize in that direction.
 *
 * 2) We need not check against bits in the status register IRQ mask, that
 *    would make this routine slow as hell.
 *
 * 3) Linux only thinks in terms of all IRQs on or all IRQs off, nothing in
 *    between like BSD spl() brain-damage.
 *
 * Furthermore, the IRQs on the MIPS board look basically (barring software
 * IRQs which we don't use at all and all external interrupt sources are
 * combined together on hardware interrupt 0 (MIPS IRQ 2)) like:
 *
 *	    MIPS IRQ	    Source
 *      --------        ------
 *             0	    Software (ignored)
 *             1        Software (ignored)
 *             2        Combined hardware interrupt (hw0)
 *             3        Hardware (ignored)
 *             4        Hardware (ignored)
 *             5        Hardware (ignored)
 *             6        Hardware (ignored)
 *             7        R4k timer (what we use)
 *
 * We handle the IRQ according to _our_ priority which is:
 *
 * Highest ----     R4k Timer
 * Lowest  ----     Combined hardware interrupt
 *
 * then we just return, if multiple IRQs are pending then we will just take
 * another exception, big deal.
 */

	.text
	.set	noreorder
	.set	noat
	.align	5
	NESTED(mipsIRQ_srs, PT_SIZE, sp)
	mfc0	k0, CP0_DESAVE
	sll		zero, 3
	move	k1, k0
	add		k0, k0, 1
	mtc0	k0, CP0_DESAVE
	sll		zero, 3
	beq		k1, zero, 1f
	nop
	PANIC("Nested INT?!\n");
	
1:  move    k0, ra
    jal     srs_save_all
    move    k1, sp
	CLI
	.set	at

	mfc0	s0, CP0_CAUSE		# get irq bits
	mfc0	s1, CP0_STATUS		# get irq mask
	and	    s0, s1

#if 0
	/* First we check for r4k counter/timer IRQ. */
1:	andi	a0, s0, CAUSEF_IP7
	beq	    a0, zero, 1f
    andi	a0, s0, CAUSEF_IP2	# delay slot, check hw0 interrupt

/* Wheee, a timer interrupt. */
	move	a0, sp
	jal	    mips_timer_interrupt
	 nop

	mfc0	k0, CP0_DESAVE
	sll		zero, 3
	sub		k0, k0, 1
	mtc0	k0, CP0_DESAVE
	sll		zero, 3
	j	    srs_ret_from_irq
	 nop
#endif


1:
	beq	    a0, zero, 1f
	andi	a0, s0, CAUSEF_IP3	# delay slot, check hw1 interrupt

/* Wheee, combined hardware level zero interrupt. */
	jal	    t300_hw0_irqdispatch
     move	a0, sp			    # delay slot

	mfc0	k0, CP0_DESAVE
	sll		zero, 3
	sub		k0, k0, 1
	mtc0	k0, CP0_DESAVE
	sll		zero, 3
	j	    srs_ret_from_irq
	 nop				        # delay slot

1:
	/*
	 * Here by mistake?  This is possible, what can happen is that by the
	 * time we take the exception the IRQ pin goes low, so just leave if
	 * this is the case.
	 */
	move	a1,s0
	mfc0	a2, CP0_EPC
	PRINT("Got interrupt: c0_cause = %08x c0_epc = %08x\n")

	mfc0	k0, CP0_DESAVE
	sll		zero, 3
	sub		k0, k0, 1
	mtc0	k0, CP0_DESAVE
	sll		zero, 3
	j	srs_ret_from_irq
	 nop
	END(mipsIRQ_srs)
