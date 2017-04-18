/**
 *  @file   audio.h
 *  @brief  header file of audio encode/decode driver
 *  $Id: ringbell.h,v 1.5 2014/03/07 03:34:19 lym Exp $
 *  $Author: lym $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/09/03  hugo     New file.
 *
 */
#ifndef __RINGBELL_H
#define __RINGBELL_H

/******************************************************************************
 * Ringbell API
 ******************************************************************************/
//#define DEC_BUFDESC_SIZE    2048
//#define DEC_BUFDESC_NUM     4

#define ringbell_idle 0
#define ringbell_play 1
#define ringbell_stop 2

#define ERROR_BUSY   (-1)
#define ERROR_FORMAT (-2)

int stopbell(void);
int ringbell(int idx,int count);
int ringbell_status(void);

#endif /* __RINGBELL_H */
