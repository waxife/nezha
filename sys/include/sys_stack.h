/**
 *  @file   sys_stack.h
 *  @brief  stack relative information and macros 
 *  $Id: sys_stack.h,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
#ifndef __SYS_STACK_H
#define __SYS_STACK_H

extern unsigned int __STACK_BARRIER;
extern unsigned int _stack;
extern unsigned int get_sp(void);

#define stack_barrier()     ((unsigned int)(&__STACK_BARRIER))
#define stack_base()        ((unsigned int)(&_stack))
#define stack_size()        (stack_base() - stack_barrier())
#define stack_deeper()      (stack_base() - get_sp())

#define ASSERT_UNDER_STACK()   assert(get_sp() - 512  > (uint32_t)(&__STACK_BARRIER))


#endif /* __SYS_STACK_H */
