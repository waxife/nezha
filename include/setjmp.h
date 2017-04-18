/**
 *  @file   setjmp.h
 *  @brief  setjmp/longjmp mips version
 *  $Id: setjmp.h,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/05  jedy  New file.
 *
 */

#ifndef __MIPS_SETJMP_H
#define __MIPS_SETJMP_H
#ifdef __mips

/* 
 * Must save: s0-s8, sp, ra (11 registers)
 * Don't change __JB_REGS without adjusting mips_setjmp.S accrodingly.
 */
#define __JB_REGS       11

/* A jmp buf is an array of __JB_REGS registers */
typedef unsigned long jmp_buf[__JB_REGS];

int setjmp(jmp_buf env);
void longjmp(jmp_buf env, int code);

#else
#include </usr/include/setjmp.h>
#endif

#endif /* __MIPS_SETJMP_H */
