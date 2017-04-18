/**
 *  @file   stdatomic.c
 *  @brief  atomic operation
 *  $Id: stdatomic.c,v 1.2 2014/03/07 07:00:18 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/02/22  jedy    New file.
 *
 */

/*-
 * Copyright (c) 2013 Ed Schouten <ed@FreeBSD.org>
 * All rights reserved.
 *
 * Copyright (c) 1998 Doug Rabson
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include <stdio.h>
#include <sys.h>
#include <stdint.h>

/*
 * Memory barriers.
 *
 * It turns out __sync_synchronize() does not emit any code when used
 * with GCC 4.2. Implement our own version that does work reliably.
 *
 * Although __sync_lock_test_and_set() should only perform an acquire
 * barrier, make it do a full barrier like the other functions. This
 * should make <stdatomic.h>'s atomic_exchange_explicit() work reliably.
 */

#define do_sync()       /* do nothing */


typedef union {
        uint8_t         v8[4];
        uint32_t        v32;
} reg_t;

/*
 * Given a memory address pointing to an 8-bit or 16-bit integer, return
 * the address of the 32-bit word containing it.
 */

static inline uint32_t *
round_to_word(void *ptr)
{

        return ((uint32_t *)((intptr_t)ptr & ~3));
}

/*
 * Utility functions for loading and storing 8-bit and 16-bit integers
 * in 32-bit words at an offset corresponding with the location of the
 * atomic variable.
 */

static inline void
put_1(reg_t *r, const uint8_t *offset_ptr, uint8_t val)
{
        size_t offset;

        offset = (intptr_t)offset_ptr & 3;
        r->v8[offset] = val;
}

static inline uint8_t
get_1(const reg_t *r, const uint8_t *offset_ptr)
{
        size_t offset;

        offset = (intptr_t)offset_ptr & 3;
        return (r->v8[offset]);
}

static inline void
put_2(reg_t *r, const uint16_t *offset_ptr, uint16_t val)
{
        size_t offset;
        union {
                uint16_t in;
                uint8_t out[2];
        } bytes;

        offset = (intptr_t)offset_ptr & 3;
        bytes.in = val;
        r->v8[offset] = bytes.out[0];
        r->v8[offset + 1] = bytes.out[1];
}

static inline uint16_t
get_2(const reg_t *r, const uint16_t *offset_ptr)
{
        size_t offset;
        union {
                uint8_t in[2];
                uint16_t out;
        } bytes;

        offset = (intptr_t)offset_ptr & 3;
        bytes.in[0] = r->v8[offset];
        bytes.in[1] = r->v8[offset + 1];
        return (bytes.out);
}

/*
 * 8-bit and 16-bit routines.
 *
 * These operations are not natively supported by the CPU, so we use
 * some shifting and bitmasking on top of the 32-bit instructions.
 */

#define EMIT_LOCK_TEST_AND_SET_N(N, uintN_t)                            \
__mips32__ uintN_t                                                      \
atomic_flag_test_and_set_##N(uintN_t *mem, uintN_t val)                 \
{                                                                       \
        uint32_t *mem32;                                                \
        reg_t val32, negmask, old;                                      \
        uint32_t temp;                                                  \
                                                                        \
        mem32 = round_to_word(mem);                                     \
        val32.v32 = 0x00000000;                                         \
        put_##N(&val32, mem, val);                                      \
        negmask.v32 = 0xffffffff;                                       \
        put_##N(&negmask, mem, 0);                                      \
                                                                        \
        do_sync();                                                      \
        __asm volatile (                                                \
                "1:"                                                    \
                "\tll   %0, %5\n"       /* Load old value. */           \
                "\tand  %2, %4, %0\n"   /* Remove the old value. */     \
                "\tor   %2, %3\n"       /* Put in the new value. */     \
                "\tsc   %2, %1\n"       /* Attempt to store. */         \
                "\tbeqz %2, 1b\n"       /* Spin if failed. */           \
                : "=&r" (old.v32), "=m" (*mem32), "=&r" (temp)          \
                : "r" (val32.v32), "r" (negmask.v32), "m" (*mem32));    \
        return (get_##N(&old, mem));                                    \
}

EMIT_LOCK_TEST_AND_SET_N(1, uint8_t)
EMIT_LOCK_TEST_AND_SET_N(2, uint16_t)

#define EMIT_VAL_COMPARE_AND_SWAP_N(N, uintN_t)                         \
__mips32__ uintN_t                                                      \atomic_val_compare_and_swap_##N(uintN_t *mem, uintN_t expected,         \
    uintN_t desired)                                                    \
{                                                                       \
        uint32_t *mem32;                                                \
        reg_t expected32, desired32, posmask, old;                      \
        uint32_t negmask, temp;                                         \
                                                                        \
        mem32 = round_to_word(mem);                                     \
        expected32.v32 = 0x00000000;                                    \
        put_##N(&expected32, mem, expected);                            \
        desired32.v32 = 0x00000000;                                     \
        put_##N(&desired32, mem, desired);                              \
        posmask.v32 = 0x00000000;                                       \
        put_##N(&posmask, mem, ~0);                                     \
        negmask = ~posmask.v32;                                         \
                                                                        \
        do_sync();                                                      \
        __asm volatile (                                                \
                "1:"                                                    \
                "\tll   %0, %7\n"       /* Load old value. */           \
                "\tand  %2, %5, %0\n"   /* Isolate the old value. */    \
                "\tbne  %2, %3, 2f\n"   /* Compare to expected value. */\
                "\tand  %2, %6, %0\n"   /* Remove the old value. */     \
                "\tor   %2, %4\n"       /* Put in the new value. */     \
                "\tsc   %2, %1\n"       /* Attempt to store. */         \
                "\tbeqz %2, 1b\n"       /* Spin if failed. */           \
                "2:"                                                    \
                : "=&r" (old), "=m" (*mem32), "=&r" (temp)              \
                : "r" (expected32.v32), "r" (desired32.v32),            \
                  "r" (posmask.v32), "r" (negmask), "m" (*mem32));      \
        return (get_##N(&old, mem));                                    \
}

EMIT_VAL_COMPARE_AND_SWAP_N(1, uint8_t)
EMIT_VAL_COMPARE_AND_SWAP_N(2, uint16_t)

#define EMIT_ARITHMETIC_FETCH_AND_OP_N(N, uintN_t, name, op)            \
__mips32__ uintN_t                                                                 \
atomic_##name##_##N(uintN_t *mem, uintN_t val)                          \
{                                                                       \
        uint32_t *mem32;                                                \
        reg_t val32, posmask, old;                                      \
        uint32_t negmask, temp1, temp2;                                 \
                                                                        \
        mem32 = round_to_word(mem);                                     \
        val32.v32 = 0x00000000;                                         \
        put_##N(&val32, mem, val);                                      \
        posmask.v32 = 0x00000000;                                       \
        put_##N(&posmask, mem, ~0);                                     \
        negmask = ~posmask.v32;                                         \
                                                                        \
        do_sync();                                                      \
        __asm volatile (                                                \
                "1:"                                                    \
                "\tll   %0, %7\n"       /* Load old value. */           \
                "\t"op" %2, %0, %4\n"   /* Calculate new value. */      \
                "\tand  %2, %5\n"       /* Isolate the new value. */    \
                "\tand  %3, %6, %0\n"   /* Remove the old value. */     \
                "\tor   %2, %3\n"       /* Put in the new value. */     \
                "\tsc   %2, %1\n"       /* Attempt to store. */         \
                "\tbeqz %2, 1b\n"       /* Spin if failed. */           \
                : "=&r" (old.v32), "=m" (*mem32), "=&r" (temp1),        \
                  "=&r" (temp2)                                         \
                : "r" (val32.v32), "r" (posmask.v32), "r" (negmask),    \
                  "m" (*mem32));                                        \
        return (get_##N(&old, mem));                                    \
}

EMIT_ARITHMETIC_FETCH_AND_OP_N(1, uint8_t, fetch_add, "addu")
EMIT_ARITHMETIC_FETCH_AND_OP_N(1, uint8_t, fetch_sub, "subu")
EMIT_ARITHMETIC_FETCH_AND_OP_N(2, uint16_t, fetch_add, "addu")
EMIT_ARITHMETIC_FETCH_AND_OP_N(2, uint16_t, fetch_sub, "subu")

#define EMIT_BITWISE_FETCH_AND_OP_N(N, uintN_t, name, op, idempotence)  \
__mips32__ uintN_t                                                      \
atomic_##name##_##N(uintN_t *mem, uintN_t val)                          \
{                                                                       \
        uint32_t *mem32;                                                \
        reg_t val32, old;                                               \
        uint32_t temp;                                                  \
                                                                        \
        mem32 = round_to_word(mem);                                     \
        val32.v32 = idempotence ? 0xffffffff : 0x00000000;              \
        put_##N(&val32, mem, val);                                      \
                                                                        \
        do_sync();                                                      \
        __asm volatile (                                                \
                "1:"                                                    \
                "\tll   %0, %4\n"       /* Load old value. */           \
                "\t"op" %2, %3, %0\n"   /* Calculate new value. */      \
                "\tsc   %2, %1\n"       /* Attempt to store. */         \
                "\tbeqz %2, 1b\n"       /* Spin if failed. */           \
                : "=&r" (old.v32), "=m" (*mem32), "=&r" (temp)          \
                : "r" (val32.v32), "m" (*mem32));                       \
        return (get_##N(&old, mem));                                    \
}

EMIT_BITWISE_FETCH_AND_OP_N(1, uint8_t, fetch_and, "and", 1)
EMIT_BITWISE_FETCH_AND_OP_N(1, uint8_t, fetch_or, "or", 0)
EMIT_BITWISE_FETCH_AND_OP_N(1, uint8_t, fetch_xor, "xor", 0)
EMIT_BITWISE_FETCH_AND_OP_N(2, uint16_t, fetch_and, "and", 1)
EMIT_BITWISE_FETCH_AND_OP_N(2, uint16_t, fetch_or, "or", 0)
EMIT_BITWISE_FETCH_AND_OP_N(2, uint16_t, fetch_xor, "xor", 0)

/*
 * 32-bit routines.
 */

__mips32__ uint32_t
atomic_val_compare_and_swap_4(uint32_t *mem, uint32_t expected,
    uint32_t desired)
{
        uint32_t old, temp;

        do_sync();
        __asm volatile (
                "1:"
                "\tll   %0, %5\n"       /* Load old value. */
                "\tbne  %0, %3, 2f\n"   /* Compare to expected value. */
                "\tmove %2, %4\n"       /* Value to store. */
                "\tsc   %2, %1\n"       /* Attempt to store. */
                "\tbeqz %2, 1b\n"       /* Spin if failed. */
                "2:"
                : "=&r" (old), "=m" (*mem), "=&r" (temp)
                : "r" (expected), "r" (desired), "m" (*mem));
        return (old);
}

#define EMIT_FETCH_AND_OP_4(name, op)                                   \
__mips32__ uint32_t                                                                \
atomic_##name##_4(uint32_t *mem, uint32_t val)                          \
{                                                                       \
        uint32_t old, temp;                                             \
                                                                        \
        do_sync();                                                      \
        __asm volatile (                                                \
                "1:"                                                    \
                "\tll   %0, %4\n"       /* Load old value. */           \
                "\t"op"\n"              /* Calculate new value. */      \
                "\tsc   %2, %1\n"       /* Attempt to store. */         \
                "\tbeqz %2, 1b\n"       /* Spin if failed. */           \
                : "=&r" (old), "=m" (*mem), "=&r" (temp)                \
                : "r" (val), "m" (*mem));                               \
        return (old);                                                   \
}

EMIT_FETCH_AND_OP_4(flag_test_and_set, "move %2, %3")
EMIT_FETCH_AND_OP_4(fetch_add, "addu %2, %0, %3")
EMIT_FETCH_AND_OP_4(fetch_and, "and %2, %0, %3")
EMIT_FETCH_AND_OP_4(fetch_or, "or %2, %0, %3")
EMIT_FETCH_AND_OP_4(fetch_sub, "subu %2, %0, %3")
EMIT_FETCH_AND_OP_4(fetch_xor, "xor %2, %0, %3")


