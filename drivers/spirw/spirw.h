/**
 *  @file   spirw.h
 *  @brief  head file for spirw
 *  $Id: spirw.h,v 1.3 2014/04/25 09:21:18 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/11/22  yc 	New file.
 *
 */

#ifndef _SPIRW_H
#define _SPIRW_H

#define WINBOND    0XEF
#define AMIC       0X37
#define MXIC       0XC2
#define GD         0XC8
#define EON   	   0x1c
#define PMC		   0X7F

//WINBOND
#define W25Q80BV_ID   0X4014
#define W25Q16BV_ID   0X4015
#define W25Q32BV_ID   0X4016
#define W25Q64BV_ID   0X4017
#define W25Q128BV_ID  0X4018

//MXIC
#define  MX25L3235D_ID 0x5e16
#define  MX25L6445E_ID 0X2017
#define  MX25L8035E_ID 0x2014
#define  MX25L1636_ID  0x2515
#define  MX25L12835F_ID 0x2018

//GD
#define  GD25Q64B_ID   0x4017
#define  GD25Q128B_ID  0x4018

//AMIC
#define  A25LQ32A_ID   0X4016

//EON
#define EN25Q32A_ID    0x3016
#define EN25Q64_ID     0x3017

//PMC
#define PM25LQ032      0x9d46 

//type

#define OSPI_TYPE_A  0  // WRSR 01 -> (STATUS 0~7,8~15)  two bytes
#define OSPI_TYPE_B  1  // WRSR 05 35 15 (STATUS 0~7) (STATUS 8~15) (STATUS 16~23) three bytes 
#define OSPI_TYPE_C  2  // WRSR 01 -> (STATUS 0~7) one byte
#define UN_KNOW_MF_ID 0xff
#define UN_KNOW_DV_ID 0xff
#define UN_KNOW_TY_ID 0xff

#define OSPI_TYPE_A_HPM_TOGGLE_VALUE 0xAA 
#define OSPI_TYPE_B_HPM_TOGGLE_VALUE 0xAA
#define OSPI_TYPE_C_HPM_TOGGLE_VALUE 0xA5

#define OSPI_TYPE_A_NON_TOGGLE_VALUE 0X55
#define OSPI_TYPE_B_NON_TOGGLE_VALUE 0x55
#define OSPI_TYPE_C_NON_TOGGLE_VALUE 0XCC

struct ospi_type_id {
    unsigned int manu_id;
    unsigned int dev_id;
	unsigned int type;
};

static const struct ospi_type_id ospi_id[]={
{WINBOND,W25Q80BV_ID,OSPI_TYPE_A},
{WINBOND,W25Q16BV_ID,OSPI_TYPE_A},
{WINBOND,W25Q32BV_ID,OSPI_TYPE_A},
{WINBOND,W25Q64BV_ID,OSPI_TYPE_A},
{WINBOND,W25Q128BV_ID,OSPI_TYPE_B},
{AMIC,A25LQ32A_ID,OSPI_TYPE_A},
{MXIC,MX25L3235D_ID,OSPI_TYPE_C},
{MXIC,MX25L6445E_ID,OSPI_TYPE_C},
{MXIC,MX25L8035E_ID,OSPI_TYPE_C},
{MXIC,MX25L1636_ID,OSPI_TYPE_C},
{MXIC,MX25L12835F_ID,OSPI_TYPE_C},
{GD,GD25Q64B_ID,OSPI_TYPE_A},
{GD,GD25Q128B_ID,OSPI_TYPE_B},
{EON,EN25Q32A_ID,OSPI_TYPE_C},
{EON,EN25Q64_ID,OSPI_TYPE_C},
{PMC,PM25LQ032,OSPI_TYPE_C},
{UN_KNOW_MF_ID,UN_KNOW_DV_ID,UN_KNOW_TY_ID},
};

/**
 * @func    enable_quad_io
 * @brief   enable NOR Flash of Quad mode and command must ref. NOR of datasheet.
 * @param   none
 * @return  0 if successfully, or 1 if fail
 */
extern int
enable_quad_io (void);
/**
 * @func    ounprotected
 * @brief   disable NOR flash of block protect.
 * @param   none
 * @return  0 if successfully, or 1 if fail
 */
extern int
ounprotected (void);
/**
 * @func    ospi_toggle_byte_set
 * @brief   enable SPIOSD of Quad mode for display OSD.
 * @param   none
 * @return  none
 */
extern void
spi_quad_mode_enable (void);
/**
 * @func    spi_quad_mode_disable
 * @brief   disable SPIOSD of Quad mode.
 * @param   none
 * @return  none
 */
extern void
spi_quad_mode_disable (void);

/*
 * SPI Master Control Functions
 */

/**
 * @func    spi_dma2oram
 * @brief   waiting DMA done by check state register.
 * @param   base_address	NOR Flash of base address
 * 			oram_address	OSDRAM of point address
 * 			length			DMA of length
 * @return  0 if DAMA done, 1 if timeout
 * @note	OSDRam address is Word Unit, Flash Address is Byte Unit,
 * 			blit length is also Byte Unit
 */
extern int
spi_dma2oram (unsigned long base_address, unsigned short oram_address, unsigned int length);
/**
 * @func    spi_wren
 * @brief   SPI NOR of write enable command.
 * @param   none
 * @return  0 if successfully, or 1 if some error
 */
extern int
spi_wren (void);
/**
 * @func    spi_wrsr
 * @brief   SPI NOR of write status register command,
 * 			command need ref. NOR of datasheet.
 * @param   reg1	register value1
 * 			reg2	register value2
 * @return  0 if successfully, or 1 if some error
 */
extern unsigned char
spi_read_byte(unsigned long address);
/**
 * @func    spi_erase_sector
 * @brief   SPI sector erase.
 * 			command need ref. NOR of datasheet.
 * @param   unsigned long address	NOR Flash of address
 * @return  0 if successfully, or 1 if some error
 */
extern int
spi_erase_sector (unsigned long address);
/**
 * @func    spi_chip_erase
 * @brief   SPI chip erase.
 * 			command need ref. NOR of datasheet.
 * @param   none
 * @return  0 if successfully, or 1 if some error
 */
extern int
spi_chip_erase (void);


/*
 * OSPI Master Control Functions
 */

/**
 * @func    ospi_wren
 * @brief   SPI NOR of write enable command.
 * @param   none
 * @return  0 if successfully, or 1 if some error
 */
extern int
ospi_wren (void);
/**
 * @func    ospi_dma_write
 * @brief   Program SPI Flash ROM via DMA moving from sLUT (256/384/HSize/HSize).
 * @param   unsigned long address	NOR Flash of address
			unsigned char cnt_mode	DMA of count (length) mode
 * @return  0 if successfully, or 1 if some error
 */
extern int
ospi_dma_write (unsigned char *RomData, unsigned long address);
/**
 * @func    spi_read_byte
 * @brief   Read byte from SPI Flash ROM.
 * @param   unsigned long address	NOR Flash of address
 * @return  data
 */
extern unsigned char
ospi_read_byte (unsigned long address);
/**
 * @func    initial_lut
 * @brief   initial SPIOSD LUT of registers for upgrade.
 * @param   none
 * @return  none
 */
extern void
initial_lut (void);

/**
 * @func    spi_read_id
 * @brief   Read Flash ID from SPI Flash ROM.
 * @param   value		read id store in this point 
 * @return  0 if successfully, or 1 if some error
 * @note	none
 */
extern int
spi_read_id(int *value);

extern  int
spi_check_id(unsigned short manu_id,unsigned short dev_id);


#endif	/* _SPIRW_H */
