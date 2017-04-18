/**
 *  @file   command.h
 *  @brief  Event definition
 *  $Id: command.h,v 1.4 2014/10/23 12:49:47 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/04/20  C.N.Yu  New file.
 *
 */
#ifndef _COMMAND_H_
#define _COMMAND_H_

#define CMD_BASE        0x1000
#define CMD_STOP        (CMD_BASE + 0x01)
#define CMD_QUIT        (CMD_BASE + 0x02)
#define CMD_PREVIOUS    (CMD_BASE + 0x03)
#define CMD_NEXT        (CMD_BASE + 0x04)
#define CMD_PLAY        (CMD_BASE + 0x05)
#define CMD_PAUSE       (CMD_BASE + 0x06)
#define CMD_FFW         (CMD_BASE + 0x07)
#define CMD_FBW         (CMD_BASE + 0x08)
#define CMD_SNAPSHOT    (CMD_BASE + 0x09)
#define CMD_SNAPSHOT_SD (CMD_BASE + 0x0a)
#define CMD_PICVIEW     (CMD_BASE + 0x0b)
#define CMD_PICVIEW_SD  (CMD_BASE + 0x0c)
#define CMD_STEP        (CMD_BASE + 0x0d)
#define CMD_INDEX_ON    (CMD_BASE + 0x0e)
#define CMD_INDEX_OFF   (CMD_BASE + 0x0f)
#define CMD_DEBUG_ON    (CMD_BASE + 0x10)
#define CMD_DEBUG_OFF   (CMD_BASE + 0x11)
#define CMD_REC         (CMD_BASE + 0x12)
#define CMD_PLAY_NOR    (CMD_BASE + 0x13)
#define CMD_PLAY_SD     (CMD_BASE + 0x14)
#define CMD_REC_NOR     (CMD_BASE + 0x15)
#define CMD_REC_SD      (CMD_BASE + 0x16)

#define CMD_STOP_ERR	(CMD_BASE + 0x17)
#define CMD_DELETE		(CMD_BASE + 0x18)
#define CMD_DOORBELL	(CMD_BASE + 0x19)
#define CMD_CVD_NTSC	(CMD_BASE + 0x1A)
#define CMD_CVD_PAL		(CMD_BASE + 0x1B)
#define CMD_CVD_LOST	(CMD_BASE + 0x1C)

#define CMD_MD_VID		(CMD_BASE + 0x1D)

#define CMD_QUIT_ERR    (CMD_BASE + 0x1E)

#endif /* _COMMAND_H_ */
