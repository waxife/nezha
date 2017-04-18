/**
 *  @file   stdatomic.h
 *  @brief  atomic operation
 *  $Id: stdatomic.h,v 1.3 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.3 $
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

#ifndef __STDATOMIC_H
#define __STDATOMIC_H
#include <linker.h> 
#include <stdint.h>

extern __mips32__ uint8_t atomic_flag_test_and_set_1(volatile uint8_t *mem, uint8_t val);
extern __mips32__ uint16_t atomic_flag_test_and_set_2(volatile uint16_t *mem, uint16_t val);
extern __mips32__ uint32_t atomic_flag_test_and_set_4(volatile uint32_t *mem, uint32_t val);

extern __mips32__ uint8_t atomic_val_compare_and_swap_1(volatile uint8_t *mem, uint8_t excepted, uint8_t desired);
extern __mips32__ uint16_t atomic_val_compare_and_swap_2(volatile uint16_t *mem, uint16_t excepted, uint16_t desired);
extern __mips32__ uint32_t atomic_val_compare_and_swap_4(volatile uint32_t *mem, uint32_t excepted, uint32_t desired);

extern __mips32__ uint8_t atomic_fetch_add_1(volatile uint8_t *mem, uint8_t val);
extern __mips32__ uint16_t atomic_fetch_add_2(volatile uint16_t *mem, uint16_t val);
extern __mips32__ uint32_t atomic_fetch_add_4(volatile uint32_t *mem, uint32_t val);

extern __mips32__ uint8_t atomic_fetch_sub_1(volatile uint8_t *mem, uint8_t val);
extern __mips32__ uint16_t atomic_fetch_sub_2(volatile uint16_t *mem, uint16_t val);
extern __mips32__ uint32_t atomic_fetch_sub_4(volatile uint32_t *mem, uint32_t val);

extern __mips32__ uint8_t atomic_fetch_and_1(volatile uint8_t *mem, uint8_t val);
extern __mips32__ uint16_t atomic_fetch_and_2(volatile uint16_t *mem, uint16_t val);
extern __mips32__ uint32_t atomic_fetch_and_4(volatile uint32_t *mem, uint32_t val);

extern __mips32__ uint8_t atomic_fetch_or_1(volatile uint8_t *mem, uint8_t val);
extern __mips32__ uint16_t atomic_fetch_or_2(volatile uint16_t *mem, uint16_t val);
extern __mips32__ uint32_t atomic_fetch_or_4(volatile uint32_t *mem, uint32_t val);
extern __mips32__ uint8_t atomic_fetch_xor_1(volatile uint8_t *mem, uint8_t val);
extern __mips32__ uint16_t atomic_fetch_xor_2(volatile uint16_t *mem, uint16_t val);
extern __mips32__ uint32_t atomic_fetch_xor_4(volatile uint32_t *mem, uint32_t val);


typedef volatile uint8_t    atomic_1_t;
typedef volatile uint16_t   atomic_2_t;
typedef volatile uint32_t   atomic_4_t;



#endif /* __STDATOMIC_H */



