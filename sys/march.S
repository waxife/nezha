/**
 *  @file   march.S
 *  @brief  memory testing program (march)
 *  $Id: march.S,v 1.2 2014/01/27 02:27:48 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/09/24  gary	New file.
 *
 */

#include <config.h>
#include <io.h>
#include <regdef.h>

/************************** 
 * Debug Macro Function
 * HALT - 
 * PUTCHAR - 
 * PUTREG - 
 * SHWMSG - 
 * SHWINT - 
 **************************/

#define HALT()				\
1:	b	1b;

#define PUTCHAR(ch)			\
    li  a0, ch;             \
    bal _putb;              \

#define SHWREG(reg)         \
    move    a0, reg;        \
    bal     _show_hex;

#define SHWMSG(str)         \
    la      a0, str;        \
    bal     _puts;
    
#define SHWINT(reg)         \
    move    a0, reg;        \
    bal     _show_int;


/* 
 * March 11N Dictionary
 */

#define SAF0		((1 << 3) | (1 << 4) | (1 << 8))
#define SAF1		((1 << 1) | (1 << 6) | (1 << 10))
#define CFIN0		((1 << 4) | (1 << 10))
#define CFIN1		((1 << 6) | (1 << 8))
#define CFIN2		((1 << 1) | (1 << 8))
#define CFIN3		((1 << 3) | (1 << 4) | (1 << 6))
#define CFST0		((1 << 4))
#define CFST1		((1 << 8))
#define CFST2		((1 << 6) | (1 << 10))
#define CFST3		((1 << 1) | (1 << 10))
#define CFST4		((1 << 3) | (1 << 8))
#define CFST5		((1 << 3) | (1 << 4))
#define CFST6		((1 << 1))
#define CFST7		((1 << 6))


/* 
 * Variables kept by march function
 */

#define march_cnt	s2	// the count for the test cycles
#define march_head	s4	// the lowest address for the march test
#define march_tail	s5	// the highest address for the march test
#define march_rst	v0	// the march result
#define count		s8	// the count of the test which has been done

#define PATTERN		    0x00000000
#define ANTI_PATTERN	0xFFFFFFFF


/***
 * void march
 *
 * Three arguments passed by the caller
 *
 * a0 - start address of test memory
 * a1 - end address of test memory (not included)
 * a2 - the count for the test cycles
 *
 * @return never return
 * @NOTE: this program will destory memory. 
 */

.text
    .globl  march
    .align  2
    .type   march,@function
    .ent    march,0
    .type   blast_icache16, @function

march:  .frame sp, 0, ra

	move 	march_head, a0
	move	march_tail, a1
	move	march_cnt,	a2
	move	count, zero
	SHWMSG(MsgMarchBegin)

/*
 * Prolog
 */

Prolog:

	add		count, count, 1

    /* if (count > march_cnt) goto MarchEnd */
	bgt	    count, march_cnt, MarchEnd

    SHWMSG(CycleCount)
 	SHWINT(count)
	SHWMSG(MsgMarch)
	b	    DoMarch

/* 
 * Memory march is performed here
 */

DoMarch:
	li	k0, PATTERN
	li	k1, ANTI_PATTERN
	move	march_rst, zero

	/* Step 1: up(w0) */
	PUTCHAR('1')
	move	t0, march_head
Step1Bgn:
	bgt	t0, march_tail, Step1End
	sw	k0, 0(t0)
	add	t0, t0, 4
	b	Step1Bgn
Step1End:

	/* Step 2: up(r0, w0) */
	PUTCHAR('2')
	move	t0, march_head
Step2Bgn:
	bgt	t0, march_tail, Step2End
	lw	t1, 0(t0)
	beq	t1, k0, 1f
	ori	march_rst, march_rst, (1 << 1)
1:	sw	k1, 0(t0)
	add	t0, t0, 4
	b	Step2Bgn
Step2End:

	/* Step 3: down(r1) */
	PUTCHAR('3')
	move	t0, march_tail
Step3Bgn:
	blt	t0, march_head, Step3End
	lw	t1, 0(t0)
	beq	t1, k1, 1f
	ori	march_rst, march_rst, (1 << 3)
1:	add	t0, t0, -4
	b	Step3Bgn
Step3End:

	/* Step 4: up(r1, w0) */
	PUTCHAR('4')
	move	t0, march_head
Step4Bgn:
	bgt	t0, march_tail, Step4End
	lw	t1, 0(t0)
	beq	t1, k1, 1f
	ori	march_rst, march_rst, (1 << 4)
1:	sw	k0, 0(t0)
	add	t0, t0, 4
	b	Step4Bgn
Step4End:

	/* Step 5: down(r0, w1) */
	PUTCHAR('5')
	move	t0, march_tail
Step5Bgn:
	blt	t0, march_head, Step5End
	lw	t1, 0(t0)
	beq	t1, k0, 1f
	ori	march_rst, march_rst, (1 << 6)
1:	sw	k1, 0(t0)
	add	t0, t0, -4
	b	Step5Bgn
Step5End:

	/* Step 6: down(r1, w0) */
	PUTCHAR('6')
	move	t0, march_tail
Step6Bgn:
	blt	t0, march_head, Step6End
	lw	t1, 0(t0)
	beq	t1, k1, 1f
	ori	march_rst, march_rst, (1 << 8)
1:	sw	k0, 0(t0)
	add	t0, t0, -4
	b	Step6Bgn
Step6End:

	/* Step 7: up(r0) */
	PUTCHAR('7')
	move	t0, march_head
Step7Bgn:
	bgt	t0, march_tail, Step7End
	lw	t1, 0(t0)
	beq	t1, k0, 1f
	ori	march_rst, march_rst, (1 << 10)
1:	add	t0, t0, 4
	b	Step7Bgn
Step7End:


    SHWMSG(MsgMarchResult)
    SHWREG(march_rst)
    PUTCHAR(']')
    PUTCHAR('\n')

/*
 * Analysis
 */
	beqz	march_rst, Epilog

ErrSAF0:
	li	t0, SAF0
	bne	march_rst, t0, ErrSAF1

	SHWMSG(MsgSAF)
	HALT()

ErrSAF1:
	li	t0, SAF1
	bne	march_rst, t0, ErrCFin0

	SHWMSG(MsgSAF)
	HALT()

ErrCFin0:
	li	t0, CFIN0
	bne	march_rst, t0, ErrCFin1

	SHWMSG(MsgCFin)
	HALT()

ErrCFin1:
	li	t0, CFIN1
	bne	march_rst, t0, ErrCFin2

	SHWMSG(MsgCFin)
	HALT()

ErrCFin2:
	li	t0, CFIN2
	bne	march_rst, t0, ErrCFin3

	SHWMSG(MsgCFin)
	HALT()

ErrCFin3:
	li	t0, CFIN3
	bne	march_rst, t0, ErrCFst0

	SHWMSG(MsgCFin)
	HALT()

ErrCFst0:
	li	t0, CFST0
	bne	march_rst, t0, ErrCFst1

	SHWMSG(MsgCFst)
	HALT()

ErrCFst1:
	li	t0, CFST1
	bne	march_rst, t0, ErrCFst2

	SHWMSG(MsgCFst)
	HALT()

ErrCFst2:
	li	t0, CFST2
	bne	march_rst, t0, ErrCFst3

	SHWMSG(MsgCFst)
	HALT()

ErrCFst3:
	li	t0, CFST3
	bne	march_rst, t0, ErrCFst4

	SHWMSG(MsgCFst)
	HALT()

ErrCFst4:
	li	t0, CFST4
	bne	march_rst, t0, ErrCFst5

	SHWMSG(MsgCFst)
	HALT()

ErrCFst5:
	li	t0, CFST5
	bne	march_rst, t0, ErrCFst6

	SHWMSG(MsgCFst)
	HALT()

ErrCFst6:
	li	t0, CFST6
	bne	march_rst, t0, ErrCFst7

	SHWMSG(MsgCFst)
	HALT()

ErrCFst7:
	li	t0, CFST7
	bne	march_rst, t0, ErrOthers

	SHWMSG(MsgCFst)
	HALT()

ErrOthers:
	SHWMSG(MsgOther)
	HALT()


/* 
 * Epilog
 */

Epilog:
	la		a0, Prolog

	jr	    a0

MarchEnd:
	SHWMSG(MsgMarchEnd)
	SHWMSG(MsgMarchPass)
	HALT()
    .end    march
    .size   march, .-march



#define UART_BASE       0xb2000000
#define UART_THR        (0x00)
#define UART_LSR        (0x14)

serial_putb:
1:  li      t8, UART_BASE
    lb      t8, UART_LSR(t8)
    andi    t8, 0x40            /* while ((readl(UART_LSR) & 0x40) == 0); */
    beqz    t8, 1b
    li      t8, UART_BASE
    sb      a0, UART_THR(t8)
    jr      ra


/*** 
 * _putb : put a character to standout, if the character is '\n', it will put '\r' first
 *
 * register: a0 as input
 * register: t8, jp will be destoried
 * register: ra is return link
 */
_putb:
    li      t8, 10
    beq     a0, t8, 2f
    b       serial_putb

2:  move    jp, ra
    li      a0, 13
    bal     serial_putb
    move    ra, jp
    li      a0, 10
    b       serial_putb


/*** 
 * _show_int : show a unsigned decimal interger value
 *
 * register: a0 as input
 * register: t0, t1, t2, t3, t4, t5, t6, t7, will be destoried
 * register: ra is return link
 */

_show_int:
    move    t7, ra       
	move	t0, zero
	move	t6, zero
	move	t3, a0
	li	    t2, 0xA
1:	add	    t6, 1           /* t6 = digit count */
	div	    t3, t2
	mfhi	t4
	mflo	t5
	add	    t3, t4, t5
	beqz	t3, 3f
	mul	    t0, t0, t2
	add	    t0, t0, t4
	move	t3, t5
	b	    1b
3:	div	    t0, t2
	mfhi	t1
	add	    a0, t1, '0'
    bal     _putb
	sub	    t6, 1
	mflo	t0
	bnez	t0, 3b
4:	sub	    t6, 1
	beqz	t6, 5f
    li      a0, '0'
    bal     _putb
	b	    4b			
5:	move    ra, t7
    li      a0, '\n'
    b      _putb


/*** 
 * _show_int : show a unsigned decimal interger value
 *
 * register: a0 as input
 * register: t0, t1, t2, t7 will be destoried
 * register: ra is return link
 */

_show_hex:
    move    t7, ra
    move    t0, a0
    li      t2, 10
    li      t3, 32
1:  sub     t3, 4    
    srl     t1, t0, t3
    and     t1, 0x0f
    add     a0, t1, '0'
    blt     t1, t2, 2f
    add     a0, t1, 'A'-10
2:  bal     _putb
    bne     t3, zero, 1b
    jr      t7

/*** 
 * _show_int : show a unsigned decimal interger value
 *
 * register: a0 as input
 * register: t0,t7 will be destoried
 * register: ra is return link
 */

_puts:
    move    t7, ra
    move    t0, a0
1:  lb      a0, 0(t0)
    beqz    a0, 2f
    bal     _putb
    addu    t0, 1
    b       1b

2:  jr      t7


MsgMarch:		.string	"March SRAM: "
MsgMarchResult: .string "E result ["
MsgMarchBegin:	.string	"\nMarch test is beginning...\n"
MsgMarchEnd:	.string "March test is finished!\n"
MsgMarchPass:	.string "\nMemory Cell Test is Passed\n"

MsgSAF:		    .string	"\nError: Stuck-at Fault\n"
MsgCFin:	    .string	"\nError: Inversion Coupling Fault\n"
MsgCFst:	    .string	"\nError: Status Coupling Fault\n"
MsgOther:	    .string "\nError: Undistinguished Fault\n"
__xfmt:         .string "%08x\n"
__dfmt:         .string "%d\n"
__sfmt:         .string "%s"

CycleCount:	    .string	"\nCycle Count: "

__funcend:

