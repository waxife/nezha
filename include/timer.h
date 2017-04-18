/**
 *  @file   timer.h
 *  @brief  provide current time 
 *  $Id: timer.h,v 1.2 2014/07/11 11:00:10 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#ifndef __TIMER_H__
#define __TIMER_H__


#include <io.h>
#define CURRENT	0

#define T30X_OSTIMER_BASE   (0xb1c00000)
#define T30X_OSTIMER_OSMR0  (void *)(T30X_OSTIMER_BASE + 0x00)
#define T30X_OSTIMER_OSMR1  (void *)(T30X_OSTIMER_BASE + 0x04)
#define T30X_OSTIMER_OSMR2  (void *)(T30X_OSTIMER_BASE + 0x08)
#define T30X_OSTIMER_OSMR3  (void *)(T30X_OSTIMER_BASE + 0x0c)
#define T30X_OSTIMER_OSCCD  (void *)(T30X_OSTIMER_BASE + 0x10)
#define T30X_OSTIMER_OSCR   (void *)(T30X_OSTIMER_BASE + 0x14)
#define T30X_OSTIMER_OSWER  (void *)(T30X_OSTIMER_BASE + 0x18)
#define T30X_OSTIMER_OSIMR  (void *)(T30X_OSTIMER_BASE + 0x1c)
#define T30X_OSTIMER_OSSR   (void *)(T30X_OSTIMER_BASE + 0x20)
#define OSIMR(N)            (1 << (N))
#define OSSR(N)             (1 << (N))

#endif // __TIMER_H__
