/**
 *  @file   debug.h
 *  @brief  debug message 
 *  $Id: debug.h,v 1.4 2014/05/29 06:05:21 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/05  jedy  New file.
 *
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#ifndef DBG_LEVEL
#define DBG_LEVEL   2
#endif

#ifndef DBG_PRINT
#include <stdio.h>
#define DBG_PRINT   printf
#endif

#ifndef DBG_ABORT
#include <stdlib.h>
#define DBG_ABORT   abort
#endif
 

#ifdef NDEBUG
#define assert(expr)    /* ({ if (!(expr)) DBG_ABORT(); }) */
#define dbg(lvl, fmt, arg...)   /* do nothing */

#else  /* ! defined(NDEBUG) */
#undef assert
#define assert(expr) \
    ({ if (! (expr)) { DBG_PRINT("assert failed: %s on %s %d\n", #expr, __FILE__, __LINE__); DBG_ABORT(); } })
#define dbg(lvl, fmt, arg...)  \
    ({if (lvl <= DBG_LEVEL) DBG_PRINT("%s:%d " fmt, __FILE__, __LINE__, ##arg);})
#endif /* NDEBUG */

#ifdef DBG_HELPER
#define INFO(fmt, arg...)   DBG_PRINT("%s:%d\x1b[32m[INFO]\x1b[0m" fmt, __FILE__,__LINE__,  ##arg)
#define WARN(fmt, arg...)   DBG_PRINT("%s:%d\x1b[33m[WARN]\x1b[0m" fmt, __FILE__,__LINE__,  ##arg)
#else
#define INFO(fmt, arg...)
#define WARN(fmt, arg...)
#endif

#define ERROR(fmt, arg...)  DBG_PRINT("%s:%d\x1b[31m[ERROR]\x1b[0m" fmt, __FILE__,__LINE__, ##arg)

#endif /* __DEBUG_H */
