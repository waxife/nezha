/**
 *  @file   nor.c
 *  @brief  nor flash driver
 *  $Id: nor2.c,v 1.21 2015/06/17 03:03:41 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.21 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/03/15  New file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <cache.h>
#include <io.h>
//#define DBG_LEVEL   3
#include <debug.h>
#include <dma.h>
#include <syserr.h>
#include <sys.h>
#include <gpio.h>
#include <mipsregs.h>
#include <nor.h>


#define MAGIC_NOR                   0x20902090
//#define MAX_PA          0x40000000

#define NOR_CTRL_BASE               0xBD800000
#define NOR_RD_PORT                 (NOR_CTRL_BASE + 0x00) 
#define NOR_WR_PORT                 (NOR_CTRL_BASE + 0x04)
#define AUTO_RDSR_CFG               (NOR_CTRL_BASE + 0x08)
#define IP_STATUS                   (NOR_CTRL_BASE + 0x10)
#define NOR_GENERAL_CTRL            (NOR_CTRL_BASE + 0x14)
#define NOR_TIMING_PARA             (NOR_CTRL_BASE + 0x18)
#define SPI_CMD                     (NOR_CTRL_BASE + 0x20)
#define SPI_ADDR                    (NOR_CTRL_BASE + 0x24)
#define NOR_PPI_CMD                 (NOR_CTRL_BASE + 0x30)
#define PPI_ERASE_RDSR_ADDR         (NOR_CTRL_BASE + 0x38)
#define PPI_Pre_Idle_CMD            (NOR_CTRL_BASE + 0x3c)
#define PPI_Suspend_CMD             (NOR_CTRL_BASE + 0x40)
#define PPI_Resume_CMD              (NOR_CTRL_BASE + 0x44)
#define PPI_Suspend_Latency_CMD     (NOR_CTRL_BASE + 0x48)
#define NOR_STATUS                  (NOR_CTRL_BASE + 0x4c)
#define NOR_PPI_CYCLE_1             (NOR_CTRL_BASE + 0x50)
#define NOR_PPI_CYCLE_2             (NOR_CTRL_BASE + 0x54)
#define NOR_PPI_CYCLE_3             (NOR_CTRL_BASE + 0x58)
#define NOR_PPI_CYCLE_4             (NOR_CTRL_BASE + 0x5c)
#define NOR_PPI_CYCLE_5             (NOR_CTRL_BASE + 0x60)
#define NOR_PPI_CYCLE_6             (NOR_CTRL_BASE + 0x64)
#define NOR_PPI_CYCLE_7             (NOR_CTRL_BASE + 0x68)
#define PPI_PROGRAM_DATA_CYCLE      (NOR_CTRL_BASE + 0x6c)

#define SPI_DMA             (1 << 18)
#define SPI_READ            (0 << 16)
#define SPI_WRITE           (1 << 16)
#define SPI_W_0             (0 << 10)
#define SPI_W_1             (0x4 << 10)
#define SPI_W_3             (0x5 << 10)
#define SPI_W_4             (0x6 << 10)
#define SPI_W_7             (0x7 << 10)
#define SPI_R_0             (0 << 8)
#define SPI_R_1             (1 << 8)
#define SPI_R_3             (2 << 8)
#define SPI_R_4             (3 << 8)

#define I_WREN              0x06
#define I_WRDI              0x04
#define I_RDID              0x9f
#define I_RDSR              0x05
#define I_WRSR              0x01
#define I_READ              0x03
#define I_FAST_READ         0x0b
#define I_PP                0x02
#define I_SE                0xd8
#define I_BE                0xc7
#define I_DP                0xb9
#define I_RES               0xab

#define S_WIP               (1 << 24)
#define S_WEL               (1 << 25)

#define CMD_WREN            (SPI_CMD | SPI_WRITE | SPI_W_0 | I_WREN)
#define CMD_WRITE1          (SPI_CMD | SPI_WRITE | SPI_W_4 | I_PP)
#define CMD_SE              (SPI_CMD | SPI_WRITE | SPI_W_3 | I_SE)
#define CMD_BE              (SPI_CMD | SPI_WRITE | SPI_W_0 | I_BE)
#define CMD_WRITE4          (SPI_DMA | SPI_CMD | SPI_WRITE | SPI_W_7 | I_PP)
#define CMD_SPI_RDID        (SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 3)

#define DATA_CNT_0          0
#define DATA_CNT_1          1
#define DATA_CNT_2          2
#define DATA_CNT_3          3
#define DATA_CNT_4          4
#define DATA_CNT_32         32



#define ERASE_WR_ERR_INT    (1 << 1)
#define ERASE_WR_OK_INT     (1 << 0)

//eddy 20080528+
#define OPC_PIORD           (0x03 << 24)
#define OPC_RDID            (0x9f << 24)    //CFI Data
#define OPC_WREN            (0x06 << 24)
#define OPC_SECERASE        (0x20 << 24)
#define OPC_BLOCKERASE      (0xd8 << 24)
#define OPC_CHIPERASE       (0xc7 << 24)
#define OPC_RDSR            (0x05 << 24)
#define OPC_WRITE           (0x02 << 24)
#define OPC_WRSR            (0x01 << 24)

#define SPI_CMD_TYPE_RD     (0 << 22)
#define SPI_CMD_TYPE_BYTEWR (1 << 22)
#define SPI_CMD_TYPE_PAGEWR (2 << 22)
#define SPI_CMD_TYPE_ERASE  (3 << 22)

#define AUTO_RDSR_EN        (1 << 21)
#define AUTO_RDSR_DIS       (0 << 21)

#define ADDR_CYCLE_0        (0 << 18)
#define ADDR_CYCLE_1        (1 << 18)
#define ADDR_CYCLE_2        (2 << 18)
#define ADDR_CYCLE_3        (3 << 18)
#define ADDR_CYCLE_4        (4 << 18)
#define ADDR_CYCLE_5        (5 << 18)
#define ADDR_CYCLE_6        (6 << 18)
#define ADDR_CYCLE_7        (7 << 18)

#define CMD_SPI_PIORD       (OPC_PIORD | SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_3)
#define CMD_SPI_BYTEWR      (OPC_WRITE | SPI_CMD_TYPE_BYTEWR | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_PAGEWR      (OPC_WRITE | SPI_CMD_TYPE_PAGEWR | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_WREN        (OPC_WREN | SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0)
#define CMD_SPI_SECERASE    (OPC_SECERASE | SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_BLOCKERASE  (OPC_BLOCKERASE | SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_3)
#define CMD_SPI_CHIPERASE   (OPC_CHIPERASE | SPI_CMD_TYPE_ERASE | AUTO_RDSR_EN | ADDR_CYCLE_0)
#define CMD_SPI_RDID        (SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 3)
#define CMD_SPI_RDSR        (OPC_RDSR|SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1)
#define CMD_SPI_WRSR        (OPC_WRSR|SPI_CMD_TYPE_BYTEWR | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1)
//#define CMD_SPI_RDID      (OPC_RDID | SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 3)

#define RESET_WRITE_POINT   0x100

#define PROTECT_BIT         (0x7 << 2)
#define AH1_BASE             0xb9800000
#define AH1_DTBASE          (AH1_BASE + 0x40)
#define AH1_REGBASE         (AH1_BASE + 0x80)
#define AH1_DT_NOR          (AH1_DTBASE + 0x10)
#define AH1_REG_NOR         (AH1_REGBASE + 0x10)
#define AH1_DT_SIZE         0x30000
#define AH1_NOR_DT_SIZE     0x40000

#define NOR_DIRECT_READ     0x80000000



/**
 * @func    ba2pa
 * @brief   flash block address to physical address
 * @param   norc        nor control handle
 * @param   ba          flash block address
 * @return  return pa, physical address
 */
int ba2pa(struct norc_t *norc, int ba)
{
    int i = 0;
    for (i = 1; i < 16; i++) {
        if (ba < norc->layout[i].ba) {
            return norc->layout[i-1].pa + ((ba - norc->layout[i-1].ba) * norc->layout[i-1].ba_size);
        }
    }
    return -1;
}

#if 0
static int ba_size(struct norc_t *norc, int pa)
{
    int i = 0;
    for (i = 1;i < 16; i++) {
        if (pa < norc->layout[i].pa)
            return norc->layout[i-1].ba_size;
    }
    return 0;    
}
#endif



static int nor_wait_erase_write_done(void)
{
    int i;
    uint32_t st;
    for (i = 0 ;i < 10000; i++) {
        if ((st = readl(NOR_STATUS)) & 3)
            break;
        usleep(5);
    }
    
    assert(i < 10000);

    writel(st, NOR_STATUS);
    
    if (st & 1<<1)
        return -EIO;

    return 0;
}

static int nor_wait_cmd_ready(void)
{
    int i;
    for (i = 0 ; i < 10000; i++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;
        usleep(1);
    }

    assert(i < 10000);

    return 0;
}

/********************************************************************
 * nor_init
 ********************************************************************/
#define BOOTSECTOR_MARK 0x55aa55aa
unsigned long nor_sizing(void)
{
    unsigned long addr = 65536;
    unsigned long tag;
    while(addr <= 0x10000000) {
       tag = readl(addr + NOR_DIRECT_READ);
       if (tag == BOOTSECTOR_MARK)
           return addr;
       addr <<= 1; 
    }

    return 0;
}

int nor_init(struct norc_t *norc, char *layout)
{
    char *p = NULL;
    int expect = '?';
    int i = 0;
    int ba_end = 0;
    int pa = 0;
    unsigned long size;    
    
    
    //chiptype = &t373_op;         
    
    memset(norc, 0, sizeof(struct norc_t));
    p = layout;
    for (i = 0; i < 15; i++) {
        if (p == NULL || *p == '\0')
            break;
        expect = '(';
        if (*p != expect)
            goto INV_LAYOUT;
        p++;
        norc->layout[i].ba_size = strtol(p, &p, 10) * 1024;
        expect = ',';
        if (*p != expect)
            goto INV_LAYOUT;  
        p++;
        norc->layout[i].ba = strtol(p, &p, 10);
        expect = '-';
        if (*p != expect)
            goto INV_LAYOUT;
        p++;
        if ((i == 0 && norc->layout[i].ba != 0) || 
            (i > 0 && norc->layout[i].ba != ba_end + 1) ||
            (norc->layout[i].ba_size <= 0)) { 
            dbg(2, "invalid layout[%d] %s ba %d ba_end %d \n", i, layout, norc->layout[i].ba, ba_end);
            return -1;
        }
 
        ba_end = strtol(p, &p, 10);
        norc->last_ba = ba_end;
        expect = ')';
        if (*p != expect)
            goto INV_LAYOUT;
        p++;

        norc->layout[i].pa = pa;
        pa += (ba_end - norc->layout[i].ba + 1) * norc->layout[i].ba_size;
    }
        
    norc->layout[i].pa = pa;
    norc->layout[i].ba = ba_end + 1;
    norc->capacity = pa;

    /* success to init nor flash */
    norc->magic = MAGIC_NOR;
    
    writel(0x05020038, AUTO_RDSR_CFG);//eddy

    size = nor_sizing();
    printf("nor_init:config nor capacity %ld\n", norc->capacity);
    printf("         real nor capacity   %ld\n", size); 
    if (size < norc->capacity) {
        ERROR("[Force system abort] because\n");
        ERROR("    System detect nor capacity is less than configation\n");
        ERROR("    The wrong may corrupt firmware and cause system crash.\n");
        ERROR("    Please change correct nor flash component\n");
        ERROR("    or change .config/CONFIG_CODE_NOR_LAYOUT\n");
        assert(0);
    } else if (size > norc->capacity) {
        WARN("[Warning] nor flash real capacity is greater than configuration\n");
    }

    return 0;
INV_LAYOUT:
    ERROR("invalid layout string '%s' in '%s' expect '%c' \n", layout, p, expect);
    return -1;
}

/**************************************************************
 * nor id
 **************************************************************/
static int nor_get_id(void)
{
    int rc;
    unsigned int nor_flash_id;
    
    writel( OPC_RDID |(CMD_SPI_RDID&0xffffff), SPI_CMD);
    
    rc = nor_wait_cmd_ready();
    if (rc < 0) {
        ERROR("CMD_SPI_RDID cmd ready Fail\n");
        goto EXIT;
    }
    
    nor_flash_id = readl(NOR_RD_PORT);
    // nor_flash_id = 0x1530ef //winbond example

    return nor_flash_id;
    
EXIT:
    return -EIO;    

}

int nor_manuid(void)
{
    unsigned int nor_id;
    unsigned char manuid;
    
    nor_id = nor_get_id();
    if(nor_id < 0)
        goto EXIT;
    
    manuid = (nor_id & 0xff);
    
    return manuid;
    
EXIT:
    dbg(2, "Fail to get nor flash id \n");
    return -EIO;
}

int nor_deviceid(void)
{
    unsigned int nor_id;
    uint32_t deviceid;
    
    nor_id = nor_get_id();
    if(nor_id < 0)
        goto EXIT;
    deviceid = (nor_id & 0xffff00) >> 8;
    
    return deviceid;

EXIT:
    dbg(2, "Fail to get nor flash id \n");
    return -EIO;
}

/***********************************************************************
 * nor sector erase
 ***********************************************************************/
int nor_sector_erase(int pa)
{    
    uint32_t ti;
    int rc = 0;

    nor_writeprotect(0);

    //Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;
            usleep(100);
    }
    //printf("IP_Status[1]:%x  %d\n", IP_STATUS, readl(IP_STATUS));    
    if (ti >= 90000) {
        dbg(2, "Read IP_Status timeout!!!\n");
        rc = -1;
        goto EXIT;
    }

    //Configure Auto_RDSR_Config (optional)

    //Write Enable
    writel(CMD_SPI_WREN, SPI_CMD);

    //Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++){
        if(readl(IP_STATUS) & (1<<1))
            break;
            usleep(100);
    }

    //printf("IP_Status[1]:%x  %d\n", IP_STATUS, readl(IP_STATUS));       
    if (ti >= 90000) {
        dbg(2, "Write Enable Fail\n");
        rc = -1;
        goto EXIT;
    } else {
        ///dbg(2, "Write Enable Success\n");
    }
        
    //Sector Address for ERASE
    writel(pa, SPI_ADDR);
    //printf("eddy: NOR_STATUS: 0x%x\n", readl(NOR_STATUS));
    //Clear Reg 0x04C[1:0]
    writel(0x03, NOR_STATUS);
    //printf("eddy: NOR_STATUS: 0x%x\n", readl(NOR_STATUS));
    //Configure SPI_COMD_Port
    writel(CMD_SPI_SECERASE, SPI_CMD);

    //Wait Interrupt flag
    //printf("Dora1: NOR_STATUS: 0x%x\n", (readl(NOR_STATUS) & 1));
    for(ti = 0; ti<90000; ti++) {       
        if (readl(NOR_STATUS) & 1){ 
            goto EXIT;                                                             
        }                   
        usleep(1000);
    }
    // printf("Dora2: NOR_STATUS: 0x%x\n", (readl(NOR_STATUS) & 1));
    if(ti >= 90000){
        dbg(2, "SECTOR Erase Fail\n");
        rc = -EIO;
        goto EXIT;
    }
    
EXIT:
    nor_writeprotect(1);

    return rc;
}

/***********************************************************************
 * nor block erase
 ***********************************************************************/
int nor_block_erase(int pa)
{    
    uint32_t ti;
    int rc = 0;

    nor_writeprotect(0);

    //Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;
            usleep(100);
    }
    //printf("IP_Status[1]:%x  %d\n", IP_STATUS, readl(IP_STATUS));    
    if (ti >= 90000) {
        dbg(2, "Read IP_Status timeout!!!\n");
        rc = -1;
        goto EXIT;
    }

    //Configure Auto_RDSR_Config (optional)

    //Write Enable
    writel(CMD_SPI_WREN, SPI_CMD);

    //Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++){
        if(readl(IP_STATUS) & (1<<1))
            break;
            usleep(100);
    }

    //printf("IP_Status[1]:%x  %d\n", IP_STATUS, readl(IP_STATUS));       
    if (ti >= 90000) {
        dbg(2, "Write Enable Fail\n");
        rc = -1;
        goto EXIT;
    } else {
        ///dbg(2, "Write Enable Success\n");
    }
        
    //Sector Address for ERASE
    writel(pa, SPI_ADDR);
    //printf("eddy: NOR_STATUS: 0x%x\n", readl(NOR_STATUS));
    //Clear Reg 0x04C[1:0]
    writel(0x03, NOR_STATUS);
    //printf("eddy: NOR_STATUS: 0x%x\n", readl(NOR_STATUS));
    //Configure SPI_COMD_Port
    writel(CMD_SPI_BLOCKERASE, SPI_CMD);

    //Wait Interrupt flag
    //printf("Dora1: NOR_STATUS: 0x%x\n", (readl(NOR_STATUS) & 1));
    for(ti = 0; ti<90000; ti++) {       
        if (readl(NOR_STATUS) & 1){ 
            goto EXIT;                                                             
        }                   
        usleep(1000);
    }
    // printf("Dora2: NOR_STATUS: 0x%x\n", (readl(NOR_STATUS) & 1));
    if(ti >= 90000){
        dbg(2, "Block Erase Fail\n");
        rc = -EIO;
        goto EXIT;
    }
    
EXIT:
    nor_writeprotect(1);

    return rc;
}

/*************************************************************************
 * nor data write
 *************************************************************************/
int nor_data_write(int pa, int wsize, char *data)
{
    unsigned char *p_end = (unsigned char *)data + wsize;
    unsigned char *p;
    int  wlen, i;
    uint32_t tmp;
    int rc;

    nor_writeprotect(0);

    for (p = (void *)data; p < p_end; pa += wlen) {
        
        /* wait until write port ready */
        rc = nor_wait_cmd_ready();
        if (rc < 0)
            goto EXIT;

        /* reset buffer pointer */
        writel(RESET_WRITE_POINT, NOR_GENERAL_CTRL);

        /* write each 32 bytes data to spi port until all done */
        /* page program require data should be programmed on the same page */
        tmp = 0;
        for (i = 0, wlen = 0; wlen < 32 && p < p_end ; wlen++, p++) {
            if (((pa + wlen) & 0xff) == 0 && wlen > 0)
                break;
                            

            tmp |= ((unsigned int) *p) << (i*8);

            i++;
            if (i == 4) {
                writel(tmp, NOR_WR_PORT);    
                i = 0;
                tmp = 0;
            }                

        }

        if (i != 0) {
            writel(tmp, NOR_WR_PORT);
        }

        rc = nor_wait_cmd_ready();
        if (rc < 0)
            goto EXIT;
        writel(CMD_SPI_WREN, SPI_CMD);
        rc = nor_wait_cmd_ready();
        if (rc < 0)
            goto EXIT;
        writel(pa, SPI_ADDR);
        writel((CMD_SPI_PAGEWR | wlen), SPI_CMD);

        rc = nor_wait_erase_write_done();
        if (rc < 0)
            goto EXIT;
    }

    nor_writeprotect(1);

    return 0;

EXIT:
    ERROR("failed to program spi nor flash pa = %x wsize = %d data = %p\n", pa, wsize, data);
    nor_writeprotect(1);
    //sti();
    return -1;
}


/*********************************************************************
 * nor status register : read / write
 *********************************************************************/
int nor_wren(void)
{
	uint32_t ti;

	//Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;

        usleep(1);
    }

    if (ti >= 80000) {
        dbg(0, "wait IP_STATTUS failed.\n");
        goto EXIT;
    }

	//Write Enable
    writel(CMD_SPI_WREN, SPI_CMD);

    for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;

        usleep(1);
    }

	if(ti >= 80000) {
        dbg(0, "wait IP_STATTUS failed.\n");
		goto EXIT;
    }

	return 0;
EXIT:
	return -EIO;
}

int nor_rdsr(int *state)
{
	uint32_t ti;

	writel(CMD_SPI_RDSR, SPI_CMD);
	for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;

        usleep(10);
    }

	if(ti >= 80000)
		goto EXIT;

	*state = readl(NOR_RD_PORT);

	return 0;
EXIT:
	return -EIO;
}	

int nor_wrsr(unsigned int value)
{
	uint32_t ti;

	/*write enable*/
	nor_wren();

	/*reset data point*/
	writel(RESET_WRITE_POINT, NOR_GENERAL_CTRL);

	/*write status*/
	writel(value, NOR_WR_PORT);
	for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;

        usleep(1);
    }

	if(ti >= 8000) {
        dbg(0, "wait IP_STATTUS failed.\n");
		goto EXIT;
    }

	/*execute command*/
    writel((OPC_WRSR | SPI_CMD_TYPE_BYTEWR| AUTO_RDSR_EN | 1), SPI_CMD);

	return 0;

EXIT:
	return -EIO;
}

int nor_is_writeprotect(void)
{
	struct sysc_t *sys = &gSYSC;
    struct norc_t *norc = sys->pnor;
    unsigned int status;

    dbg(2, "write protect count is %ld\n", norc->wp_cnt);

    nor_rdsr(&status);

    return ((status & 0x1C) == 0x1C);
}

void nor_writeprotect(char enable)
{
	struct sysc_t *sys = &gSYSC;
    struct norc_t *norc = sys->pnor;

    if (enable) {
        if (norc->wp_cnt == 0) {
            dbg(3, "NOR write protect\n");
            nor_wrsr (0x3C);
        }

        norc->wp_cnt++;
    } else {
        if (norc->wp_cnt == 1) {
            dbg(3, "NOR write unprotect\n");
            nor_wrsr (0x00);
        }

        norc->wp_cnt--;
    }

    dbg(3, "write protect count is %ld\n", norc->wp_cnt);
}

/**********************************************************************
 * DEPRECATED Function: no longer support
 **********************************************************************/
// #define align4(x)   (((x)+3) & (-4))

/*
 * read data from nor flash into dst
 */
#if 0
int nor_data_flush(void *sys_ctx)
{
	struct sys_ctx_t *sys = (struct sys_ctx_t *)sys_ctx;
    struct norc_t *norc = sys->pnor;
    //struct norc_t *norc;
    int spa, size;
    int rc = 0;
    if (norc->cache_ba >= 0 && norc->cache_dirty) {
        spa = ba2pa(norc, norc->cache_ba);
        size = ba_size(norc, spa);
        rc = nor_block_erase(spa);
        if (rc < 0) 
            goto EXIT;        
        rc = nor_data_write(spa, size, (void *)norc->cache);
        if (rc < 0) 
            goto EXIT;
    }
    norc->cache_dirty = 0;
    return 0;
EXIT:
    return -1;
}
#endif
#if 0
static int memcpy4(char *dst, char *src, int size)
{
    long *ls = (long *)src;
    uint32_t data;
    for(; size >= 4; size-=4, ls++, dst+=4) {
        data = *ls;
        memcpy(dst, &data, sizeof(long));
    }
    if (size) {
        data = *ls;
        memcpy(dst, &data, size);
    }
    return 0;
}
#endif

/*
int nor_idx2pa(struct norc_t *norc, int idx)
{
    int i = 0;
    int ba = norc->ba_start + (idx / 2);
    int pa;
    if (ba > norc->ba_end)
        ba = norc->ba_end;
    for (i = 1; i < 16; i++) {
        if (ba < norc->layout[i].ba) {
            pa = norc->layout[i-1].pa + ((ba - norc->layout[i-1].ba) * norc->layout[i-1].ba_size);
            if (idx % 2)
                pa += 0x8000; // 32KB 
            return pa;
        }
    }
    return -1;
}*/


