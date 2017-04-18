/**
 *  @file   spinlock.c
 *  @brief  spinlock
 *  $Id: spinlock.c,v 1.1 2014/08/06 07:07:36 jedy Exp $
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

static char _spin = 0;
__mips32__
void spinlock(void)
{
    cli();
    _spin++;
}

__mips32__
void spinunlock(void)
{
    _spin--;
    if (_spin == 0)
        sti();
}

