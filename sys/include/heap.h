/**
 *  @file   heap.h
 *  @brief  heap memory managment functions
 *  $Id: heap.h,v 1.2 2014/03/13 10:31:35 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/03/15  New file.
 *
 */

#ifndef __HEAP_H
#define __HEAP_H

int heap_create(char *memory, int size);
char *__heap_alloc(const char *func, int lno, int size);
int  __heap_release(const char *func, int lno, char *p);

#define heap_alloc(size)    __heap_alloc(__FUNCTION__, __LINE__, size)
#define heap_release(p)      __heap_release(__FUNCTION__, __LINE__, p) 


#endif /* __HEAP_H */
