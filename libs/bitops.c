/*
 * This file is subject to the terms and conditions of the GNU General Public
 * License.  See the file "COPYING" in the main directory of this archive
 * for more details.
 *
 * Copyright (c) 1994 - 1997, 1999, 2000  Ralf Baechle (ralf@gnu.org)
 * Copyright (c) 1999, 2000  Silicon Graphics, Inc.
 */
#include <config.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <mipsregs.h>
#include <io.h>
#include "ptrace.h"
#include "cache.h"


#define SZLONG_LOG 5
#define SZLONG_MASK 31UL
#define __LL	"ll	"
#define __SC	"sc	"

/*
 * test_and_set_bit - Set a bit and return its old value
 * @nr: Bit to set
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
__mips32__
int test_and_set_bit(unsigned long nr,
	volatile unsigned long *addr)
{	
	unsigned long *m = ((unsigned long *) addr) + (nr >> SZLONG_LOG);
	unsigned long temp, res;

	__asm__ __volatile__(
	"	.set	noreorder	# test_and_set_bit	\n"
	"1:	" __LL "%0, %1					\n"
	"	or	%2, %0, %3				\n"
	"	" __SC	"%2, %1					\n"
	"	beqz	%2, 1b					\n"
	"	 and	%2, %0, %3				\n"
	".set\treorder"
	: "=&r" (temp), "=m" (*m), "=&r" (res)
	: "r" (1UL << (nr & SZLONG_MASK)), "m" (*m)
	: "memory");

	return res != 0;
}

/*
 * test_and_clear_bit - Clear a bit and return its old value
 * @nr: Bit to clear
 * @addr: Address to count from
 *
 * This operation is atomic and cannot be reordered.
 * It also implies a memory barrier.
 */
__mips32__
int test_and_clear_bit(unsigned long nr,
	volatile unsigned long *addr)
{
	unsigned long *m = ((unsigned long *) addr) + (nr >> SZLONG_LOG);
	unsigned long temp, res;

	__asm__ __volatile__(
	"	.set	noreorder	# test_and_clear_bit	\n"
	"1:	" __LL	"%0, %1					\n"
	"	or	%2, %0, %3				\n"
	"	xor	%2, %3					\n"
		__SC 	"%2, %1					\n"
	"	beqz	%2, 1b					\n"
	"	 and	%2, %0, %3				\n"
	"	.set	reorder					\n"
	: "=&r" (temp), "=m" (*m), "=&r" (res)
	: "r" (1UL << (nr & SZLONG_MASK)), "m" (*m)
	: "memory");

	return res != 0;
}


/*
 * clear_bit - Clears a bit in memory
 * @nr: Bit to clear
 * @addr: Address to start counting from
 *
 * clear_bit() is atomic and may not be reordered.  However, it does
 * not contain a memory barrier, so if it is used for locking purposes,
 * you should call smp_mb__before_clear_bit() and/or smp_mb__after_clear_bit()
 * in order to ensure changes are visible on other processors.
 */
__mips32__
void clear_bit(unsigned long nr, volatile unsigned long *addr)
{
	unsigned long *m = ((unsigned long *) addr) + (nr >> SZLONG_LOG);
	unsigned long temp;

	__asm__ __volatile__(
	"1:	" __LL "%0, %1			# clear_bit	\n"
	"	and	%0, %2					\n"
	"	" __SC "%0, %1					\n"
	"	beqz	%0, 1b					\n"
	: "=&r" (temp), "=m" (*m)
	: "ir" (~(1UL << (nr & SZLONG_MASK))), "m" (*m));
}
