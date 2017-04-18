/**
 *  @file   linker.h
 *  @brief  section attribute for linker
 *  $Id: linker.h,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#ifndef __LINKER_H__
#define __LINKER_H__

#ifdef __mips

#define __mips16__  __attribute__((mips16))
#define __mips32__  __attribute__((nomips16, noinline))
#define __init__    __attribute__((section (".init.data"))) 

#else

#define __mips16__
#define __mips32__
#define __init__
#endif

#endif /* __STDIO_H__ */
