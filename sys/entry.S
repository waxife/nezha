/**
 *  @file   entry.S
 *  @brief  here define exception entries
 *  $Id: entry.S,v 1.2 2014/05/07 02:01:23 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/12  New file.
 *
 */

#include <config.h>
#include "regdef.h"
#include "asm.h"
#include "stackframe.h"


		.text
		.align	4
		.set	push
		.set	reorder
FEXPORT(ret_from_irq)
FEXPORT(ret_from_exception)
restore_all:	.set	noat
		RESTORE_ALL_AND_RET
		.set	at


/*
 * Build a default exception handler for the exceptions that don't need
 * special handlers.  If you didn't know yet - I *like* playing games with
 * the C preprocessor ...
 */
#define __BUILD_clear_none(exception)
#define __BUILD_clear_sti(exception)                            \
		STI
#define __BUILD_clear_cli(exception)                            \
		CLI
#define __BUILD_clear_ade(exception)                            \
		.set	reorder;					                    \
		MFC0	t0,CP0_BADVADDR;                                \
		.set	noreorder;					                    \
		sw	    t0,PT_BVADDR(sp);                               \
		KMODE

#define __BUILD_silent(exception)

#define fmt "Got %s at %08lx.\n"

#define __BUILD_verbose(exception)                              \
		la	    a1,8f;                                          \
		TEXT	(#exception);                                   \
		lw	    a2,PT_EPC(sp);                                  \
		PRINT(fmt)

/*
 * handle_xxx define macro
 */

#define BUILD_HANDLER(exception,handler,clear,verbose)          \
		.align	4;                                              \
NESTED(handle_##exception, PT_SIZE, sp);		                \
		.set	noat;                                           \
        .set    reorder;                                        \
		mfc0	k0, CP0_DESAVE;									\
		sll zero, 3;											\
		move	k1, k0;											\
		add		k0, k0, 1;										\
		mtc0	k0, CP0_DESAVE;									\
		sll zero, 3;											\
		beq		k1, zero, 1f;									\
		move	k0, ra;											\
		move	k1, sp;											\
		jal		save_all;										\
		nop;													\
		mfc0	a1, CP0_CAUSE;									\
		sll		zero, 3;										\
		PRINT("Nested EXP/INT Cause %08x\n");					\
		j		2f;												\
1:      move    k0, ra;                                         \
        move    k1, sp;                                         \
        jal     srs_save_all;                                   \
FEXPORT(handle_##exception##_int);					            \
		__BUILD_clear_##clear(exception);                       \
		.set	at;                                             \
		__BUILD_##verbose(exception);                           \
2:		jal	    do_##handler;                                   \
		move	a0, sp;                                         \
		mfc0	k0, CP0_DESAVE;									\
		sll zero, 3;											\
		sub		k0, k0, 1;										\
		mtc0	k0, CP0_DESAVE;									\
		sll zero, 3;											\
		beq		k0, zero, 1f;									\
		j	    ret_from_exception;                             \
		nop;                                                    \
1:		j 		srs_ret_from_exception;							\
		nop;													\
END(handle_##exception)

		BUILD_HANDLER(adel,ade,ade,silent)		        /* #4  */
		BUILD_HANDLER(ades,ade,ade,silent)		        /* #5  */
		BUILD_HANDLER(ibe,be,cli,silent)		        /* #6  */
		BUILD_HANDLER(dbe,be,cli,silent)		        /* #7  */
		BUILD_HANDLER(sys,sys,sti,silent)			    /* #8  */
		BUILD_HANDLER(bp,bp,sti,silent)			        /* #9  */
		BUILD_HANDLER(ri,ri,sti,silent)			        /* #10 */
		BUILD_HANDLER(cpu,cpu,sti,silent)	        	/* #11 */
		BUILD_HANDLER(ov,ov,sti,silent)			        /* #12 */
		BUILD_HANDLER(tr,tr,sti,silent)			        /* #13 */
		BUILD_HANDLER(watch,watch,sti,silent)		    /* #23 */
		BUILD_HANDLER(mcheck,mcheck,cli,silent)		    /* #24 */
		BUILD_HANDLER(reserved,reserved,sti,silent)	    /* others */

FEXPORT(save_all)
save_all:       
        .set    noat
        .set    reorder
        SAVE_ALL
        jr      ra

		.set	pop

FEXPORT(srs_save_all)
srs_save_all:
		.set	noat
		.set	push
		.set	reorder
		rdpgpr	sp, sp;
		subu	sp, sp, 48;
		mfc0	v1, CP0_STATUS;
		nop;
		sw		v1, 24(sp);
		mfc0	v1, CP0_CAUSE;
		nop;
		sw		v1, 28(sp);
		mfc0	v1, CP0_EPC;
		nop;
		sw		v1, 32(sp);
		mfhi	v1;
		nop;
		nop;
		sw		v1, 36(sp);
		mflo	v1;
		nop;
		nop;
		sw		v1, 40(sp);
		nop;
		jr		ra;
		.set	pop

FEXPORT(srs_ret_from_irq)
FEXPORT(srs_ret_from_exception)
srs_restore_all:	.set	noat
		.set	push
		.set	reorder
		mfc0	a0, CP0_STATUS;
		.set	pop
		ori		a0, 0x1f;
		xori	a0,	0x1f;
		mtc0	a0, CP0_STATUS;
		li		v1, 0xff00;
		and		a0, v1;
		lw		v0, 24(sp);
		nor		v1, $0, v1;
		and		v0, v1;
		or		v0, a0;
		mtc0	v0, CP0_STATUS;
		lw		v1, 32(sp);
		mtc0	v1, CP0_EPC;
		lw		t0, 40(sp);
		mtlo	t0;
		lw		t0, 36(sp);
		mthi	t0;
		.set	mips3
		eret;
		.set	mips0

