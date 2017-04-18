/**
 *  @file   spinlock.c
 *  @brief  spinlock
 *  $Id: cpu_wait.c,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <sys.h>

__mips32__
void cpu_wait(void)
{
    __asm__ __volatile__ (
        ".set mips32r2;"
        "wait;"
    );
}
