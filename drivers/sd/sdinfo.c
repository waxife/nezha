/**
 *  @file   sdinfo.c
 *  @brief  show sd information
 *  $Id: sdinfo.c,v 1.2 2013/12/30 07:21:51 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/24  jedy New file.
 *
 */

#include <config.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <fat32.h>
#include <io.h>
#include <sys.h>

//extern sdc_t gSDC;

#define T300_SDC_BASE_T      			0xba800000
#define SDC_BUS_WIDTH_REG				(T300_SDC_BASE_T + 0x003C)



int sd_info(sdc_t *gSDC)
{
    sdc_t *sdc = gSDC;
    int mid, appid;
    char pname[6], *p;
    int rev;
    uint32_t sernum;
    int month, year;
    int i;
    int cardbusclk;
    uint32_t *cid = sdc->card.cid;
    uint32_t *csd = sdc->card.csd;
    uint32_t *scr = sdc->card.scr;
    sd_csd_bit_t  *pcsd = (sd_csd_bit_t  *)sdc->card.csd;
    int sd = 0;
    char *ctype;

    if (!sdc->active) {
        printf("!!! No sd/mmc card actived\n");
    }

    sd = sdc->card.type;

    if (sd == SD_CARD)
        ctype = "SD";
    else if (sd == SDHC_CARD)
        ctype = "SDHC";
    else if (sd == MMC_CARD)
        ctype = "MMC";
    else
        ctype = "Unknown";

    printf("Card Type %s\n", ctype);
    printf("  CID %s = %08lx %08lx %08lx %08lx\n", 
            ((cid[0] & 1) != 1) ? "x" : " ", 
            cid[3], cid[2], cid[1], cid[0]);
    printf("  CSD %s = %08lx %08lx %08lx %08lx\n", 
            ((csd[0] & 1) != 1) ? "x" : " ", 
            csd[3], csd[2], csd[1], csd[0]);

    if (sd) {
        printf("  SCR   = %08lx %08lx\n", scr[0], scr[1]);
    }

    mid = (cid[3] >> 24) & 0xff;
    appid = (cid[3] >> 8) & 0xffff;
    
    p = (char *)(cid) + 12;
    for (i = 0; i < 5; i++, p--)
        pname[i] = *p;
    pname[i] = '\0';
    
    rev = ((cid[1] >> 16) & 0x0f) * 10 + ((cid[1] >> 20) & 0x0f);
    sernum = ((cid[1] & 0xffff) << 16) | (cid[0] >> 16);
    month = (cid[0] >> 8) & 0xf;
    year = ((cid[0] >> 12) & 0xf) + 1997;

    printf("  cid   = %s (m=%02x a=%02x) r=%02d no=%lu %d/%d\n", 
            pname, mid, appid, rev, sernum, year, month);

    if (sd) {
        p = (pcsd->CSD_STRUCTURE == 0) ? "CSD V1.0" : "CSD V2.0";
        
    } else {
        p = "";
    }
    
    if (sd) 
        printf("  sd phys = %d\n", sdc->card.sd_ver);
    

    printf("  csd   = %d %s\n", pcsd->CSD_STRUCTURE, p);
    printf("  spec  = %d\n", pcsd->MMC_SPEC_VERS);
    printf("  taac  = %d\n", sdc->card.taac);
    printf("  nsac  = %d\n", sdc->card.nsac);
    printf("  readtimeout  = %d cycle\n", sdc->card.readtimeout);
    printf("  writetimeout = %d cycle\n", sdc->card.writetimeout);
    printf("  speed        = %d\n", sdc->card.speed);

    cardbusclk = sys_apb_clk / (2 * (sdc->card.div+1));

    printf("  cardbus clk  = %d\n", cardbusclk);
    printf("  div          = %d\n", sdc->card.div);
    printf("  block size   = r (%d) w (%d)\n", sdc->card.rblksize, sdc->card.wblksize);
    printf("  misalign     = r (%d) w (%d)\n", pcsd->READ_BLK_MISALIGN, pcsd->WRITE_BLK_MISALIGN);
    printf("  partial      = r (%d) w (%d)\n", pcsd->READ_BL_PARTIAL, pcsd->WRITE_BL_PARTIAL);
    printf("  no of block  = %d\n", (int)sdc->card.nblocks);
    printf("  capacity     = %dM\n", (int)sdc->card.capacity/(1024*1024));
    printf("  bus width    = %d\n", readl(SDC_BUS_WIDTH_REG) & 0x7);

    printf("Error data = %d, rsp crc = %d rsp timeout = %d\n", sdc->data_err, 
            sdc->rsp_crc_err, sdc->rsp_timeout_err);
    printf("Access read = %d, write = %d\n", (int)sdc->read, (int)sdc->write);


    return 0;
}


int sd_1bit(sdc_t *gSDC, int arg)
{
    sdc_t *sdc = gSDC;
    
    if (arg == 0) {
        printf("%d bit mode\n", (sdc->one_bit_mode) ? 1 : 4);
        return 0;
    }

    if (arg == '1') 
        sdc->one_bit_mode = 1;
    else
        sdc->one_bit_mode = 0;

    return 0;
}
