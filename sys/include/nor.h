/**
 *  @file   nor.h
 *  @brief  nor flash api
 *  $Id: nor.h,v 1.5 2015/06/17 03:03:14 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/03/15  New file.
 *
 */

#ifndef __NOR_H
#define __NOR_H

#include <stdint.h>

#define NOR_DATA_BASE   0x80000000
#define NOR_DATA_LEN    0x01000000

struct norc_t;

/* nor2.c */
extern int nor_init(struct norc_t *norc, char *layout);
extern int ba2pa(struct norc_t *norc, int ba);
extern int nor_sector_erase(int pa);
extern int nor_block_erase(int pa);
extern int nor_data_write(int pa, int wsize, char *data);
extern int nor_manuid(void);
extern int nor_deviceid(void);
extern int nor_wren(void);
extern int nor_rdsr(int *state);
extern int nor_wrsr(unsigned int value);
extern int nor_is_writeprotect(void);
extern void nor_writeprotect(char enable);



#if 0
struct nor_flash {    
    int (* block_erase_spi)(int pa);    
    int (* data_write_spi)(int pa, int wsize, char *data);
    int (* nor_chiperase)(void);      
    void (* norreset)(void);       
};
#endif

/*
 * NOTE: store photo number restriction, (pi_max - win_size) >= 2 
 */
struct norc_t {
    int magic; 
    //uint16_t ppi;       /* 1: PPI or 0: SPI */      
    struct layout_t {
        int          ba;
        int          pa;
        int          ba_size;           /* 0 indicate end of layout array */
    } layout[16];
    uint32_t         capacity;
    int              last_ba;
    int32_t          wp_cnt;            /* write protect count */
#if 0    
    uint16_t         pic_no;            /* pic no. */
                                        /* 1 ba == 2 idx, 1 block store 2 images */
    uint8_t          ba_start;          /* pic start ba */
    uint8_t          ba_end;            /* pic end ba */
    uint8_t          wr_idx;            /* write idx (logical) */
    uint8_t          rd_idx;            /* read idx (logical) */
    uint8_t          unview_idx;        /* unview idx (logical) */
    uint8_t          photo_cnt;         /* photo count */
    uint8_t          unview_cnt;        /* pic unview count */
    uint8_t          bad_cnt;           /* bad pics count */
    uint8_t          win_size;          /* size of slide window (max pic number) */
    uint8_t          pi_log0;           /* physical index of logical index 0 */
    uint8_t          num_reward;        /* number to window end from logical 0 */
    uint8_t          pi_max;            /* physical index max */
    uint8_t          ps;                /* slide window start physical position */
    uint8_t          pe;                /* slide window end physical position */
    uint8_t          is;                /* slide window start logical index */
    uint8_t          db_error;          /* photo db error */
    uint8_t          storage_type;      /* storage type(0: 2 photo/1 block, 1: 1 photo/ 1 block */
    uint8_t          reserved;          /* reserved */
    unsigned char    *cache;
    int              cache_size;
    int              cache_ba;
    int              cache_dirty;
#endif
};

#endif /* __NOR_H */
