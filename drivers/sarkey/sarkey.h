/**
 *  @file   sarkey.h
 *  @brief  sarkey interrupt handler 
 *  $Id: sarkey.h,v 1.8 2014/09/03 05:56:39 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#ifndef _SARKEY_H_
#define _SARKEY_H_

extern void sarkey_close(void);
extern void sarkey_init (void);
extern int sar1_read (void);
extern int sar2_read (void);
extern void sarkey_isr(void);
extern void sarkey_clear(void);
extern void sarkey2_clear(void);
extern void sarkey_init_table(const unsigned char (*sarkey)[2], unsigned char num);
extern void sarkey2_init_table(const unsigned char (*sarkey)[2], unsigned char num);
extern const unsigned char sarkey_evb_table[6][2];
extern unsigned char sar_status( ); 
extern int sar_sel(unsigned char sar_no,unsigned char pin_no);
#endif
