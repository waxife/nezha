/**
 *  @file   sd.h
 *  @brief  sd structuture header file
 *  $Id: sd.h,v 1.10 2014/07/16 06:35:03 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.10 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/24  jedy New file.
 *
 */

#ifndef __SD_H
#define __SD_H

/* response type */
#define NR      0       /* NR : no response */   
#define R1      1       /* R1 : [45:40] cmd idx [39:8] card status response */
#define R2      2       /* R2 : [133:128] all 1 [127:1] CID, CSD register response */    
#define R3      3       /* R3 : [45:40] all 1   [39:38] OCR register response */
#define R6      6       /* R6 : [45:40] cmd idx [39:8] RCA register response */
#define R7      7       /* R7 : [45:40] CMD8    [39:20] all 0 [19:16] 0001 [15:8] echo pattern */

#define QUITE(rt)       (-(rt))
#define IS_QUITE(rt)    ((rt) < 0)

#define CMD(cmd)        ((cmd) | CMD_CMD_EN)
#define ACMD(cmd)       ((cmd) | CMD_APP_CMD | CMD_CMD_EN)


#define CONFIG_ALWAYS_SD_POWER_ON

#define MMC_BUG


#define T300_SDC_BASE_T      			0xba800000
#define SDC_CMD_REG						(T300_SDC_BASE_T + 0x0000)
#define SDC_ARGU_REG					(T300_SDC_BASE_T + 0x0004)
#define SDC_RESPONSE0_REG				(T300_SDC_BASE_T + 0x0008)
#define SDC_RESPONSE1_REG				(T300_SDC_BASE_T + 0x000C)
#define SDC_RESPONSE2_REG				(T300_SDC_BASE_T + 0x0010)
#define SDC_RESPONSE3_REG				(T300_SDC_BASE_T + 0x0014)
#define SDC_RSP_CMD_REG					(T300_SDC_BASE_T + 0x0018)
#define SDC_DATA_CTRL_REG				(T300_SDC_BASE_T + 0x001C)
#define SDC_DATA_TIMER_REG			    (T300_SDC_BASE_T + 0x0020)
#define SDC_DATA_LEN_REG				(T300_SDC_BASE_T + 0x0024)
#define SDC_STATUS_REG					(T300_SDC_BASE_T + 0x0028)
#define SDC_CLEAR_REG					(T300_SDC_BASE_T + 0x002C)
#define SDC_INT_MASK_REG				(T300_SDC_BASE_T + 0x0030)
#define SDC_POWER_CTRL_REG			    (T300_SDC_BASE_T + 0x0034)
#define SDC_CLOCK_CTRL_REG			    (T300_SDC_BASE_T + 0x0038)
#define SDC_BUS_WIDTH_REG				(T300_SDC_BASE_T + 0x003C)
#define SDC_DATA_WINDOW_REG			    (T300_SDC_BASE_T + 0x0040)
#define SDC_FEATURE_REG                 (T300_SDC_BASE_T + 0x0044)

/* command register */
#define CMD_SDC_RST     (1 << 10)
#define CMD_CMD_EN      (1 << 9)
#define CMD_APP_CMD     (1 << 8)
#define CMD_LONG_RSP    (1 << 7)
#define CMD_NEED_RSP    (1 << 6)
#define CMD_CMD_MASK    (0x3f)

/* clock control register */
#define CLK_DIS         (1 << 8)
#define CLK_SD          (1 << 7)
#define CLK_DIV_MASK    (0x3f)

/* bus width register */
#define WIDE_BUS_SUPPORT        (1 << 3)
#define BUS_WIDTH_4             (1 << 2)
#define BUS_WIDTH_1             (1 << 0)

/* status register */
#define STATUS_WRITE_PROT       (1 << 12)
#define STATUS_CARD_DETECT      (1 << 11)
#define STATUS_CARD_CHANGE      (1 << 10)
#define STATUS_FIFO_ORUN        (1 << 9)
#define STATUS_FIFO_URUN        (1 << 8)
#define STATUS_DATA_END         (1 << 7)
#define STATUS_CMD_SENT         (1 << 6)
#define STATUS_DATA_CRC_OK      (1 << 5)
#define STATUS_RSP_CRC_OK       (1 << 4)
#define STATUS_DATA_TIMEOUT     (1 << 3)
#define STATUS_RSP_TIMEOUT      (1 << 2)
#define STATUS_DATA_CRC_FAIL    (1 << 1)
#define STATUS_RSP_CRC_FAIL     (1 << 0)

/* data register */
#define DATA_EN                 (1 << 6)
#define DATA_DMA                (1 << 5)
#define DATA_WRITE              (1 << 4)
#define DATA_READ               0

#define BLK_SIZE_8              3
#define BLK_SIZE_256            8
#define BLK_SIZE_512            9
#define BLK_SIZE_1024           10
#define BLK_SIZE_2048           11


/* SD card status */
#define SD_OCR_POWER_UP                     (1 << 31)

#define SD_STATUS_OUT_OF_RANGE				(1 << 31)
#define SD_STATUS_ADDRESS_ERROR				(1 << 30)
#define SD_STATUS_BLOCK_LEN_ERROR			(1 << 29)
#define SD_STATUS_ERASE_SEQ_ERROR			(1 << 28)
#define SD_STATUS_ERASE_PARAM				(1 << 27)
#define SD_STATUS_WP_VIOLATION				(1 << 26)
#define SD_STATUS_CARD_IS_LOCK				(1 << 25)
#define SD_STATUS_LOCK_UNLOCK_FILED			(1 << 24)
#define SD_STATUS_COM_CRC_ERROR				(1 << 23)
#define SD_STATUS_ILLEGAL_COMMAND			(1 << 22)
#define SD_STATUS_CARD_ECC_FAILED			(1 << 21)
#define SD_STATUS_CC_ERROR					(1 << 20)
#define SD_STATUS_ERROR						(1 << 19)
#define SD_STATUS_UNDERRUN					(1 << 18)
#define SD_STATUS_OVERRUN					(1 << 17)
#define SD_STATUS_CID_CSD_OVERWRITE			(1 << 16)
#define SD_STATUS_WP_ERASE_SKIP				(1 << 15)
#define SD_STATUS_CARD_ECC_DISABLE			(1 << 14)
#define SD_STATUS_ERASE_RESET				(1 << 13)
#define SD_STATUS_CURRENT_STATE				(0xf << 9)
#define SD_STATUS_READY_FOR_DATA			(1 << 8)
#define SD_STATUS_APP_CMD					(1 << 5)
#define SD_STATUS_AKE_SEQ_ERROR				(1 << 3)

#define SD_STATUS_ERROR_BITS	\
                (SD_STATUS_OUT_OF_RANGE | SD_STATUS_ADDRESS_ERROR | \
				SD_STATUS_BLOCK_LEN_ERROR | SD_STATUS_ERASE_SEQ_ERROR | \
                SD_STATUS_ERASE_PARAM | SD_STATUS_WP_VIOLATION | \
                SD_STATUS_LOCK_UNLOCK_FILED | SD_STATUS_CARD_ECC_FAILED | \
                SD_STATUS_CC_ERROR | SD_STATUS_ERROR | \
                SD_STATUS_UNDERRUN | SD_STATUS_OVERRUN | \
                SD_STATUS_CID_CSD_OVERWRITE | SD_STATUS_WP_ERASE_SKIP | \
                SD_STATUS_AKE_SEQ_ERROR)

#define CARD_ERR(xcardstatus)   ((xcardstatus)&(SD_STATUS_ERROR_BITS))

#define K       1000
#define KK      (1000*1000)

#define IS_QUITE(rt)    ((rt) < 0)

#define MAX_MULTIBLOCK_NUM      126

extern unsigned int sdc_debug;
#define SDC_DEBUG_READ   (1 << 0)

/*
 * The CSD (card specific data) register bit structure 
 */
typedef struct sd_csd_bit
{
	unsigned long NotUsed:1;	            /* [0] */
	unsigned long CRC:7;                    /* [7:1] */
	unsigned long MMCardReserved1:2;        /* [9:8] */
	unsigned long FILE_FORMAT:2;            /* [11:10] */
	unsigned long TMP_WRITE_PROTECT:1;      /* [12] */
	unsigned long PERM_WRITE_PROTECT:1;     /* [13] */
	unsigned long COPY:1;                   /* [14] */
	unsigned long FILE_FORMAT_GRP:1;        /* [15] */
	
	unsigned long Reserved2:5;              /* [20:16] */
	unsigned long WRITE_BL_PARTIAL:1;       /* [21] */
	unsigned long WRITE_BL_LEN:4;           /* [25:22] */
	unsigned long R2W_FACTOR:3;             /* [28:26] */
	unsigned long MMCardReserved0:2;        /* [30:29] */
	unsigned long WP_GRP_ENABLE:1;          /* [31] */
	
	/* Word 1 */
	unsigned long WP_GRP_SIZE:7;            /* [38:32] */
	unsigned long ERASE_SECTOR_SIZE:7;      /* [45:39] */
	unsigned long ERASE_BLK_ENABLE:1;       /* [46] */
	unsigned long C_SIZE_MULT:3;            /* [49:47] */
	unsigned long VDD_W_CURR_MAX:3;         /* [52:50] */
	unsigned long VDD_W_CURR_MIN:3;         /* [55:53] */
	unsigned long VDD_R_CURR_MAX:3;         /* [58:56] */
	unsigned long VDD_R_CURR_MIN:3;         /* [61:59] */
	
	unsigned long C_SIZE_1:2;               /* [63:62] */
    /* Word 2 */
	unsigned long C_SIZE_2:10;              /* [73:64] */
		
	unsigned long Reserved1:2;              /* [75:74]  */
	unsigned long DSR_IMP:1;                /* [76] */
	unsigned long READ_BLK_MISALIGN:1;      /* [77] */
	unsigned long WRITE_BLK_MISALIGN:1;     /* [78] */
	unsigned long READ_BL_PARTIAL:1;        /* [79] */
	
	unsigned long READ_BL_LEN:4;	        /* [83:80]  */
	unsigned long CCC:12;                   /* [95:84]  */     
	                
	/* Word 3 */
	unsigned long TRAN_SPEED_RateUnit:3;    /* [98:96]  */
	unsigned long TRAN_SPEED_TimeValue:4;   /* [102:99] */
	unsigned long TRAN_SPEED_Reserved:1;    /* [103] */
	
	unsigned long NSAC:8;                   /* [111:104] */
	
	unsigned long TAAC_TimeUnit:3;          /* [114:112] */
	unsigned long TAAC_TimeValue:4;         /* [119:115] */
	unsigned long TAAC_Reserved:1;          /* [119] */
	
	unsigned long Reserved0:2;              /* [122:120] */
	unsigned long MMC_SPEC_VERS:4;          /* [125:122] */
	unsigned long CSD_STRUCTURE:2;	        /* [127:126] */
} sd_csd_bit_t;

#define SDHC_CARD       2
#define SD_CARD         1
#define MMC_CARD        0


struct sd_card {
    int16_t     type;      
    int16_t     sd_ver;             /* sd phys specification ver */
    uint16_t    read_bl_len;        /* read block length */
    uint16_t    write_bl_len;       /* write block length */
    uint32_t    nblocks;            /* number of blocks */
    uint32_t    capacity;           /* card capacity */
    int         readtimeout;
    int         writetimeout;
    int         rblksize;
    int         wblksize;
    int         speed;
    int         div;
    int         taac;
    int         nsac;
    uint32_t    csd[4];
    uint32_t    cid[4];
    uint32_t    scr[2];
};


#define     MAXTIMEOUT      0x1fffffff

#define MAGIC_SDC       0x2d2dd2d2
#define MAX_LLD_NUM		20
struct slld_t {
    uint32_t    srcaddr;
    uint32_t    dstaddr;
    uint32_t    llp;
    uint32_t    control;
    uint32_t    totsize;
};

enum SDC_STATE {
    SDC_NONE = 0,
    SDC_INSERT,
    SDC_MOUNTED,
    SDC_MOUNTED_RDONLY
};

typedef struct sdc {
    uint32_t        magic;
    int            state;
    uint32_t        rca;
    struct sd_card  card;
    uint32_t        cmd20:1;            /* support CMD20 */
    uint32_t        cmd23:1;            /* support CMD23 */
    uint32_t        wp: 1;
    uint32_t        one_bit_mode:1;
    uint32_t        active:1;
    uint32_t        rdy:1;
    uint32_t        stop:1;
    uint32_t        read;
    uint32_t        write;
    int             data_err;
    int             rsp_crc_err;
    int             rsp_timeout_err;
//    int             active;
//    int				rdy;
//    int				stop;
#if 1 // sherman test ...    
    int				multi_dma;
    int				dma;
    struct slld_t	lld[MAX_LLD_NUM];
    uint8_t			llp_cnt;
#endif    
} sdc_t;

#define SD_CARD_NOT_READY 1001
#define SD_CTRL_NOT_READY 1002

void sd_donothing(void);
void sd_drv_init(void *argu);
int sdc_carddetected(void);
int sdc_status(void);

/* sd card status */
enum {
    SD_ST_IDLE      = 0,
    SD_ST_READY     = 1,
    SD_ST_IDENT     = 2, 
    SD_ST_STBY      = 3, 
    SD_ST_TRAN      = 4, 
    SD_ST_DATA      = 5, 
    SD_ST_RCV       = 6, 
    SD_ST_PRG       = 7, 
    SD_ST_DIS       = 8
};

/* macros for setting SDC */
#define SDC_SENDCMD(SDC, CMD, ARG, RT) ({ int STATUS; writel (0x7ff, SDC_CLEAR_REG); writel ((ARG), SDC_ARGU_REG); writel ((CMD) | CMD_NEED_RSP, SDC_CMD_REG); while (((STATUS = readl(SDC_STATUS_REG)) & 0xff) == 0); readl (SDC_RESPONSE0_REG); })

#define SDC_SETUP(SDC, START, SIZE) ({ unsigned int start = ((SDC)->card.type == SDHC_CARD) ? (START) : (START) * 512; writel ((SDC)->card.readtimeout * 2, SDC_DATA_TIMER_REG); writel (SIZE, SDC_DATA_LEN_REG); writel (BLK_SIZE_512 | DATA_READ | DATA_DMA | DATA_EN, SDC_DATA_CTRL_REG); SDC_SENDCMD ((SDC), ((SIZE) > 512 ? CMD (18) : CMD (17)), start, R1); })

#define SDC_STOP_TRANSMISSION(SDC) SDC_SENDCMD((SDC), CMD(12), 0, R1)


#endif /* __SD_H */

