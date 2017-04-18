/**
 *  @file   ttl_driver.c
 *  @brief	TTL of driver
 *  $Id: ttl_driver.c,v 1.5 2014/01/22 08:21:33 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  kevin 	New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <sys.h>
#include <unistd.h>
#include <debug.h>
#include <iml.h>
#include "gpio.h"

#include "../../codec_eng/jreg.h"
#include "./drivers/cq/cq.h"

#define WRITEL(VAL, ADDR)   writel (VAL, ADDR)
#define READL(ADDR)         readl (ADDR)


void ttl_init_reg_tables(const struct reg* p0,int p0_size_t , const struct reg* p1, int p1_size_t, 
                         const struct reg* p2, int p2_size_t,  const struct reg* p3, int p3_size_t)
{
    int i;
    writel(0x00, 0xB0400E20);   
    if(p0){
        for (i = 0; i < p0_size_t; i++) {
    		cq_write_byte(CQ_P0, p0[i].addr, p0[i].value);
    	}
    }
    
    if(p1){
        for (i = 0; i < p1_size_t; i++) {
    		cq_write_byte(CQ_P1, p1[i].addr, p1[i].value);
    	}
    }
    
    if(p2){
        for (i = 0; i < p2_size_t; i++) {
    		cq_write_byte(CQ_P2, p2[i].addr, p2[i].value);
    	}
    }
    
    if(p3){
        for (i = 0; i < p3_size_t; i++) {
    		cq_write_byte(CQ_P3, p3[i].addr, p3[i].value);
    	}
    }
    
   
    if (cq_flush_now () < 0)
		ERROR ("cq_flush_now ()\n");
}

void ttl_set_reg_tables(const struct reg* p0,int p0_size_t , const struct reg* p1, int p1_size_t, 
                         const struct reg* p2, int p2_size_t,  const struct reg* p3, int p3_size_t)
{
    int i;
    writel(0x00, 0xB0400E20);
    
    if(p0){
        for (i = 0; i < p0_size_t; i++) {
    		cq_write_byte(CQ_P0, p0[i].addr, p0[i].value);
    	}
    }
    
    if(p1){
        for (i = 0; i < p1_size_t; i++) {
    		cq_write_byte(CQ_P1, p1[i].addr, p1[i].value);
    	}
    }
    
    if(p2){
        for (i = 0; i < p2_size_t; i++) {
    		cq_write_byte(CQ_P2, p2[i].addr, p2[i].value);
    	}
    }
    
    if(p3){
        for (i = 0; i <  p3_size_t; i++) {
    		cq_write_byte(CQ_P3, p3[i].addr, p3[i].value);
    	}
    }
    
  
    if (cq_flush_vde () < 0)
		ERROR ("cq_flush_now ()\n");
}


/* pre-play (reg LB_CONF): dto      [15: 0]
 *                         pre-fill [31:16]
 *          (reg TIMING) : H back-porch [ 7:0]
 *                         V vack-porch [13:8] */
#define PPLAY_LB_CONF_CVBS_TTL  ((0x2500 << 16) | (0x0c9a & 0xffff))
#define PPLAY_TIMING_CVBS_TTL    0x0406

void ttl_set_liveview (void)
{
    writel ((readl(0xb04000e0)&(~0x20)), 0xb04000e0);	// switch to input source
}

void ttl_set_playback (void)
{
    /* set pre-play reg */
    writel(PPLAY_LB_CONF_CVBS_TTL, PPLAY_LB_CONF_REG);
    writel(PPLAY_TIMING_CVBS_TTL, PPLAY_TIMING_REG);
}

