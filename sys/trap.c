/**
 *  @file   trap.c
 *  @brief  here init exception and interrupt entries
 *  $Id: trap.c,v 1.2 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/12  New file.
 *
 */

/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (C) 1994 - 1999, 2000, 01 Ralf Baechle
 * Copyright (C) 1995, 1996 Paul M. Antoine
 * Copyright (C) 1998 Ulf Carlsson
 * Copyright (C) 1999 Silicon Graphics, Inc.
 * Kevin D. Kissell, kevink@mips.com and Carsten Langgaard, carstenl@mips.com
 * Copyright (C) 2000, 01 MIPS Technologies, Inc.
 * Copyright (C) 2002, 2003, 2004  Maciej W. Rozycki
 */

#include <config.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <mipsregs.h>
#include <io.h>
#include "ptrace.h"
#include "cache.h"

#define asmlinkage  


extern asmlinkage void handle_mod(void);
extern asmlinkage void handle_tlbl(void);
extern asmlinkage void handle_tlbs(void);
extern asmlinkage void handle_adel(void);
extern asmlinkage void handle_ades(void);
extern asmlinkage void handle_ibe(void);
extern asmlinkage void handle_dbe(void);
extern asmlinkage void handle_sys(void);
extern asmlinkage void handle_bp(void);
extern asmlinkage void handle_ri(void);
extern asmlinkage void handle_cpu(void);
extern asmlinkage void handle_ov(void);
extern asmlinkage void handle_tr(void);
extern asmlinkage void handle_fpe(void);
extern asmlinkage void handle_watch(void);
extern asmlinkage void handle_mcheck(void);
extern asmlinkage void handle_reserved(void);

static inline int delay_slot(struct pt_regs *regs)
{
	return regs->cp0_cause & CAUSEF_BD;
}

static inline unsigned long exception_epc(struct pt_regs *regs)
{
	if (!delay_slot(regs))
		return regs->cp0_epc;

	return regs->cp0_epc + 4;
}

extern int __compute_return_epc(struct pt_regs *regs);

static inline int compute_return_epc(struct pt_regs *regs)
{
	if (!delay_slot(regs)) {
		regs->cp0_epc += 4;
		return 0;
	}
#if 0
	return __compute_return_epc(regs);
#endif
    return 0;
}

__mips32__
void show_regs(struct pt_regs *regs)
{
	/*
	 * Saved main processor registers
	 */
    printf("Code dump ---- regs %p\n", regs);
    if (((unsigned)regs & 0x03) || (((unsigned)regs < 0x80000000) && ((unsigned) regs > 0xf0000000))) {
        printf("invalid regs %p\n", regs);
        return;
    }

	printf("$0 : %08x %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
	       0,             regs->regs[1], regs->regs[2], regs->regs[3],
	       regs->regs[4], regs->regs[5], regs->regs[6], regs->regs[7]);
	printf("$8 : %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
	       regs->regs[8],  regs->regs[9],  regs->regs[10], regs->regs[11],
	       regs->regs[12], regs->regs[13], regs->regs[14], regs->regs[15]);
	printf("$16: %08lx %08lx %08lx %08lx %08lx %08lx %08lx %08lx\n",
	       regs->regs[16], regs->regs[17], regs->regs[18], regs->regs[19],
	       regs->regs[20], regs->regs[21], regs->regs[22], regs->regs[23]);
	printf("$24: %08lx %08lx                   %08lx %08lx %08lx %08lx\n",
	       regs->regs[24], regs->regs[25],
	       regs->regs[28], regs->regs[29], regs->regs[30], regs->regs[31]);
	printf("Hi : %08lx\n", regs->hi);
	printf("Lo : %08lx\n", regs->lo);

	/*
	 * Saved cp0 registers
	 */
	printf("epc   : %08lx\n", regs->cp0_epc);
	printf("Status: %08lx\n", regs->cp0_status);
	printf("Cause : %08lx\n", regs->cp0_cause);
	printf("PrId  : %08x\n", read_c0_prid());
}


__mips32__
asmlinkage void panic(const char *fmt, ...)
{
    char buf[1024];
    va_list ap;

    
    va_start(ap, fmt);
    __doprnt(buf, 1024, fmt, ap);
    va_end(ap);
    printf("System panic: %s\n\n", buf);
    
    for(;;)
        ; /* do nothing */    
}

__mips32__
asmlinkage void do_ade(struct pt_regs *regs)
{
    printf("BadVAddr = %08x\n", read_c0_badvaddr());
    printf("Caught Ade excetion  - probably caused by unalignment accessing\n");
	show_regs(regs);
    compute_return_epc(regs);
}

__mips32__
asmlinkage void do_be(struct pt_regs *regs)
{
	int data = regs->cp0_cause & 4;

	/*
	 * Assume it would be too dangerous to continue ...
	 */
	printf("%s bus error, epc == %08lx, ra == %08lx\n",
	       data ? "Data" : "Instruction",
	       regs->cp0_epc, regs->regs[31]);

    compute_return_epc(regs);
}

__mips32__
asmlinkage void do_ov(struct pt_regs *regs)
{
    printf("Caught Overflow excetion\n");
	show_regs(regs);
    compute_return_epc(regs);
}

__mips32__
asmlinkage void do_sys(struct pt_regs *regs)
{
    printf("Caught Sys excetion\n");
	show_regs(regs);
    compute_return_epc(regs);
}

__mips32__
asmlinkage void do_bp(struct pt_regs *regs)
{
	printf("Caught Break exception\n");
	show_regs(regs);
    panic("break");
}

__mips32__
asmlinkage void do_tr(struct pt_regs *regs)
{
	printf("Caught Trap exception\n");
	show_regs(regs);
	panic("trap");

}

__mips32__
asmlinkage void do_ri(struct pt_regs *regs)
{
    printf("Caught RI exception\n");
	show_regs(regs);
	panic("ri");
}

__mips32__
asmlinkage void do_cpu(struct pt_regs *regs)
{
    printf("Caught CpU exception\n");
	show_regs(regs);
	panic("cpu");
}

__mips32__
asmlinkage void do_watch(struct pt_regs *regs)
{
	/*
	 * We use the watch exception where available to detect stack
	 * overflows.
	 */
	printf("Caught WATCH exception - probably caused by stack overflow.");
	show_regs(regs);
	panic("watch");
}

__mips32__
asmlinkage void do_mcheck(struct pt_regs *regs)
{
    printf("Caught Machine Check exception\n");
	show_regs(regs);
	/*
	 * Some chips may have other causes of machine check (e.g. SB1
	 * graduation timer)
	 */
	panic("Caught Machine Check exception - %scaused by multiple "
	      "matching entries in the TLB.",
	      (regs->cp0_status & ST0_TS) ? "" : "not ");
}

__mips32__
asmlinkage void do_reserved(struct pt_regs *regs)
{
	/*
	 * Game over - no way to handle this if it ever occurs.  Most probably
	 * caused by a new unknown cpu type or after another deadly
	 * hard/software error.
	 */
	printf("Caught reserved exception\n");
	show_regs(regs);
	panic("Caught reserved exception %ld - should not happen.",
	      (regs->cp0_cause & 0x7f) >> 2);
}


/*
 * NMI exception handler.
 */
__mips32__ 
void nmi_exception_handler(struct pt_regs *regs)
{
	printf("NMI taken!!!!\n");
	show_regs(regs);
	while(1) ;
}

unsigned long exception_handlers[32];

/*
 * As a side effect of the way this is implemented we're limited
 * to interrupt handlers in the address range from
 * KSEG0 <= x < KSEG0 + 256mb on the Nevada.  Oh well ...
 */
__mips32__ 
void *set_except_vector(int n, void *addr)
{
	unsigned long handler = (unsigned long) addr;
	unsigned long old_handler = exception_handlers[n];

	exception_handlers[n] = handler;

	return (void *)old_handler;
}

__mips32__
void  trap_init(void)
{
#if 0
	extern char except_vec3_generic;
#endif
	unsigned long i;

	/*
	 * Setup default vectors
	 */
	for (i = 0; i <= 31; i++)
		set_except_vector(i, handle_reserved);


	/*
	 * Only some CPUs have the watch exceptions.
	 */
    set_except_vector(23, handle_watch);

	/*
	 * The Data Bus Errors / Instruction Bus Errors are signaled
	 * by external hardware.  Therefore these two exceptions
	 * may have board specific handlers.
	 */

#if 0   /* TLB exception to handle_reserved */
	set_except_vector(1, handle_mod);
	set_except_vector(2, handle_tlbl);
	set_except_vector(3, handle_tlbs);
#endif

	set_except_vector(4, handle_adel);
	set_except_vector(5, handle_ades);

	set_except_vector(6, handle_ibe);
	set_except_vector(7, handle_dbe);

	set_except_vector(8, handle_sys);
	set_except_vector(9, handle_bp);
	set_except_vector(10, handle_ri);
	set_except_vector(11, handle_cpu);
	set_except_vector(12, handle_ov);
	set_except_vector(13, handle_tr);

    set_except_vector(24, handle_mcheck);

#if 0
    memcpy((void *)(KSEG0 + 0x180), &except_vec3_generic, 0x80);
#endif
	flush_icache_range(KSEG0, KSEG0 + 0x400);

	/*
	 * Disable coprocessors and 64-bit addressing and set FPU for
	 * the 16/32 FPR register model.  Reset the BEV flag that some
	 * firmware may have left set and the TS bit (for IP27).  Set
	 * XX for ISA IV code to work.
	 */

	change_c0_status(ST0_CU|ST0_RE|ST0_FR|ST0_BEV|ST0_TS|ST0_KX|ST0_SX|ST0_UX,
			 ST0_CU0);

}
