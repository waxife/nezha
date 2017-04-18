/**
 *  @file   spinlock.h
 *  @brief  spinlock
 *  $Id: spinlock.h,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */

#ifndef __SPINLOCK_H
#define __SPINLOCK_H

#include <stdio.h>

__mips32__ void spinlock(void);
__mips32__ void spinunlock(void);


#endif /* __SPINLOCK_H */
