/**
 *  @file   sd.c
 *  @brief  sd driver for tboot
 *  $Id: sdraw.c,v 1.28 2016/01/28 05:45:21 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.28 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/24  jedy New file.
 *
 */


#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <unistd.h>
#include <fs.h>
#include <dma.h>
#include <gpio.h>
#include <io.h>
#include <sys.h>
#define DBG_LEVEL   0 
#include <debug.h>
#include <cache.h>
#include <interrupt.h>
#include <mipsregs.h>
#include <fat32.h>

unsigned int sdc_debug = 0;

const unsigned long taac_timeunit_table[] =
{
	1, 10, 100, 1*K, 10*K, 100*K, 1*KK, 10*KK,
};

const unsigned long taac_timevalue_table[] =
{
	//0, 1.0, 1.2, 1.3, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 7.0, 8.0
	0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

const unsigned long trans_speed_rateuint_table[] = 
{
	100*K, 1*KK, 10*KK, 100*KK
};

#undef K
#undef KK

const unsigned long trans_speed_timevalue_table[] =
{
	//0, 1.0, 1.2, 1.3, 1.5, 2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 5.5, 6.0, 7.0, 8.0
	0, 10, 12, 13, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 70, 80
};

const unsigned long vdd_curr_min_table[] =
{
	//0.5, 1, 5, 10, 25, 35, 60, 100,
	1, 1, 5, 10, 25, 35, 60, 100,
};
const unsigned long vdd_curr_max_table[] =
{
	1, 5, 10, 25, 35, 45, 80, 200,
};


/* add for avoid dma write 0 size chip limitation */

static void inline mk_llp(int read, struct slld_t *llp, void *buf, int dsize)
{
    if (read) {
        llp->srcaddr = virt_to_phys(SDC_DATA_WINDOW_REG);
        llp->dstaddr = virt_to_phys(buf);
        llp->totsize = dsize >> 2;
        llp->control = LLPC_TCMASK | LLPC_READ | LLPC_SRC_FIX | LLPC_DST_INC | LLPC_SRC_W32 | LLPC_DST_W32;        
    } else {
        llp->srcaddr = virt_to_phys(buf);
        llp->dstaddr = virt_to_phys(SDC_DATA_WINDOW_REG);
        llp->totsize = dsize >> 2;
        llp->control = LLPC_TCMASK | LLPC_WRITE | LLPC_SRC_INC | LLPC_DST_FIX | LLPC_SRC_W32 | LLPC_DST_W32;        
    }
    //printf("data read %d src %x dst %x dsize %d\n", read, llp->srcaddr, llp->dstaddr, dsize);
}

static int _sdc_cmd(int cmd, unsigned long arg)
{
    /* write argument */
    writel(arg, SDC_ARGU_REG);

    /* send command */
    writel(cmd, SDC_CMD_REG);
    
    return 0;

}

static int _sdc_wait_cmd_done(int msec, int cmd)
{
    int wait = msec * 1000 + 1;
    int i;
    int status = 0;

    for (i = 0; i < wait; i++) {
        status = readl(SDC_STATUS_REG);
        if ((status & 0xff) != 0) 
            return status;
        usleep(1);
    }
    
    dbg(2, "!!! wait_cmd_done timeout %ccmd%d status = %08x\n", 
        (cmd & CMD_APP_CMD) ? 'a' : ' ', (cmd & CMD_CMD_MASK), status);

    return -EBUSY;

}

int sdc_sendcmd(sdc_t *sdc, int cmd, int arg, int rt, uint32_t *response)
{
    int tryi;
    int status = 0;
    int need_rsp = 1;
    int acmd = 0;
    int quite = IS_QUITE(rt);
    
    if (IS_QUITE(rt)) {
        quite = 1;
        rt = -rt;
    }

    if (rt == NR) {
        need_rsp = 0;
    } else if (rt == R2) {
        cmd = cmd | CMD_LONG_RSP | CMD_NEED_RSP;
    } else {
        cmd = cmd | CMD_NEED_RSP;
    }

    if ((cmd & CMD_APP_CMD) == CMD_APP_CMD) {
        acmd = 1;
    }

    for (tryi = 0; tryi < 3; tryi++) {
        if (acmd) { /* app command */
            /* clear command relative bits of status register */
            writel(0x7ff, SDC_CLEAR_REG);

            _sdc_cmd(CMD_NEED_RSP | CMD(55), sdc->rca);
            status = _sdc_wait_cmd_done(10, CMD(55));
            if (status < 0) 
                goto EXIT;
            
            if ((status & STATUS_RSP_CRC_OK) == 0)
                continue;

            status = readl(SDC_RESPONSE0_REG);
            if (CARD_ERR(status))
                continue;
        }


        /* clear command relative bits of status register */
        writel(0x7ff, SDC_CLEAR_REG);
       
        /* write argument */
        writel(arg, SDC_ARGU_REG);
    
        /* send command */
        writel(cmd, SDC_CMD_REG);
        status = _sdc_wait_cmd_done(100, cmd);
        if (status < 0) 
            goto EXIT;

        if (status &  STATUS_RSP_TIMEOUT) {
            /* try again */
            sdc->rsp_timeout_err++;
            continue;
        }

        if (status & STATUS_RSP_CRC_FAIL) {
            /* try again */
            sdc->rsp_crc_err++;
            continue;
        }

        if (need_rsp) {
            if (status & STATUS_RSP_CRC_OK) { /* command is done */
                if (cmd & CMD_LONG_RSP) {
                    *response = readl(SDC_RESPONSE0_REG);
                    *(response+1) = readl(SDC_RESPONSE1_REG);
                    *(response+2) = readl(SDC_RESPONSE2_REG);
                    *(response+3) = readl(SDC_RESPONSE3_REG);

                } else {
                    *response = readl(SDC_RESPONSE0_REG);
                }

                if (rt == R1 && CARD_ERR(*response) && !quite) {
                    dbg(2, "get card status error %lx rsp cmd %d\n", *response, readl(SDC_RSP_CMD_REG));
                    goto EXIT;
                }

                return 0;
            }
            
        } else {
            if (status & STATUS_CMD_SENT)  /* command is done */
                return 0;
        }
    }

EXIT:
    if (! quite) {
        dbg(2, "send %ccmd%d arg %08x status = %x\n", (acmd) ? 'a' : ' ', (cmd & CMD_CMD_MASK), arg, status);
    }

    if (response) 
        *response = status;

    return -EIO;
}



char * sdc_get_state(sdc_t *sdc)
{
    uint32_t cardstatus;
    int rc;
    int state;
    char *_sd_state[] = { "Idle", "Ready", "Ident", "Stby", "Tran", "Data", 
                      "Rcv", "Prg", "Dis" };

    rc = sdc_sendcmd(sdc, CMD(13), sdc->rca, R1, &cardstatus);
    if (rc < 0) 
        return "Unknown";

    state = (cardstatus >> 9) & 0xf;
    if (state >= 9)
        return "Reserved";
    
    return _sd_state[state];
}

#if 0
static int sdc_setblksize(sdc_t *sdc, int blksize)
{
	uint32_t cardstatus;
    int rc;

    rc = sdc_sendcmd(sdc, CMD(16), blksize, R1, &cardstatus);
    if (rc < 0) 
        return -EIO;

    return 0;
}
#endif


#if 0
static int sdc_clear_fifo(sdc_t *sdc)
{
    int i;
    int status = 0;
    for (i = 0; i < 64; i++) {
        readl(SDC_DATA_WINDOW_REG);
        status = readl(SDC_STATUS_REG);
        if (status & STATUS_FIFO_URUN) {
            printf("urun %d\n", i);
            break;
        }
    }

    writel(STATUS_FIFO_URUN, SDC_CLEAR_REG);
    return 0;
}
#endif

static int sdc_pioread(sdc_t *sdc, uint32_t *buf, int len) 
{
    int i;
    int wait = 10000;
    int fifosize;
    int wordcount;
    int status = 0;

    fifosize = (readl(SDC_FEATURE_REG) & 0xff);
printf("fifosize %d\n", fifosize);

    while(len > 0) {
        for(i = 0; i < wait; i++) {
            status = readl(SDC_STATUS_REG);
            if (status & STATUS_FIFO_ORUN)
                break;
            if (status & STATUS_DATA_TIMEOUT)   /* don't wait data timeout */
                goto EXIT;
            usleep(1);
        }

        if ((status & STATUS_FIFO_ORUN) == 0) {
            dbg(2, "wait data ready timeout %x\n", status);
            goto EXIT;
        }

        if (len > fifosize)
            wordcount = fifosize;
        else 
            wordcount = len / 4;

        for (i = 0; i < wordcount; i++, buf++)
            *buf = readl(SDC_DATA_WINDOW_REG);
        
        len -= (wordcount * 4);

        writel(STATUS_FIFO_ORUN, SDC_CLEAR_REG);
    }
    
    /* check data CRC ok */
    for(i = 0; i < wait; i++) {
        status = readl(SDC_STATUS_REG);
        if (status & STATUS_DATA_END) {
            if (status & STATUS_DATA_CRC_OK)
                return 0;
            if (status & STATUS_DATA_CRC_FAIL)
                goto EXIT;
        } else if (status & STATUS_DATA_TIMEOUT) {
            goto EXIT;
        }
    }

    dbg(2, "wait data crc ok and data end timeout %x\n", status);
    
EXIT:
    return -EIO;

}




static inline uint32_t sd_read_timeout_cycle(uint32_t bus_clk, int taac, int nsac)
{
    uint32_t ns_total, ns_percycle;

    ns_percycle = ((1000*1000*1000) + (bus_clk/2)) / (bus_clk);
    
    ns_total = (taac + (nsac * 100 * ns_percycle)) * 100;

    if (ns_total > (100*1000*1000))
        ns_total = 100*1000*1000;

    return (ns_total/ns_percycle);

}


static int sdc_csd_parse(sdc_t *sdc, uint32_t csd[], int *pdiv)
{
    sd_csd_bit_t *pcsd;
    int taac;
    int nsac;
    int div;
    int speed;
    int cardbusclk;
    int writefactor;
    unsigned long system_sd_clk, sysmgm_clk_div;

    if ((csd[0] & 1) != 1) {
        dbg(2, "Invalid CSD %08lx %08lx %08lx %08lx\n", csd[0], csd[1], csd[2], csd[3]);
        return -EIO;
    }
    sysmgm_clk_div = readl(SYSMGM_CLK_DIV);
    system_sd_clk = sys_plld_clk / (((sysmgm_clk_div>>16) & 0x0F) + 1);
    pcsd = (sd_csd_bit_t *)csd;
    taac = (taac_timevalue_table[pcsd->TAAC_TimeValue] * 
                 taac_timeunit_table[pcsd->TAAC_TimeUnit]) / 10;
    nsac = pcsd->NSAC * 100;
    speed = (trans_speed_rateuint_table[pcsd->TRAN_SPEED_RateUnit] * 
                        trans_speed_timevalue_table[pcsd->TRAN_SPEED_TimeValue]) / 10;
    sdc->card.read_bl_len = pcsd->READ_BL_LEN;
    sdc->card.rblksize = 1 << pcsd->READ_BL_LEN; 
    sdc->card.write_bl_len =  pcsd->WRITE_BL_LEN;
    sdc->card.wblksize = 1 << pcsd->WRITE_BL_LEN;

    if (sdc->card.type == SDHC_CARD && pcsd->CSD_STRUCTURE == 1) { /* SD CSD v2.0 */
        int c_size;
        c_size = (csd[1] >> 16) + (csd[2] & 0x3f);
        sdc->card.capacity = c_size * 512 * 1024;
#if 1 /* capacity has overfow problem */
        sdc->card.nblocks = c_size * 1024;
#else
        sdc->card.nblocks = sdc->card.capacity / sdc->card.rblksize;
#endif

    } else {
        int c_size;
        int mult;
        c_size = ((int)pcsd->C_SIZE_2 << 2) + (pcsd->C_SIZE_1);
        mult = 1 << ((int)pcsd->C_SIZE_MULT + 2);
        sdc->card.nblocks =  mult* (c_size + 1);
        sdc->card.capacity = sdc->card.nblocks * sdc->card.rblksize;
    }
    
    writefactor = (1 << pcsd->R2W_FACTOR);

// printf("speed = %d csd %08x %08x %08x %08x\n", speed, csd[0], csd[1], csd[2], csd[3]);
    for (div = 0x3f; div >= 0 ; div--) {
        if (speed < (system_sd_clk / (2*(div+1))))
            break;
    }
    div++;
// printf("div = %d\n", div);
    if (div > 0x3f) /* clock divid only 5 bits */
        div = 0x3f;

    *pdiv = div;
    cardbusclk = system_sd_clk / (2 * (div+1));
 
    sdc->card.readtimeout = sd_read_timeout_cycle(cardbusclk, taac, nsac);
    sdc->card.writetimeout = sdc->card.readtimeout  * writefactor;
    sdc->card.speed = speed;
    sdc->card.div = div;
    sdc->card.taac = taac;
    sdc->card.nsac = nsac;

    return 0;

}

#define SD_ID_MODE_CLOCK   (400*1000)      /* defult 400K */
#define MMC_ID_MODE_CLOCK  (300*1000)      /* defult 400K */

static int sdc_reset(sdc_t *sdc, int sd)
{
    int reset = 0;
    int i;
    int rc;
    uint32_t rsp = 0;
    uint32_t rca;
    int powerup = 0;
    int clkdiv;
    int card_type = SD_CARD;
    uint32_t hcs = 0;
    int init_clock = SD_ID_MODE_CLOCK;
    unsigned long    system_sd_clk, sysmgm_clk_div ;
    
    sysmgm_clk_div = readl(SYSMGM_CLK_DIV);
    system_sd_clk = sys_plld_clk / (((sysmgm_clk_div>>16) & 0x0F) + 1);
    writel(CMD_SDC_RST, SDC_CMD_REG);   /* reset SDC */
    sdc->rca = 0;

    /* wait reset done */
    for (i = 0; i < 10*1000; i++ ) {    /* no more than 10 ms */
        if ((readl(SDC_CMD_REG) & CMD_SDC_RST) == 0) {
            reset = 1;
            break;
        }
        usleep(1);
    }

    if (!reset) {
        dbg(2, "reset sdc timeout\n");
        return -ENODEV;
    }

   
    if (!sd)
        init_clock = MMC_ID_MODE_CLOCK;

    /* enable clock */
    clkdiv = (system_sd_clk + 2*init_clock - 1)/(2*init_clock) - 1;
    if (sd) {
    	//dbg(0, "sd: clkdiv 0x%x sdclk %ld init_clock %d \n", clkdiv, system_sd_clk, init_clock);
        writel(CLK_SD | clkdiv, SDC_CLOCK_CTRL_REG); /* SD and SD default clock enable */
        writel(BUS_WIDTH_1, SDC_BUS_WIDTH_REG);
    } else {
        writel(clkdiv, SDC_CLOCK_CTRL_REG);          /* MMC and MMC default clock enable */
        writel(BUS_WIDTH_1, SDC_BUS_WIDTH_REG);
    }

    /* step 0 reset SD Card */
    rc = sdc_sendcmd(sdc, CMD(0), 0, NR, NULL);
    if (rc < 0)
        goto EXIT;

    if (sd) {
        sdc->card.sd_ver = 10;
        /* send cmd8 to get sd phys ver */
        rc = sdc_sendcmd(sdc, CMD(8), (1 <<8) | 0x5a, QUITE(R7), &rsp);
        if (rc == 0) {
#if 0        
            assert(rsp == ((1 << 8) | 0x5a));
#else
            /*
             * some cards return unknown value instead of the asking byte, but
             * they are still 2.0 cards 
             */
            if ( rsp != ((1<<8) | 0x5a) )
                dbg(2, "SD cmd8 got rsp == 0x%08lx, not 0x%08x\n", rsp, ((1<<8)|0x5a));
#endif            
            sdc->card.sd_ver = 20;
            hcs = 1 << 30;
        }
    }


    /*
     * Card Identification Mode 
     */
    
    /* step2 ACMD41(SD) or CMD1(MMC) ask the card to send its OCR and wait power up */
    for (i = 0; i < 10000; i++) { /* no more than 1 sec */ //Kevin modify, 50000 too big to lock system.
        if (i == 2000) {                                   //Kevin modify, 6000 too big to lock system.
            printf("reset again\n");
            rc = sdc_sendcmd(sdc, CMD(0), 0, NR, NULL);
            if (rc < 0)
                goto EXIT;
        }

        if (sd) {
            rc = sdc_sendcmd(sdc, ACMD(41), (hcs | 0xff8000), QUITE(R3), &rsp);
            if (rc < 0) {
                /* card don't known ACMD41 try MMC again */
                goto EXIT;
            }
        } else {
            rc = sdc_sendcmd(sdc, CMD(1), 0xff8000, QUITE(R3), &rsp);
            if (rc < 0) 
                goto EXIT;
        }

        if (rsp & SD_OCR_POWER_UP) { /* sd or mmc card is power up */
            powerup = 1;
            break;
        }
        usleep(5);
    }

    if (! powerup) {
        dbg(2, "%s Power up not ready, OCR %08lx\n", (sd) ? "SD" : "MMC", rsp);
        goto EXIT;
    }

    if (sd && sdc->card.sd_ver >= 20) {
        if (rsp & (1 << 30)) {
            card_type = SDHC_CARD;
        }
    }



    /* step3 CMD2 to get CID */
    rc = sdc_sendcmd(sdc, CMD(2), 0, R2, sdc->card.cid);
    if (rc < 0 && sd) {
        dbg(2, "failed to get ALL CID (CMD2)\n");
        goto EXIT;
    }
    
   
    /* step4 CMD3(SD) get RCA, or CMD3(MMC) set RCA */
    if (sd) {
        rc = sdc_sendcmd(sdc, CMD(3), 0, R6, &rca);
    } else {
        rca = rand() & 0xffff0000;
        if (rca == 0) rca = 1 << 16;
        rc = sdc_sendcmd(sdc, CMD(3), rca, R6, (uint32_t *)&rsp);
    }
    if (rc < 0) {
        dbg(2, "failed to get/set RCA (CMD3)\n");
        goto EXIT;
    }
    
    sdc->card.type = (sd) ? card_type : MMC_CARD;
    sdc->rca = (rca & 0xffff0000);

    return 0;


EXIT:
    return -ENODEV;
}


int sdc_carddetected(void)
{
    return (readl(GPIO_LEVEL) &  SD_CARDDECT) ? -ENODEV : 0;
}



/* setup sd dma */
static int setup_dma(sdc_t *sdc, int read, void *data, int dsize)
{
    unsigned long cfg;
    unsigned long src, dst;
    unsigned long csr;
    int len;
    void *buf;

    assert(data != NULL );
    assert((dsize & 0x3) == 0);

    writel(1, DMA_CSR);    /* both AH0, AH1 are little endian, enable dma */
    writel(1 << DMA_SD, DMA_INT_TC_CLR);  /* clear TC and INT */
    writel(ERRABT << DMA_SD, DMA_ERRABT_CLR); /* clear err abort INT */
    //writel(1 << DMA_SD, DMA_SYNC);


    buf = data;
    len = dsize / 4;

    if (read) {
        cfg = SRC_HE | SRC_CH(DMA_SD) | 0x7;
        src = virt_to_phys(SDC_DATA_WINDOW_REG);
        dst = virt_to_phys(buf);
        csr = CSR_BURST_4 | CSR_READ | CSR_SRC_FIX | CSR_DST_INC | CSR_SRC_W32 | CSR_DST_W32 | CSR_HWH;
    } else {
        cfg = DST_HE | DST_CH(DMA_SD) | 0x7;
        src = virt_to_phys(buf);
        dst = virt_to_phys(SDC_DATA_WINDOW_REG);
        csr = CSR_BURST_4 | CSR_WRITE | CSR_SRC_INC | CSR_DST_FIX | CSR_SRC_W32 | CSR_DST_W32 | CSR_HWH;
    }


    writel(csr, DMA_CH_CSR(DMA_SD));
    writel(cfg, DMA_CH_CFG(DMA_SD));
    writel(src, DMA_CH_SrcAddr(DMA_SD));
    writel(dst, DMA_CH_DstAddr(DMA_SD));
    writel(len, DMA_CH_SIZE(DMA_SD));
    writel(0, DMA_CH_LLP(DMA_SD));

#if 0
    dma_cache_wback_inv(data, dsize);
#else
    flush_dcache_all();
#endif
    do {} while(0);

    writel(csr | CSR_CH_EN, DMA_CH_CSR(DMA_SD));

#if 0
printf("%08x=%08x %08lx\n", DMA_CH_CFG(DMA_SD), readl(DMA_CH_CFG(DMA_SD)), cfg);
printf("%08x=%08x\n", DMA_CH_SrcAddr(DMA_SD), readl(DMA_CH_SrcAddr(DMA_SD)));
printf("%08x=%08x %08lx\n", DMA_CH_DstAddr(DMA_SD), readl(DMA_CH_DstAddr(DMA_SD)), dst);
printf("%08x=%08x %08x\n", DMA_CH_SIZE(DMA_SD), readl(DMA_CH_SIZE(DMA_SD)), len);
printf("%08x=%08x\n", DMA_CH_LLP(DMA_SD), readl(DMA_CH_LLP(DMA_SD)));
printf("%08x=%08x %08lx\n", DMA_CH_CSR(DMA_SD), readl(DMA_CH_CSR(DMA_SD)), csr | CSR_CH_EN );
#endif

    return 0;

}

static int sdc_stop_transmission(sdc_t *sdc)
{
    int rc;
    uint32_t cardstatus;

    rc = sdc_sendcmd(sdc, CMD(12), 0, R1, &cardstatus);
    if (rc < 0) 
        return -EIO;
    
    return 0;
}

typedef struct sd_scr_bit {
    unsigned long rsv_manufacture;  /* [31:0] */
    unsigned long cmd_support:4;    /* [35:32] */
    unsigned long rsv:6;            /* [41:36] */
    unsigned long sd_spec4:1;       /* [42] */
    unsigned long ex_security:4;    /* [46:43] */
    unsigned long sd_spec3:1;       /* [47] */
    unsigned long sd_bus_widths:4;  /* [51:48] */
    unsigned long sd_security:3;    /* [54:52] */
    unsigned long data_state_after_erase:1; /* [55] */
    unsigned long sd_spec:4;        /* [59:56] */
    unsigned long scr_structure:4;  /* [63:60] */
} sd_scr_bit_t;


static void __attribute__((unused))
sdc_scr_info(sdc_t *sdc, uint32_t *scr)
{
    uint32_t sbb[2];
    sd_scr_bit_t *sb = (sd_scr_bit_t *)sbb;

    sbb[1] = ((scr[0]>>24)&0xff) | // move byte 3 to byte 0
              ((scr[0]<<8)&0xff0000) | // move byte 1 to byte 2
              ((scr[0]>>8)&0xff00) | // move byte 2 to byte 1
              ((scr[0]<<24)&0xff000000); // byte 0 to byte 3
    sbb[0] = ((scr[1]>>24)&0xff) | // move byte 3 to byte 0
              ((scr[1]<<8)&0xff0000) | // move byte 1 to byte 2
              ((scr[1]>>8)&0xff00) | // move byte 2 to byte 1
              ((scr[1]<<24)&0xff000000); // byte 0 to byte 3
#if 0
    printf("scr : %08lx - %08lx\n",sbb[0], sbb[1]); 
    printf(" rsv_manufacture:   %lx\n", sb->rsv_manufacture);
    printf(" cmd_support:       %x\n", sb->cmd_support);
    printf(" rsv:               %x\n", sb->rsv);
    printf(" sd_spec4:          %x\n", sb->sd_spec4);
    printf(" ex_security:       %x\n", sb->ex_security);
    printf(" sd_spec3:          %x\n", sb->sd_spec3);
    printf(" sd_bus_width:      %x\n", sb->sd_bus_widths);
    printf(" sd_security:       %x\n", sb->sd_security);
    printf(" data_state_after_erase: %x\n", sb->data_state_after_erase);
    printf(" sd_spec:           %x\n", sb->sd_spec);
    printf(" scr_structure:     %x\n", sb->scr_structure);
#endif
    sdc->cmd20 = (sb->cmd_support & 1) ? 1 : 0;
    sdc->cmd23 = (sb->cmd_support & 2) ? 1 : 0;

    printf("---- SD SCR Info -----------\n");
    if (sb->sd_spec == 0)
        printf(" SD Spec v1.0/v1.01\n");
    else if (sb->sd_spec == 1)
        printf(" SD Spec v1.10\n");
    else if (sb->sd_spec == 2) {
        if (sb->sd_spec4)
            printf(" SD Spec v4.0\n");
        else if (sb->sd_spec3)
            printf(" SD Spec v3.0\n");
        else
            printf(" SD Spce v2.0\n");
    }
    if (sb->sd_security == 2)
        printf(" SDSC Card\n");
    else if (sb->sd_security == 3)
        printf(" SDHC Card\n");
    else if (sb->sd_security == 4)
        printf(" SDXC Card\n");

    if (sb->sd_bus_widths == 1)
        printf(" SD Bus Width 1\n");
    else if (sb->sd_bus_widths == 5)
        printf(" SD Bus Width 1 & 4\n");

    if ((sb->cmd_support & 1)) 
        printf(" CMD20 supported\n");
    if ((sb->cmd_support & 2))
        printf(" CMD23 supported\n");
    if ((sb->cmd_support & 4))
        printf(" CMD48/49 supported\n");
    if ((sb->cmd_support & 8))
        printf(" CMD58/59 supported\n");    
    printf("--------------------------\n");
}

static int sd_probe(void *hcard)
{
    int rc;
    int lrc = -1;
    sdc_t *sdc = (sdc_t *)hcard;
    uint32_t    cardstatus;
    int         bus_width;
    int         div = 0;
    int         clk;
    uint32_t    *scr;

    assert(sdc->magic == MAGIC_SDC);    
    if (sdc->active)
        return 0;
   
    /* check card inserted */
    rc = sdc_carddetected();
    if (rc < 0) {
	dbg(2, "caredtected failed rc = %d\n", rc);
        lrc = -ENODEV;
        goto EXIT;
    }

    sdc->data_err = sdc->rsp_crc_err = sdc->rsp_timeout_err = 0;   /* reset statistic */
    sdc->read = sdc->write = 0;
/*T582 not use GPIO to control SD power now. (Kevin 2014/01/28)*/
//#ifdef CONFIG_ALWAYS_SD_POWER_ON
//    writel(SD_POWER, GPIO_CLEAR);     /* power on SD */
//#else
//    writel(SD_POWER, GPIO_SET);     /* power on SD */
//    usleep(10);
//    writel(SD_POWER, GPIO_CLEAR);     /* power on SD */
//#endif 


    /* select sd card */
    card_select(CARD_SEL_SD);
    
    rc = sdc_reset(sdc, 1);     /* try SD card first */
    if (rc < 0) {
        rc = sdc_reset(sdc, 0); /* try MMC card again */
    }

    if (rc < 0) {
        dbg(2, "can't identified both SD and MMC\n");
        lrc = -ENODEV;
        goto EXIT;
    }

    /* 
     * Enter Data Transfer Mode - Standby State
     */

    /* step5 CMD9 get CSD */
    rc = sdc_sendcmd(sdc, CMD(9), sdc->rca, R2, sdc->card.csd);
    if (rc < 0) {
        dbg(2, "failed to get CSD\n");
        lrc = rc;
        goto EXIT;
    }


    /* step6 CMD10 to get rca's CID */
    rc = sdc_sendcmd(sdc, CMD(10), sdc->rca, R2, sdc->card.cid);
    if (rc < 0) {
        dbg(2, "failed to get CID\n");
        lrc = rc;
        goto EXIT;
    }

    // sdc_cid_parse(sdc, cid);
    sdc_csd_parse(sdc, sdc->card.csd, &div);
    clk = readl(SDC_CLOCK_CTRL_REG);
    clk = (clk & ~CLK_DIV_MASK) | div;
    dbg(4, "sd: div 0x%x clk 0x%x \n", div, clk);
    writel(clk, SDC_CLOCK_CTRL_REG);

    /* 
     * Enter Data Transfer Mode - Transfer State
     */

    /* read SCR from card */
    /* step 7 CMD7 to select card */
    rc = sdc_sendcmd(sdc, CMD(7), sdc->rca, R1, &cardstatus);
    if (rc < 0) {
        dbg(2, "CMD(7) rc = %d\n", rc);
        lrc = -EIO;
        goto EXIT;
    }

    /* clear card detect pull high */
    if (sdc->card.type == SD_CARD) {
        rc= sdc_sendcmd(sdc, ACMD(42), 0, R1, &cardstatus);
        if (rc < 0) {
	    dbg(2, "ACMD(42) rc = %d\n",rc);
            lrc = -EIO;
            goto EXIT;
        }
    }

    /* program bus width */

    if (readl(SDC_BUS_WIDTH_REG) & WIDE_BUS_SUPPORT && !sdc->one_bit_mode) {
        if (sdc->card.type != MMC_CARD) {
#if 0
            rc = sdc_setblksize(sdc, 8);
            if (rc < 0) {
                dbg(2, "failed to set transfer size %d\n", rc);
                lrc = rc;
                goto EXIT;
            }
#endif
        
            writel(MAXTIMEOUT*2, SDC_DATA_TIMER_REG);
            writel(8, SDC_DATA_LEN_REG);
            writel(BLK_SIZE_8 | DATA_READ | DATA_EN, SDC_DATA_CTRL_REG);
            
            /* ACMD51 SCR, read the SD Configuration Register */
            rc = sdc_sendcmd(sdc, ACMD(51), 0, R1, &cardstatus);
            if (rc < 0) {
		dbg(2, "ACMD(51) rc=%d\n", rc);
                lrc = -EIO;
                goto EXIT;
            }
        
            scr = sdc->card.scr;
            rc = sdc_pioread(sdc, scr, 8);
            if (rc < 0) {   
                dbg(2, "failed to read scr data %d\n", rc);
            } else {
                bus_width = (scr[0] >> 8) & 0x0f;
                if (bus_width & 0x04) { /* support bus_width 4 */
                    rc = sdc_sendcmd(sdc, ACMD(6), 2, R1, &cardstatus);
                    if (rc < 0) {
			dbg(2, "ACMD(6) rc = %d\n", rc);
                        lrc = -EIO;
                        goto EXIT;
                    }
                    writel(BUS_WIDTH_4, SDC_BUS_WIDTH_REG);
                }
            }

            sdc_scr_info(sdc,scr);
        } else { /* mmc use cmd6 to set bus width */

            /* Because MMC spec 4.0 use DAT0 to singal busy state when R1b responsed
             * The faraday doesn't check DAT0 to wait R1b response. The CMD6 may not
             * support by faraday SD controller (Jedy 2007/04/13)
             */
            uint32_t arg;
            arg = (3 << 24) | (183 << 16) | (1 << 8);
            rc = sdc_sendcmd(sdc, CMD(6), arg, R1, &cardstatus);
            if (rc == 0) {
		dbg(2, "CMD(6) rc %d\n", rc);
                writel(BUS_WIDTH_4, SDC_BUS_WIDTH_REG);
            }
        }
    }

    /* record write_protect bit */
    sdc->wp = 0;
    sdc->active = 1;

    return 0;
EXIT:
    sdc->active = 0;
    writel(0x1ff, SDC_CLOCK_CTRL_REG);   /* disable SD clock */
    writel(BUS_WIDTH_1, SDC_BUS_WIDTH_REG);
    writel(CMD_SDC_RST, SDC_CMD_REG);   /* reset SDC */
/*T582 not use GPIO to control SD power now. (Kevin 2014/01/28)*/
//#ifndef CONFIG_ALWAYS_SD_POWER_ON
//    writel(SD_POWER, GPIO_SET);         /* power off SD */
//#endif

    return lrc;

}

static int sd_remove(void *hcard)
{
    sdc_t *sdc = (sdc_t *)hcard;

    assert(sdc->magic == MAGIC_SDC);


    sdc->active = 0;

    writel(0x1ff, SDC_CLOCK_CTRL_REG);  /* disable SD clock */
    writel(BUS_WIDTH_1, SDC_BUS_WIDTH_REG);
    writel(CMD_SDC_RST, SDC_CMD_REG);   /* reset SDC */
/*T582 not use GPIO to control SD power now. (Kevin 2014/01/28)*/
//#ifndef CONFIG_ALWAYS_SD_POWER_ON
//    writel(SD_POWER, GPIO_SET);     /* power off SD */
//#endif
    dbg(0,"Crad Remove.\n");
    return 0;
}

static int
sd_size (void *hcard)
{
    struct sdc *sdc = (struct sdc *)hcard;
    struct sd_card *card = &sdc->card;

    return card->nblocks * (card->rblksize / 512);
}


int  sdc_until_transfer_state(sdc_t *sdc, int *pstate)
{
    uint32_t cardstatus;
    int rc, lrc = -1;
    int state;

    for (;;) {
        rc = sdc_sendcmd(sdc, CMD(13), sdc->rca, R1, &cardstatus);
        if (rc < 0) {
            lrc = -EIO;
            goto EXIT;
        }

        state = (cardstatus >> 9) & 0x0f;
        if (pstate) *pstate = state;
        
        if (state == SD_ST_TRAN)
            return 0;

        if (state == SD_ST_IDLE || state == SD_ST_READY || state == SD_ST_IDENT ||
            state == SD_ST_STBY || state == SD_ST_DIS) {
            printf("until transfer state but state = %d\n", state);
            return 1;
        }

        usleep(1);
    }
    
EXIT:
    return lrc;    

}





/*
 * SD Card Read/Write/Erase Function
 */
static int sdc_read_blocks(sdc_t *sdc, uint32_t start, int bsize, int count, char *buf) 
{
    int timeout = sdc->card.readtimeout;
    int dsize = bsize * count;
    int rc;
    int lrc = -1;
    int status = 0;
    uint32_t cardstatus;
    uint32_t err, tc;
    int i;
    // int bl = sdc->card.read_bl_len;
    int need_stop = 0;
    int done = 0;
    int tryi;

    if ( timeout == 0 ) /* for one SZ sd card bug with taac and nsac equal to 0 */
        timeout = MAXTIMEOUT;

    sdc_until_transfer_state(sdc, NULL);

    for (tryi = 0; tryi < 3; tryi++) {
        /* stop dma transfer */
        writel(CSR_ABORT, DMA_CH_CSR(DMA_SD));

        rc = setup_dma(sdc, 1, buf, dsize);
        if (rc < 0) {
            dbg(2, "failed to setup dmp %d\n", rc);
            lrc = rc;
            goto EXIT;
        }

        writel(timeout*2, SDC_DATA_TIMER_REG);
        writel(dsize, SDC_DATA_LEN_REG);
        // writel(bl | DATA_READ | DATA_DMA | DATA_EN, SDC_DATA_CTRL_REG);
        writel(BLK_SIZE_512 | DATA_READ | DATA_DMA | DATA_EN, SDC_DATA_CTRL_REG);

#ifdef MMC_BUG    
        if (count >= 1) { /* JEDY  beware of 2GB count may be >= 2 */
#else
        if (count > 1) { /* JEDY  beware of 2GB count may be >= 2 */
#endif
            rc = sdc_sendcmd(sdc, CMD(18), start, R1, &cardstatus);
            if (rc < 0) {
                lrc = -EIO;
                goto EXIT;
            }
            need_stop = 1;
        } else {
            rc = sdc_sendcmd(sdc, CMD(17), start, R1, &cardstatus);
            if (rc < 0) {
                lrc = -EIO;
                goto EXIT;
            }
        }

        /* wait until data transfer end */        
        for (i = 0; i < 100000*count; i++) { /* wait no more 100 ms */
            status = readl(SDC_STATUS_REG);
            if (status & (STATUS_DATA_TIMEOUT | STATUS_DATA_END | STATUS_DATA_CRC_FAIL))
                break;
            usleep(1);
        }

        if ((sdc_debug & SDC_DEBUG_READ) && i > 10000) {
            printf ("  offset=%10lu, wait transfer %5dus\n", start, i);
        }
        if (status & STATUS_DATA_TIMEOUT) {
            dbg(2, "read data timeout, status 0x%x\n", status);
            lrc = -EIO;
            goto EXIT;
        } else if (status & STATUS_DATA_CRC_FAIL) {
            dbg(2, "read data: DATA_CRC_FAIL, status 0x%x\n", status);
            sdc->data_err++;
            if (need_stop)
                sdc_stop_transmission(sdc); 
            continue;
        } else if ((status & (STATUS_DATA_CRC_OK | STATUS_DATA_END)) == (STATUS_DATA_CRC_OK | STATUS_DATA_END)) {
            done = 1;
            break; /* should be ok */
        }

        dbg(2, "try again:: read block status = %x\n", status);

        /* otherwise try again */
    }

    if (!done) {
        dbg(2, "sdc_read_blocks:%d-%d too may tries status = %x (state=%s)\n", (int)start, (int)count, (int)status, sdc_get_state(sdc));
        lrc = -EIO;
        goto EXIT;
    }

    /* wait until dma done */
    for (i = 0; i < (10*1000); i++) {  /* no more 10 ms */
        tc = readl(DMA_TC);
        if (tc & (1 << DMA_SD)) 
            break;

        err = readl(DMA_ERRABT);
        if (err & (ERRABT << DMA_SD)) {
            dbg(2, "DMA error or abort when read sd block %lx\n", err);
            lrc = -EIO;
            goto EXIT;
        }
        
        usleep(1);
    }

    writel(1 << DMA_SD, DMA_INT_TC_CLR);  /* clear TC and INT */
    writel(ERRABT << DMA_SD, DMA_ERRABT_CLR); /* clear err abort INT */

    if (need_stop) {
        rc = sdc_stop_transmission(sdc);
        if (rc < 0) 
            dbg(2, "Failed to stop transmission %d\n", rc);
    }        

    sdc_until_transfer_state(sdc, NULL);
    
    sdc->read += count;


    return 0;

EXIT:
    if (need_stop)
        sdc_stop_transmission(sdc);

    return lrc;
}

static int sd_read_sector(void *hcard, int startsector, 
        int sectorcount, int sectorsize, char *buf)
{
    sdc_t *sdc = (sdc_t *)hcard;
    int rc;
    int lrc = -1;
    int blocksize;
    int blockcount;
    int blocks_per_sector;
    int nblock;
    int startblock;
    uint32_t start;

    ASSERT_UNDER_STACK();
    assert(sdc->magic == MAGIC_SDC);

    if (!sdc->active) {
        lrc = -ENODEV;    
        goto EXIT;
    }

    if (sdc_carddetected() < 0) {
        lrc = -ENODEV;
        goto EXIT;
    }

    /* select sd card */
    card_select(CARD_SEL_SD);

    blocksize = 512; // sdc->card.rblksize;
    if (blocksize > sectorsize) {
        dbg(2, "invalid sector size %d (block %d)\n", sectorsize, blocksize);
        goto EXIT;
    }

    blocks_per_sector = sectorsize / blocksize;
    blockcount = sectorcount * blocks_per_sector;
    startblock = startsector * blocks_per_sector;
    if (sdc->card.type == SDHC_CARD) {
        start = startblock;
    } else {
        start = startblock * blocksize;
    }
    
    while(blockcount > 0) {
        nblock = (blockcount > MAX_MULTIBLOCK_NUM) ? MAX_MULTIBLOCK_NUM : blockcount;
        rc = sdc_read_blocks(sdc, start, blocksize, nblock, buf);
        if (rc < 0) {
            lrc = rc;
            ERROR ("sdc_read_blocks(%lu,%d), rc=%d\n",  start, nblock, rc);

            /* reset SD */
            sd_remove (sdc);
            sd_probe (sdc);
            goto EXIT;
        }

        blockcount -= nblock;
        if (sdc->card.type == SDHC_CARD) {
            start += nblock;
        } else {
            start += (nblock * blocksize);
        }
        buf += (nblock * blocksize);
    }
    
    return 0;

EXIT:
    if (sdc_carddetected() < 0) 
        sd_remove(sdc);
    

    return lrc;

}

int sdc_change_mode(void *handle, int mode)
{
    int rc, lrc = 0;
    uint32_t cardstatus;
    sdc_t *sdc = (sdc_t *)handle;
    if (! sdc->cmd20) /* does't support speed class */
        return 0;

    rc = sdc_sendcmd(sdc, CMD(20), mode, R1, &cardstatus);
    if (rc < 0) {
        lrc = -EIO;
        goto EXIT;
    }

    sdc_until_transfer_state(sdc, NULL);
EXIT:
    return lrc;    
}


static int sdc_write_blocks(sdc_t *sdc, uint32_t start, int bsize, int count, char *buf) 
{
    int timeout = sdc->card.writetimeout;
    int dsize = bsize * count;
    int rc;
    int lrc = -1;
    int status;
    uint32_t cardstatus;
    uint32_t err, tc;
    int i;
    // int bl = sdc->card.write_bl_len;
    int need_stop = 0;

    sdc_until_transfer_state(sdc, NULL);


    /* stop dma transfer */
    writel(CSR_ABORT, DMA_CH_CSR(DMA_SD));

    rc = setup_dma(sdc, 0, buf, dsize);
    if (rc < 0) {
        dbg(2, "failed to setup dmp %d\n", rc);
        lrc = rc;
        goto EXIT;
    }

    writel(timeout*2, SDC_DATA_TIMER_REG);
    writel(dsize, SDC_DATA_LEN_REG);
    writel(BLK_SIZE_512 | DATA_WRITE | DATA_DMA | DATA_EN, SDC_DATA_CTRL_REG);

    if (count > 1) {
        rc = sdc_sendcmd(sdc, ACMD(23), dsize, R1, &cardstatus);
        if (rc < 0) {
            lrc = -EIO;
            goto EXIT;
        }
        rc = sdc_sendcmd(sdc, CMD(25), start, R1, &cardstatus);
        if (rc < 0) {
            lrc = -EIO;
            goto EXIT;
        }
        need_stop = 1;
    } else {
        rc = sdc_sendcmd(sdc, CMD(24), start, R1, &cardstatus);
        if (rc < 0) {
            lrc = -EIO;
            goto EXIT;
        }
    }




    /* wait until data transfer end */
    for (;;) {
        status = readl(SDC_STATUS_REG);
        if (status & (STATUS_DATA_TIMEOUT | STATUS_DATA_CRC_FAIL)) {
            dbg(2, "write data transfer timeout status = %x\n", status);
            lrc = -EIO;
            goto EXIT;
        } else if ((status & (STATUS_DATA_CRC_OK | STATUS_DATA_END)) == (STATUS_DATA_CRC_OK | STATUS_DATA_END)) {
            break;
        }
        usleep(1);
    }


    if (need_stop) {
        rc = sdc_stop_transmission(sdc);
        if (rc < 0) 
            dbg(2, "Failed to stop transmission %d\n", rc);
    }

    /* wait until dma done */
    for (i = 0; i < (10*1000); i++) {  /* no more 10 ms */
        tc = readl(DMA_TC);
        if (tc & (1 << DMA_SD)) 
            break;

        err = readl(DMA_ERRABT);
        if (err & (ERRABT << DMA_SD)) {
            dbg(2, "DMA error or abort when read sd block %lx\n", err);
            lrc = -EIO;
            goto EXIT;
        }
    
        usleep(1);
    }

    writel(1 << DMA_SD, DMA_INT_TC_CLR);  /* clear TC and INT */
    writel(ERRABT << DMA_SD, DMA_ERRABT_CLR); /* clear err abort INT */

//    sdc_until_transfer_state(sdc, NULL);

    sdc->write += count;

    return 0;

EXIT:
    if (need_stop) 
        sdc_stop_transmission(sdc);

    return lrc;
}

static int sd_write_sector(void *hcard, int startsector, int sectorcount, 
    int sectorsize, char *buf)
{
    sdc_t *sdc = (sdc_t *)hcard;
    int rc = 0;
    int lrc = -1;
    int blocksize;
    int blockcount;
    int blocks_per_sector;
    int nblock;
    int startblock;
    int retry_count=0;
    uint32_t start;

    assert(sdc->magic == MAGIC_SDC);

    if (!sdc->active) {
        lrc = -ENODEV;    
        goto EXIT;
    }

    if (sdc_carddetected() < 0) {
        lrc = -ENODEV;
        goto EXIT;
    }

    /* select sd card */
    card_select(CARD_SEL_SD);

    blocksize = 512; // sdc->card.wblksize;
    if (blocksize > sectorsize) {
        dbg(2, "invalid sector size %d (block %d)\n", sectorsize, blocksize);
        goto EXIT;
    }

    blocks_per_sector = sectorsize / blocksize;
    blockcount = sectorcount * blocks_per_sector;
    startblock = startsector * blocks_per_sector;
    if (sdc->card.type == SDHC_CARD) {
        start = startblock;
    } else {
        start = startblock * blocksize;
    }

    while(blockcount > 0) {
        nblock = (blockcount > MAX_MULTIBLOCK_NUM) ? MAX_MULTIBLOCK_NUM : blockcount;
        retry_count=0;
RETRYW:
        if(retry_count<5)        
            rc = sdc_write_blocks(sdc, start, blocksize, nblock, buf);    
            
        if (rc < 0) {
            //dbg(2, "failed to sdc_write_blocks (%d-%d) %d Retry\n", startsector, sectorcount, rc);
            lrc = rc;
            if(retry_count++ < 5){
                goto RETRYW;
            }
            else{
                dbg(2, "failed to sdc_write_blocks (%d-%d) %d Retry 5 times.\n", startsector, sectorcount, rc);
                goto EXIT;
            }
        }

        blockcount -= nblock;
        if (sdc->card.type == SDHC_CARD) {
            start += nblock;
        } else {
            start += (nblock * blocksize);
        }
        buf += (nblock * blocksize);
    }

    return 0;

EXIT:
    if (sdc_carddetected() < 0) 
        sd_remove(sdc);

    return lrc;
}

static int sd_erase_sector(void *hcard, int startsector, 
    int sectorcount, int sectorsize)
{
    
    return 0;
}

int sd_disk_insert(struct fat_t *fat)
{	
    sdc_t *gSDC = (sdc_t *)fat->hcard;

    
    printf("\nSD/MMC Card Inserted\n");
/*T582 not use GPIO to control SD power now. (Kevin 2014/01/28)*/    
//    writel(SD_POWER, GPIO_CLEAR);     /* power on SD */
    //rc = drive_insert(SD_DISK);
    if ( gSDC->one_bit_mode != 2) {
        printf("Try SD 1 bit mode\n");
        gSDC->one_bit_mode = 2;
    }

    return 0;
}

int sd_disk_remove(struct fat_t *fat)
{
    sdc_t *gSDC = (sdc_t *)fat->hcard;
    printf("\nSD/MMC Card Removed\n");
    ///drive_remove(SD_DISK);

    if (gSDC->one_bit_mode == 2)
        gSDC->one_bit_mode = 0;

    return 0;
}


const drive_op_t sd_op = {
    init_card:      sd_probe,
    release_card:   sd_remove,
    device_size:    sd_size,
    read_sector:    sd_read_sector,
    write_sector:   sd_write_sector,
    erase_sector:   sd_erase_sector, 
};

void sd_donothing(void) {}

void sd_drv_init(void *argu)
{
    //uint32_t oe;
    struct sysc_t *sys = (struct sysc_t *)argu;
    struct fat_t *fat = sys->pfat;
    sdc_t *gSDC = fat->hcard;

    memset(gSDC, 0, sizeof(sdc_t));
    gSDC->magic = MAGIC_SDC;
	fat->op = (drive_op_t *)&sd_op;

	gSDC->dma = DMA_SD;	
/*T582 not use GPIO to control SD power now. (Kevin 2014/01/28)*/
//    oe = SD_POWER | readl(GPIO_OE);
//    writel(oe, GPIO_OE);
//#ifdef CONFIG_ALWAYS_SD_POWER_ON
//    writel(SD_POWER, GPIO_CLEAR);       /* power on SD */
//#else
//    writel(SD_POWER, GPIO_SET);         /* power off SD */
//#endif

    /* do card detection by UI later */

#if 1
	//gSDC->multi_dma = 1;
	gSDC->rdy = 1;    
	gSDC->stop = 0;	
#endif 
}

static int check_sdc_ready(void *handle, int need_stop, int nblock)
{
    sdc_t *sdc = (sdc_t *)handle;
    int status;
    int lrc;
    int rc;
    int i;
    uint32_t err, tc;

    status = readl(SDC_STATUS_REG);
    if (status & (STATUS_DATA_TIMEOUT | STATUS_DATA_CRC_FAIL)) {
        dbg(2, "write data transfer timeout status = %x\n", status);
        lrc = -EIO;
        goto EXIT;
	} else if ((status & (STATUS_DATA_CRC_OK | STATUS_DATA_END)) == (STATUS_DATA_CRC_OK | STATUS_DATA_END)) {
    //} else if ((status & (STATUS_DATA_CRC_OK | STATUS_FIFO_URUN)) == (STATUS_DATA_CRC_OK | STATUS_FIFO_URUN)) {
    	
   	    if (need_stop) {
	        rc = sdc_stop_transmission(sdc);
	        if (rc < 0) 
	            dbg(2, "Failed to stop transmission %d\n", rc);
	    }

	    /* wait until dma done */
	    for (i = 0; i < (10*1000); i++) {  /* no more 10 ms */
	        tc = readl(DMA_TC);
	        if (tc & (1 << DMA_SD)) 
	            break;
	
	        err = readl(DMA_ERRABT);
	        if (err & (ERRABT << DMA_SD)) {
	            dbg(2, "DMA error or abort when read sd block %lx\n", err);
	            lrc = -EIO;
	            goto EXIT;
	        }
	    
	        usleep(1);
	    }

	    writel(1 << DMA_SD, DMA_INT_TC_CLR);  /* clear TC and INT */
	    writel(ERRABT << DMA_SD, DMA_ERRABT_CLR); /* clear err abort INT */
	
	    sdc->write += nblock;
    	
        return 0;
    }
    else{ /* sdc not ready */
//printf("check_sdc_ready %08x\n", status);
		return -1;
	}
	
EXIT:
	return lrc;	
	
}

int sdr_write(void *handle, int startsector, int sectorcount, char *buf)
{
    sdc_t *sdc = (sdc_t *)handle;
    int rc;
    int lrc = -1;
    int sectorsize = 512;
    int blocksize = 512;
    int blockcount;
    int blocks_per_sector;
    int nblock;
    int startblock;
    uint32_t start;
    int timeout = sdc->card.writetimeout;
    int dsize;
    uint32_t cardstatus;
    int need_stop = 0;
    	
    assert(sdc->magic == MAGIC_SDC);
	
	dsize = blocksize *sectorcount;
    if (!sdc->active) {
        lrc = -ENODEV;    
        goto EXIT;
    }

    if (sdc_carddetected() < 0) {
        lrc = -ENODEV;
        goto EXIT;
    }

    /* select sd card */
    card_select(CARD_SEL_SD);

    blocks_per_sector = sectorsize / blocksize;
    blockcount = sectorcount * blocks_per_sector;
    startblock = startsector * blocks_per_sector;
    if (sdc->card.type == SDHC_CARD) {
        start = startblock;
    } else {
        start = startblock * blocksize;
    }

    nblock = (blockcount > MAX_MULTIBLOCK_NUM) ? MAX_MULTIBLOCK_NUM : blockcount;
//printf("bc %d, nblock %d\n", blockcount, nblock);

   	if ( !sdc->rdy ){
//printf("^^^ %d\n", sdc->stop);   		
   		rc = check_sdc_ready(sdc, sdc->stop, nblock);
   		if ( rc < 0 ){
//printf(">>> sdc not ready ...\n");			
   			return SD_CTRL_NOT_READY;
   		}
   		else{
//printf(">>> sdc ready ...\n");		
			sdc->rdy = 1;
			sdc->stop = 0;				
   			return 0;
   		}
   	}

	/* check sd card ready or not */
    int state;
    rc = sdc_sendcmd(sdc, CMD(13), sdc->rca, R1, &cardstatus);
    if (rc < 0) {
        lrc = -EIO;
        goto EXIT;
    }

    state = (cardstatus >> 8) & 0x01;
	if ( !state ){
//printf(">>> card not ready ...\n");			
		return SD_CARD_NOT_READY;
	} 
	/*^^^check sd card ready or not */
    
    
    /* stop dma transfer */
    writel(CSR_ABORT, DMA_CH_CSR(DMA_SD));
    
    rc = setup_dma(sdc, 0, buf, dsize);
    if (rc < 0) {
        dbg(2, "failed to setup dmp %d\n", rc);
        lrc = rc;
        goto EXIT;
    }

    writel(timeout*2, SDC_DATA_TIMER_REG);
    writel(dsize, SDC_DATA_LEN_REG);
    writel(BLK_SIZE_512 | DATA_WRITE | DATA_DMA | DATA_EN, SDC_DATA_CTRL_REG);

    if (nblock > 1) {
        rc = sdc_sendcmd(sdc, CMD(25), start, R1, &cardstatus);
        if (rc < 0) {
            lrc = -EIO;
            goto EXIT;
        }
        need_stop = 1;
		sdc->stop = 1;	

    } else {
        rc = sdc_sendcmd(sdc, CMD(24), start, R1, &cardstatus);
        if (rc < 0) {
            lrc = -EIO;
            goto EXIT;
        }
    }

	/* check SDC ready or not */
	rc = check_sdc_ready(sdc, need_stop, nblock);
	if ( rc < 0 ){
		sdc->rdy = 0;		
		return SD_CTRL_NOT_READY;    			
	}

	return 0;	

EXIT:
    if (sdc->stop) 
        sdc_stop_transmission(sdc);
	
    if (sdc_carddetected() < 0) 
        sd_remove(sdc);

    return lrc;
	
}

void sdc_detect(void)
{
    struct fat_t *pfat = gSYSC.pfat;
    struct sdc *psdc;
    int rc = 0;


    /* check card inserted */
    psdc = pfat->hcard;
    psdc->state = SDC_NONE;
    rc = sdc_carddetected();
    if (rc == 0) {
        rc = _ismounted();
        if (rc == 0) {
            psdc->state = SDC_INSERT;
            rc = _mount(pfat);
            if (rc == 0) {
                psdc->state = SDC_MOUNTED;
            } else if (rc == 1) {
                psdc->state = SDC_MOUNTED_RDONLY;
            }
        } else if (rc == 1) {
            psdc->state = SDC_MOUNTED;
        } else if (rc == 2) {
            psdc->state = SDC_MOUNTED_RDONLY;
        }
    }
}

int sdc_status(void)
{
    struct sdc *psdc = gSYSC.pfat->hcard;

    psdc->state = SDC_NONE;
    sdc_detect();

    return psdc->state;
}

int sdc_sd_speed_verify(void){

    struct sdc *sd = gSYSC.pfat->hcard;
    char buf[1024];
    unsigned int ticks;
    unsigned int total_time;
    unsigned int wticks=0;
    int i, j, start;
    int writ_size = 0;
    int rc;
    
    rc = sd_probe(sd);
    if (rc < 0) {
        printf("Failed to probe sd card rc =%d\n", rc);
    }
    
        writ_size = 2*512;
        
        printf("-----> Write 32MB and %d byte at a time.\n", writ_size);
      

        for (i = 0; i < 10; i++) {  //10 x 10KB
            start = i * 10 * 1024;  // Histogram one time with 2MB 
         
            for (j = 0; j < (10*1024)/writ_size; j++) { //Histogram one time with 2MB 
                start += writ_size;             
                ticks = read_c0_count32();
                rc = sd_write_sector (sd, start/512, writ_size/512, 512, buf);
                if (rc < 0) {
                    printf("Failed to write sector i=%d j=%d rc=%d\n", i, j, rc);
                }
                wticks += read_c0_count32() - ticks;     
            }
        }
        total_time = wticks/10/(sys_cpu_clk/2/1000);
        printf("Total Write 10K = %d ms\n", total_time);
        //if(total_time>50)
        //    printf("Low performance SD Card!\n");
    return total_time;
}
