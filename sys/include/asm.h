/**
 *  @file   asm.h
 *  @brief  assembly helper marco
 *  $Id: asm.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/01/12  New file.
 *
 */

#ifndef __ASM_H
#define __ASM_H

/*
 * LEAF - declare leaf routine
 */
#define	LEAF(symbol)                            \
		.globl	symbol;                         \
		.align	2;                              \
		.type	symbol,@function;               \
		.ent	symbol,0;                       \
symbol:		.frame	sp,0,ra

/*
 * NESTED - declare nested routine entry point
 */
#define	NESTED(symbol, framesize, rpc)          \
		.globl	symbol;                         \
		.align	2;                              \
		.type	symbol,@function;               \
		.ent	symbol,0;                       \
symbol:		.frame	sp, framesize, rpc


/*
 * END - mark end of function
 */
#define	END(function)                   \
		.end	function;		        \
		.size	function,.-function

/*
 * EXPORT - export definition of symbol
 */
#define EXPORT(symbol)					\
		.globl	symbol;                 \
symbol:

/*
 * FEXPORT - export definition of a function symbol
 */
#define FEXPORT(symbol)				    \
		.globl	symbol;				    \
		.type	symbol,@function;		\
symbol:

/*
 * ABS - export absolute symbol
 */
#define	ABS(symbol,value)               \
		.globl	symbol;                 \
symbol		=	value

#define	PANIC(msg)                      \
		.set	push;				    \
		.set	reorder;                \
		la	    a0,8f;                  \
		jal	    panic;                  \
9:		b	    9b;                     \
		.set	pop;				    \
		TEXT(msg)

/*
 * Print formatted string
 */
#define PRINT(string)                   \
		.set	push;				    \
		.set	reorder;                \
		la      a0,8f;                  \
		jalx	printf16;               \
		.set	pop;				    \
		TEXT(string)

#define	TEXT(msg)                       \
		.pushsection .data;			    \
8:		.asciiz	msg;                    \
		.popsection;




#endif /* __ASM_H */
