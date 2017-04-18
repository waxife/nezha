/**
 *  @file   ir.h
 *  @brief  ir interrupt header 
 *  $Id: ir.h,v 1.1.1.1 2013/12/18 03:43:42 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#ifndef _IR_H_
#define _IR_H_
extern void ir_close(void);
extern void ir_init(void);
extern int IRread(void);
extern void ir_isr(void);
#endif
