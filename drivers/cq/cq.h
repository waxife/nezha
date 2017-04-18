/**
 *  @file   cq.h
 *  @brief  head file of command queue
 *  $Id: cq.h,v 1.1.1.1 2013/12/18 03:43:49 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/06/04  hugo	New file.
 *
 */

#ifndef __CQ_H
#define __CQ_H

#define CQ_MAX_SIZE (128 - 1)

/* error codes */
#define CQ_ERR_UNKNOWN          0x10    /* unknown interrupt status */
#define CQ_ERR_QUEUE_OVERFLOW   0x11    /* queue overflow */
#define CQ_ERR_TRIGGER_TIMEOUT  0x12    /* wait trigger signal timeout */
#define CQ_ERR_INVALID_CMD      0x13    /* invalid command */
#define CQ_ERR_EMPTY            0x20    /* issue empty command queue */
#define CQ_ERR_LENGTH           0x21    /* illegal data length */
#define CQ_ERR_CMD_OVERFLOW     0x22    /* too many data for a command */
#define CQ_ERR_UNFINISHED       0x23    /* the last command not finished */
#define CQ_ERR_ISSUE_TIMEOUT    0x24    /* wait issue done timeout */
#define CQ_ERR_ARGUMENT         0x25    /* illegal arguments */

extern unsigned char cq_debug;
#define CQ_DEBUG_SET_REG    	(1 << 0)
#define CQ_DEBUG_DATAPORT   	(1 << 1)
#define CQ_DEBUG_ISSUE      	(1 << 2)
#define CQ_DEBUG_LINECNT    	(1 << 3)
#define CQ_DEBUG_CONFIG     	(1 << 4)
#define CQ_DEBUG_WRITE      	(1 << 5)
#define CQ_DEBUG_REGTABLE   	(1 << 6)

/* trigger source */
#define CQ_TRIGGER_SW           0
#define CQ_TRIGGER_VSYNC        1
#define CQ_TRIGGER_VDE          2

/* define PAGE0~PAGE7 for CQ configure */
#define CQ_P0					0
#define CQ_P1					1
#define CQ_P2					2
#define CQ_P3					3
#define CQ_P4					4
#define CQ_P5					5
#define CQ_P6					6
#define CQ_P7					7

#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */
struct cmdhdr {
    unsigned char page;
    unsigned char addr;
    unsigned char incr;
    unsigned char size;
} __attribute__((packed));

struct regval {
    unsigned char addr;
    unsigned char value;
} __attribute__((packed));

#pragma pack(pop)

void cq_info (void);
void cq_dump (void);

int cq_available (void);
int cq_config (unsigned char page, unsigned char addr, unsigned char incr, unsigned char len);
int cq_data (unsigned char byte);
int cq_flush (void);
int cq_flush_now (void);
int cq_flush_vsync (void);
int cq_flush_vde (void);

int cq_write_cmd (unsigned char const *cmd);
int cq_write_cmdset (unsigned char const *cmd[], unsigned char num);
void cq_trigger_source (unsigned char source);
void cq_trigger_delay (char enable, int count, char dly_in_hsync);
void cq_trigger_timeout (unsigned char count);
void cq_reset (void);
void cq_init (void);
void cq_isr (void);
int cq_busy (void);

int cq_write_byte(unsigned char bPage, unsigned char bAdd, unsigned char bData);
int cq_write_byte_issue(unsigned char bPage, unsigned char bAdd, unsigned char bData,unsigned char trig);
int cq_write_word(unsigned char bPage, unsigned char bAdd, unsigned short bData, unsigned char incr);
int cq_write_word_issue(unsigned char bPage, unsigned char bAdd, unsigned short bData, unsigned char incr,unsigned char trig);
int cq_WriteRegsTable(unsigned char const * RegsTable,unsigned char end_page);
int cq_WriteRegsTable_issue(unsigned char const * RegsTable,unsigned char end_page, unsigned char trig);
void cq_init_timeout(unsigned char timeout);

void cq_try_overflow (void);        /* test error handling of queue overflow */
void cq_try_trigger_timeout(void);  /* test error handling of trigger timeout */
void cq_try_invalid_cmd (void);     /* test error handling of invalid command */
void cq_try_issue_timeout (void);   /* test error handling of issue timeout */

#endif /* __CQ_H */
