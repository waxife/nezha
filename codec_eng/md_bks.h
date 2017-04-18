/*
 *  @file   md_bks.h
 *  @brief  header file for motion detection process
 *  $Id: md_bks.h,v 1.4 2015/08/06 01:35:48 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/02/18  YC New file.
 *
 */

#ifndef _MD_BKS_H_
#define _MD_BKS_H_

#include <stdint.h>
#define MDST_INIT           0
#define MDST_BACKGROUND0    1
#define MDST_BACKGROUND1    2
#define MDST_BACKGROUND2    3
#define MDST_BACKGROUND3    4
#define MDST_BACKGROUND4    5
#define MDST_DETECT         6

#define MD_BLOCK_SIZE		8
#define MDTH_CONTX_MAX      2
#define MDTH_CONTY_MAX      1	
#define MDTH_CONTXY_MAX	    5
#define MDTH_SEGMENTX       2
#define MDTH_SEGMENTY       2
#define MDTH_SEGMENTXY      3
#define MDTH_DIFFRATIO      1
#define MDTH_MOTIONCOUNT    1
#define MDTH_MARGIN	        0
#define MD_REC_SECS         10
#define MD_FRAME_WIDTH      80
#define MD_FRAME_HEIGHT     60 
#define MD_FRAMEBUF_SIZE    4800
#define MD_FIELD_WIDTH      90
#define MD_FIELD_HEIGHT     30
#define MD_FIELDBUF_SIZE    2700
#define NUM_BUFFER          4
#define PIXDIFF_THRESHOLD   10
#define MD_FRAME_MODE       0
#define MD_FIELD_MODE       1

#define MDBKS_BASE       	0x1100
#define MDBKS_NONE			(MDBKS_BASE + 0x01)
#define MDBKS_DETECT       	(MDBKS_BASE + 0x02)
#define MDBKS_EROOR			(MDBKS_BASE + 0x03)





#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */
struct mdbks_t{
    uint16_t fno;
    uint16_t end_fno;
#if CONFIG_RESERVE_CODEC_BUFFER
    uint8_t *cur_frame;
    uint8_t *background;
#else
    uint8_t cur_frame[MD_FRAMEBUF_SIZE];
    uint8_t background[MD_FRAMEBUF_SIZE];
#endif
    uint32_t background_avg;
    uint32_t last_addr;
    uint32_t buf_avg;
    uint16_t diffcount;
    uint8_t state;
    uint8_t motioncount;
    uint8_t mode;
    uint8_t started;  
    uint8_t mt_x;
    uint8_t mt_y;
    uint8_t mt_w;
    uint8_t mt_h;
    uint8_t fwidth;
    uint8_t fheight;
    uint16_t fsize;
    uint8_t th_contx;
    uint8_t th_conty;
    uint8_t th_contxy;
    uint8_t th_segx;
    uint8_t th_segy;
    uint8_t th_segxy;
    uint8_t th_diffratio;    
    uint8_t th_motioncount;
    uint8_t th_recsecs;
    uint8_t th_recdetframes;
    uint8_t th_pixeldiff;
    uint8_t th_margin;
    uint16_t dbg1;
    uint16_t dbg2;
    uint16_t dbg3;
    uint8_t rangex;
	uint8_t rangey;
	uint8_t rangew;
	uint8_t rangeh;
    uint8_t sensitive;
    uint8_t detect_channels;
    uint8_t md_channel;
} __attribute__((packed));

#pragma pack(pop)

int mdbks_init(struct mdbks_t* md);
int mdbks_process(struct mdbks_t* md);
int mdbks_release(struct mdbks_t *md);
int mdbks_process_swich_ch(struct mdbks_t* md);

#endif /* _MD_BKS_H_ */

