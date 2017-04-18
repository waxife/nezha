/**
 *  @file   event.h
 *  @brief  Event definition
 *  $Id: event.h,v 1.3 2014/01/11 08:29:14 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/04/20  C.N.Yu  New file.
 *
 */
#ifndef _EVENT_H_
#define _EVENT_H_

/* system */
#define SYS_EVENT_OK            0x10
#define SYS_EVENT_ERR           0x11

/* record */
#define REC_EVENT_OK            0x20
#define REC_EVENT_ERR           0x21
#define REC_EVENT_FULL          0x22
#define REC_EVENT_STOP          0x23

/* playback */
#define PLAY_EVENT_OK           0x30
#define PLAY_EVENT_ERR          0x31
#define PLAY_EVENT_SOF          0x32
#define PLAY_EVENT_EOF          0x33
#define PLAY_EVENT_INV_FILE     0x34
#define PLAY_EVENT_FRAME_IDX    0x35

/* motion detection */
#define MD_EVENT_OK             0x40
#define MD_EVENT_ERR            0x41
#define MD_EVENT_DETECT         0x42
#define MD_EVENT_CANCEAL        0x43

/* snapshot */
#define SNAP_EVENT_OK           0x50
#define SNAP_EVENT_ERR          0x51
#define SNAP_EVENT_CAPTURED     0x52
#define SNAP_EVENT_FULL         0x53

/* picture viewer */
#define PVIEW_EVENT_OK          0x60
#define PVIEW_EVENT_ERR         0x61
#define PVIEW_EVENT_EMPTY       0x62

/* aumsg */
#if 0
#define AUMSG_EVENT_OK         0x70
#define AUMSG_EVENT_ERR        0x71
#define AUMSG_EVENT_DONE       0x72
#define AUMSG_EVENT_FULL       0x73
#define AUMSG_EVENT_EMPTY      0x74
#define AUMSG_EVENT_TIME       0x75
#endif

#endif /* _EVENT_H_ */
