/**
 *  @file   spirw.c
 *  @brief  terawins SPI control function
 *  $Id: spirw.c,v 1.8 2016/07/15 11:22:48 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/11/22  yc 	New file.
 *
 */
#include <config.h>
#include "debug.h"
#include "string.h"
#include <unistd.h>

//#include "default_config.h"
#include "reg_tw.h"
#include "iccontrol.h"
#include "./drivers/spiosd/spiosd.h"
//#ifdef CONFIG_OSD2_ONLY
#include "./drivers/osd2/osd2.h"
//#endif
#include "spirw.h"

//#define SPIRW_DEBUG_MODE
static unsigned char spi_quad_mode = 0;
static unsigned char spi_hp_mode_use = 0;     // high performance mode

static unsigned char HPM_TOGGLE_VALUE =0;
static unsigned char NONE_TOGGLE_VALUE = 0;

int (*ospi_quade_wrsr)(void);//for different types for a or b or c types

/**
 * @func    enable_quad_io
 * @brief   enable NOR Flash of Quad mode and command must ref. NOR of datasheet.
 * @param   none
 * @return  0 if successfully, or 1 if fail
 */
int
enable_quad_io (void)
{
	unsigned char rc = 0;
	int value = 0;
	if(spi_read_id(&value) == 0){
		if(spi_check_id(value&0xff,(value>>8)&0xffff) == 1)
			goto EXIT;
	}else{
		goto EXIT;
	}
	if(ospi_quade_wrsr())
		goto EXIT;
	spi_quad_mode_enable();             // 4x_dma
	spiosd_quad_mode_enable();          // 4x_sosd
	return rc;
EXIT:
	ERROR("ENABLE QUAD NOR FLASH ERROR \r\n");
	return -1;
}

/**
 * @func    ospi_toggle_byte_set
 * @brief   enable SPIOSD of Quad mode for display OSD.
 * @param   none
 * @return  none
 */
void
spi_quad_mode_enable (void)
{
    spi_quad_mode = 1;
    IC_WriteByte(TWIC_P3, SPI_4X_ENABLE_REG, SPI_4X_DMA);
}

/**
 * @func    spi_quad_mode_disable
 * @brief   disable SPIOSD of Quad mode.
 * @param   none
 * @return  none
 */
void
spi_quad_mode_disable (void)
{
    spi_quad_mode = 0;
   	IC_WriteByte(TWIC_P3, SPI_4X_ENABLE_REG, 0);
}


/*
 * SPI Master Control Functions
 */

/**
 * @func    spi_toggle_byte_set
 * @brief   set QSPI of toggle value and value must ref. NOR of datasheet.
 * @param   none
 * @return  none
 */
static void
spi_toggle_byte_set (void)
{   
    IC_WriteByte(TWIC_P3, SPI_WR_DATA_REG, NONE_TOGGLE_VALUE);
    IC_WriteByte(TWIC_P3, SPI_TOGGLE_WR_DATA, HPM_TOGGLE_VALUE);
}

/**
 * @func    wait_cmd
 * @brief   waiting CMD done by check register.
 * @param   none
 * @return  0 if CMD done, 1 if timeout
 */
static int
wait_cmd (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if(!(IC_ReadByte(TWIC_P3, SPI_COMMAND_REG)&SPI_ISSUE_CMD))
			return 0;
		else
			twdDelay(3);
	}
	return 1;
}

/**
 * @func    wait_wip
 * @brief   waiting WIP done by check register.
 * @param   none
 * @return  0 if WIP done, 1 if timeout
 */
static int
wait_wip (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if((IC_ReadByte(TWIC_P3, SPI_COMMAND_REG)&SPI_CHECK_WIP))
			return 0;
		else
			twdDelay(3);
	}
	return 1;
}

/**
 * @func    wait_dma_int
 * @brief   waiting DMA done by check interrupt.
 * @param   none
 * @return  0 if DAMA done, 1 if timeout
 */
static int
wait_dma_int (void)
{
    unsigned short i = 0;
    for(i = 0; i < 65535  ; i++) {
        if((IC_ReadByte(TWIC_P2, 0xE1) & 0x04)) {
          	IC_WriteByte(TWIC_P2, 0xE1, (IC_ReadByte(TWIC_P2, 0xE1) | 0x04) );
            return 0;
        }
        else
            twdDelay(3);
    }
    DBG_PRINT("Wait DMA Interrupt Timeout\r\n");
    return 0;
}

/**
 * @func    wait_dma
 * @brief   waiting DMA done by check state register.
 * @param   none
 * @return  0 if DAMA done, 1 if timeout
 */
static int
wait_dma (void)
{
	unsigned short count = 0;
	for(count=0; count < 65535; count++) {
		if((IC_ReadByte(TWIC_P3, SPI_DMA_CTRL_REG)&SPI_DMA_GO))
			return 0;
		else
			twdDelay(3);
	}
    DBG_PRINT("Wait DMA Timeout\r\n");
	return 0;
}
#if 0
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
int
spi_dma2oram (unsigned long base_address, unsigned short oram_address, unsigned int length)
{
#ifdef CONFIG_OSD2_ONLY
	/* just read from memory not is really SPI DMA */
	int i = 0;
	char *src = (char*)base_address;

	/* set OSDRAM of point address */
	osd2_set_ram_addr(oram_address);
	/* write data to OSDRAM through data port and auto inc.  */
	for(i=0; i < (length/2); i++) {
		/* word access, LSB first, then MSB byte */
		IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, (unsigned char)src[i*2+0]);
		IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, (unsigned char)src[i*2+1]);
	}

	return 0;
#else
	int rc = 0;

	/* set OSDRAM of point address */
	IC_WriteByte(TWIC_P0, OSD_RAM_AL_REG, (unsigned char)(oram_address & 0xff));
	IC_WriteByte(TWIC_P0, OSD_RAM_AH_REG, (unsigned char)(oram_address>>8));

    /* check QSPI mode */
	if(spi_quad_mode) {
		/* use 4x command */
        IC_WriteByte(TWIC_P3, SPI_4X_ENABLE_REG, SPI_4X_I8051 | SPI_4X_DMA);
      	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
        IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_DMA_4X);
    } else {
    	/* use general command */
        IC_WriteByte(TWIC_P3, SPI_4X_ENABLE_REG, 0);
      	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ);
        IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_DMA);
    }
    
	/* set NOR Flash of base address */
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG1, (base_address&0x000000FF));		    // AL
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, ((base_address>>8)&0x000000FF));	// AM
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, ((base_address>>16)&0x000000FF));	// AH

    /* set QSPI of toggle value */
	spi_toggle_byte_set();
    
	/* set OSDRAM of data port as DMA target */
	IC_WriteByte(TWIC_P3, SPI_DMA_ADDR_REG1, OSDRAM_DATA_PORT&0x00FF);			// OSDRam Dataport
	IC_WriteByte(TWIC_P3, SPI_DMA_ADDR_REG2, (OSDRAM_DATA_PORT>>8)&0x00FF);		// OSDRam Dataport
	/* set DMA of length */
	IC_WriteByte(TWIC_P3, SPI_DMA_COUNT_REG1, length & 0x00FF);					// DMA count;
	IC_WriteByte(TWIC_P3, SPI_DMA_COUNT_REG2,(length & SPI_DMA_COUNT_BIT)>>8);	// DMA count;
	/* DMA go, this write twice and DMA go on secondary,
	 * workaround for unknown bug~ */
	IC_WriteByte(TWIC_P3, SPI_DMA_COUNT_REG2, DMA_READ|((length & SPI_DMA_COUNT_BIT)>>8));	// DMA go;

    // Notice: FPGA Sample Code check P2_E1[2], and need write 1 clear
	/* we should what to do if on real-chip??? */
    if((IC_ReadByte(TWIC_P2, 0xE3) & 0x04)) {
    	// dma interrupt mask
       	if(wait_dma()) {
       		rc = 1;
       		goto EXIT;
       	}
    } else {
    	// dma interrupt unmask
        if(wait_dma_int()) {
        	rc = 1;
			goto EXIT;
        }
    }

EXIT:    
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);

    return rc;
#endif
}
#endif
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
int
spi_dma2oram (unsigned long base_address, unsigned short oram_address, unsigned int length)
{	
	int rc = 0;
	/* just read from memory not is really SPI DMA */
	if(config_osd2_only){        
        mem2oram (base_address, oram_address, length);
		return 0;
	}else{
		/* set OSDRAM of point address */
		IC_WriteByte(TWIC_P0, OSD_RAM_AL_REG, (unsigned char)(oram_address & 0xff));
		IC_WriteByte(TWIC_P0, OSD_RAM_AH_REG, (unsigned char)(oram_address>>8));

		/* check QSPI mode */
		if(spi_quad_mode) {
			/* use 4x command */
			IC_WriteByte(TWIC_P3, SPI_4X_ENABLE_REG, SPI_4X_I8051 | SPI_4X_DMA);
			IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
			IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_DMA_4X);
		} else {
			/* use general command */
			IC_WriteByte(TWIC_P3, SPI_4X_ENABLE_REG, 0);
			IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ);
			IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_DMA);
		}
		
		/* set NOR Flash of base address */
		IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG1, (base_address&0x000000FF));		    // AL
		IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, ((base_address>>8)&0x000000FF));	// AM
		IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, ((base_address>>16)&0x000000FF));	// AH

		/* set QSPI of toggle value */
		spi_toggle_byte_set();
		
		/* set OSDRAM of data port as DMA target */
		IC_WriteByte(TWIC_P3, SPI_DMA_ADDR_REG1, OSDRAM_DATA_PORT&0x00FF);			// OSDRam Dataport
		IC_WriteByte(TWIC_P3, SPI_DMA_ADDR_REG2, (OSDRAM_DATA_PORT>>8)&0x00FF);		// OSDRam Dataport
		/* set DMA of length */
		IC_WriteByte(TWIC_P3, SPI_DMA_COUNT_REG1, length & 0x00FF);					// DMA count;
		IC_WriteByte(TWIC_P3, SPI_DMA_COUNT_REG2,(length & SPI_DMA_COUNT_BIT)>>8);	// DMA count;
		/* DMA go, this write twice and DMA go on secondary,
		 * workaround for unknown bug~ */
		IC_WriteByte(TWIC_P3, SPI_DMA_COUNT_REG2, DMA_READ|((length & SPI_DMA_COUNT_BIT)>>8));	// DMA go;

		// Notice: FPGA Sample Code check P2_E1[2], and need write 1 clear
		/* we should what to do if on real-chip??? */
		if((IC_ReadByte(TWIC_P2, 0xE3) & 0x04)) {
			// dma interrupt mask
			if(wait_dma()) {
				rc = 1;
				goto EXIT;
			}
		} else {
			// dma interrupt unmask
			if(wait_dma_int()) {
				rc = 1;
				goto EXIT;
			}
		}
	}
EXIT:    
	if(spi_hp_mode_use)
		IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);

	return rc;
}

/**
 * @func    spi_wren
 * @brief   SPI NOR of write enable command.
 * @param   none
 * @return  0 if successfully, or 1 if some error
 */
int
spi_wren (void)
{
    int rc = 0;

    IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, WRITE_ENABLE);	// WREN command
	if(wait_wip()) {
		rc = 1;
		goto EXIT;
	}
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, WRITE_INSTRUCTION);	// Control
    
EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);

    return rc;
}

/**
 * @func    spi_rdsr_WQ
 * @brief   SPI NOR of read status register command,
 * 			this function for WQ NOR Flash.
 * @param   status		read state0 store in this point
 * 			status1		read state1 store in this point
 * @return  0 if successfully, or 1 if some error
 */
int
spi_rdsr_a (unsigned char* states,unsigned char* states1)
{
    int rc = 0;

    IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG0_GD); // RDSR command
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(wait_cmd()) {
		rc = 1;
		goto EXIT;
	}
	*states = IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value
	
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG1_GD); // RDSR command
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(wait_cmd()) {
		rc = 1;
		goto EXIT;
	}
	
	*states1 = IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value

  EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
    return rc;    
}

/**
 * @func    spi_rdsr_GD
 * @brief   SPI NOR of read status register command,
 * 			this function for GD NOR Flash.
 * @param   status		read state0 store in this point
 * 			status1		read state1 store in this point
 * 			status2		read state2 store in this point
 * @return  0 if successfully, or 1 if some error
 */
int
spi_rdsr_b (unsigned char* states,unsigned char* states1,unsigned char* states2)
{
    int rc = 0;

    IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG0_GD); // RDSR command
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(wait_cmd()) {
		rc = 1;
		goto EXIT;
	}
	*states = IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value
	
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG1_GD); // RDSR command
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(wait_cmd()) {
		rc = 1;
		goto EXIT;
	}
	*states1 = IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value
	
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG2_GD); // RDSR command
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(wait_cmd()) {
		rc = 1;
		goto EXIT;
	}
	*states2 = IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value

EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
    return rc;   
}

/**
 * @func    spi_rdsr
 * @brief   SPI NOR of read status register command,
 * 			command need ref. NOR of datasheet.
 * @param   status		read state0 store in this point
 * @return  0 if successfully, or 1 if some error
 */
int
spi_rdsr_c (unsigned char* states)
{
    int rc = 0;

    IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG); 	// RDSR command
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(wait_cmd()) {
		rc = 1;
		goto EXIT;
	}
	*states = IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value
    rc = 1;

  EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
    return rc;    
}

int spi_wrsr_a(unsigned char reg1, unsigned char reg2)
{
	int rc = 0;
	if(spi_wren()) {
		rc = 1;
        goto EXIT; 
	}
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, WS_REG);      	// WRSR command
    IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, reg2);            	// register value
    IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, reg1);              // register value
    IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, WRITE_RSR_WIN);   	// Control
	
	if(wait_wip()) {
		rc = 1;
        goto EXIT;
	}

 EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
        
    spi_toggle_byte_set();
    return rc;
}

int spi_wrsr_b(unsigned char reg1, unsigned char reg2)
{
	int rc = 0;
	if(spi_wren()) {
		rc = 1;
        goto EXIT; 
	}
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, reg2);	    	// WRSR command
	IC_WriteByte(TWIC_P3, SPI_WR_DATA_REG, reg1);				// register value
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, WRITE_RSR);	    	// Control
	
	if(wait_wip()) {
		rc = 1;
        goto EXIT;
	}

 EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
        
    spi_toggle_byte_set();
    return rc;
}

int spi_wrsr_c(unsigned char reg)
{
	int rc = 0;
	if(spi_wren()) {
		rc = 1;
        goto EXIT; 
	}
    IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, WS_REG);	    	// WRSR command
	IC_WriteByte(TWIC_P3, SPI_WR_DATA_REG, reg);			    // register value
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, WRITE_RSR);	    	// Control
	
	if(wait_wip()) {
		rc = 1;
        goto EXIT;
	}

 EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
        
    spi_toggle_byte_set();
    return rc;
}

static int 
ospi_quad_a(void)
{
	int rc = 0;
	unsigned char reg = 0;
	unsigned char reg1 = 0;
	unsigned char count = 0;
	INFO("QUAD_NOR_TYPE_A \r\n");
	spi_wrsr_a(BLOCK_PROTECT,QUAD_ENABLE_WIN);
	for(count=0; count<255; count++) {
		if(!spi_rdsr_a(&reg,&reg1)){
			if(!(reg&0x01))
				break;
		}
	}
	INFO("QUAD_NOR_STATUS r1=%x r2=%x\r\n",reg,reg1);
	
	if(!(reg1&0x02)) {
		ERROR("quad enable error! \r\n");
		rc = 1;
	}
	return rc;
}
static int 
ospi_quad_b(void)
{
	int rc = 0;
	unsigned char reg = 0;
	unsigned char reg1 = 0;
	unsigned char reg2 = 0;
	unsigned char count = 0;
	INFO("QUAD_NOR_TYPE_B \r\n");
	spi_wrsr_b(BLOCK_PROTECT,WS_REG0_GD128M);	
	spi_wrsr_b(QUAD_ENABLE_GD128,WS_REG1_GD128M);		  
	spi_wrsr_b(QUAD_STATUS_3_GD128,WS_REG2_GD128M);	  
	for(count=0; count<255; count++) {
		if(!spi_rdsr_b(&reg,&reg1,&reg2)){
			if(!(reg&0x01))
				break;
		}
	}
	INFO("QUAD_NOR_STATUS r1=%x r2=%x r3=%x\r\n",reg,reg1,reg2);
	if(!(reg1&0x02)) {
		ERROR("quad enable error! \r\n");
		rc = 1;
	}
	return rc;
}
static int 
ospi_quad_c(void)
{
	unsigned char reg = 0;
	unsigned char count = 0;
	int rc = 0;
	INFO("QUAD_NOR_TYPE_C \r\n");
	spi_wrsr_c(0x7c); 
	for(count=0; count<255; count++) {
        if(!spi_rdsr_c(&reg)){
			if(!(reg&0x01))
				break;
		}
	}
	INFO("QUAD_NOR_STATUS r1=%x \r\n",reg);
	if(!(reg&0x40)) {
		ERROR("quad enable error! \r\n");
		rc = 1;
	}
	return rc;
}


/**
 * @func    spi_read_byte
 * @brief   Read byte from SPI Flash ROM.
 * @param   unsigned long address	NOR Flash of address
 * @return  data
 * @note	just read from memory not is really SPI read
 */
unsigned char
spi_read_byte(unsigned long address)
{
    unsigned char read;
    memcpy((unsigned char*)&read, (unsigned char*)address, 1);
    return read;
}

/**
 * @func    spi_read_id
 * @brief   Read Flash ID from SPI Flash ROM.
 * @param   value		read id store in this point 
 * @return  0 if successfully, or 1 if some error
 * @note	none
 */
int
spi_read_id(int *value)
{
	int tmp = 0;
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, READ_ID);
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, 0XC8);
	if(wait_cmd()) {
        goto EXIT;
	}
	tmp |= IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1)<<0;//manufacture id
	tmp |= IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG2)<<8;//device id 0~7
	tmp |= IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG3)<<16;//device id 8~15
	*value = tmp;
	
	if((tmp & 0xff) == 0){
		IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, 0X90);
		IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG1, 0x00);	    
		IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, 0x00);
		IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, 0x00);
		IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, 0xD8);
		if(wait_cmd()) {
			goto EXIT;
		}
		tmp |= IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1)<<0;//manufacture id
		tmp |= IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG2)<<8;//device id 0~7
		tmp |= IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG3)<<16;//device id 8~15
		*value = tmp;
	}
    return 0; 
EXIT:
	return 1;
}

/**
 * @func    spi_check_id
 * @brief   Check flash type
 * @param   value		read id store in this point 
 * @return  0 if successfully, or 1 if some error
 * @note	none
 */
int
spi_check_id(unsigned short manu_id,unsigned short dev_id)
{
	int type = 0xff;
	int i =0;
	INFO("Flash Manufacture_id=%x Device_id=%x\r\n",manu_id,dev_id);
	while(ospi_id[i].manu_id != 0xff){
		if((manu_id == ospi_id[i].manu_id) && (dev_id == ospi_id[i].dev_id)){
			type = ospi_id[i].type;
			break;
		}
		i++;
	}
	
	if(type <0xff){
		switch(type){
			case OSPI_TYPE_A:
				ospi_quade_wrsr = ospi_quad_a;
				HPM_TOGGLE_VALUE = OSPI_TYPE_A_HPM_TOGGLE_VALUE;
				NONE_TOGGLE_VALUE = OSPI_TYPE_A_NON_TOGGLE_VALUE;
				break;
			case OSPI_TYPE_B:
				ospi_quade_wrsr = ospi_quad_b;
				HPM_TOGGLE_VALUE = OSPI_TYPE_B_HPM_TOGGLE_VALUE;
				NONE_TOGGLE_VALUE = OSPI_TYPE_B_NON_TOGGLE_VALUE;
				break;
			case OSPI_TYPE_C:
				ospi_quade_wrsr = ospi_quad_c;
				HPM_TOGGLE_VALUE = OSPI_TYPE_C_HPM_TOGGLE_VALUE;
				NONE_TOGGLE_VALUE = OSPI_TYPE_C_NON_TOGGLE_VALUE;
				break;
		}
	}else{
		goto EXIT;
	}
	INFO("Flash OSPI_TYPE = %x HPM_TOGGLE_VALUE = %x NON_TOGGLE_VALUE = %X\r\n",type,HPM_TOGGLE_VALUE,NONE_TOGGLE_VALUE);
    return 0; 
EXIT:
	return 1;
}

/**
 * @func    ounprotected
 * @brief   disable NOR flash of block protect.
 * @param   none
 * @return  0 if successfully, or 1 if fail
 */
int
ounprotected (void)
{
	int rc = 0;
	int i = 0;
	unsigned char state = 0;

	if(spi_wrsr_c(NO_PROTECT)) {
		rc = 1;
		goto EXIT;
	}

	rc = 1;
	for(i=0; i < 5; i++) {
		if(spi_rdsr_c(&state)) {
			if((state&0x3c) == 0)
				return 0;
		}
	}

EXIT:
	return rc;
}

/**
 * @func    spi_erase_sector
 * @brief   SPI sector erase.
 * 			command need ref. NOR of datasheet.
 * @param   unsigned long address	NOR Flash of address
 * @return  0 if successfully, or 1 if some error
 */
int
spi_erase_sector (unsigned long address)
{
	int rc = 0;

	/* WRSR */

	if(spi_wrsr_c(NO_PROTECT)) {
		rc = 1;
		goto EXIT;
	}

    if(spi_wren()) {
    	rc = 1;
        goto EXIT;
    }

	/* SE */
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, SECTOR_ERASE);

	/* NOR FLASH SECTOR ADDRESS */
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG1, address&0xFF);
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, (address>>8)&0xFF);
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, (address>>16)&0xFF);

	/* SPI SECTOR ERASE COMMAND */
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, SE_ERASE);

	if(wait_cmd()) {
		rc = 1;
        goto EXIT;
	}

EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);

    return rc;
}

/**
 * @func    spi_chip_erase
 * @brief   SPI chip erase.
 * 			command need ref. NOR of datasheet.
 * @param   none
 * @return  0 if successfully, or 1 if some error
 */
int
spi_chip_erase (void)
{
	int i = 0;

	if(spi_wren()) {
		return 1;
	}

	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, CHIP_ERASE);
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, BULK_ERASE);

	for(i=0; i < 65535; i++) {
		if(IC_ReadByte(TWIC_P3, SPI_COMMAND_REG) & 0x02)
			return wait_cmd();
		usleep(200000);
	}

	return 1;	// timeout
}



/*
 * OSPI Master Control Functions
 */

/**
 * @func    ospi_toggle_byte_set
 * @brief   set QSPI of toggle value and value must ref. NOR of datasheet.
 * @param   none
 * @return  none
 */
static void
ospi_toggle_byte_set (void)
{
    IC_WriteByte(TWIC_P3, OSPI_WR_DATA_REG, NONE_TOGGLE_VALUE);
    IC_WriteByte(TWIC_P3, OSPI_TOGGLE_WR_DATA, HPM_TOGGLE_VALUE);
}

/**
 * @func    wait_ospi_cmd
 * @brief   waiting CMD done by check register.
 * @param   none
 * @return  0 if CMD done, 1 if timeout
 */
static int __attribute__((unused))
wait_ospi_cmd (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if(!(IC_ReadByte(TWIC_P3, OSPI_COMMAND_REG)&OSPI_ISSUE_CMD))
			return 0;
		else
			twdDelay(3);
	}
	return 1;
}

/**
 * @func    wait_ospi_wip
 * @brief   waiting WIP done by check register.
 * @param   none
 * @return  0 if WIP done, 1 if timeout
 */
static int
wait_ospi_wip (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if((IC_ReadByte(TWIC_P3, OSPI_COMMAND_REG)&OSPI_CHECK_WIP))
			return 0;
		else
			twdDelay(3);
	}
	return 1;
}

/**
 * @func    ospi_wren
 * @brief   SPI NOR of write enable command.
 * @param   none
 * @return  0 if successfully, or 1 if some error
 */
int
ospi_wren (void)
{
	int rc = 0;

	IC_WriteByte(TWIC_P3, OSPI_INSTRUCTION_REG, WRITE_ENABLE);	// WREN command
	IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG, 0x82);				// Control

	if(wait_ospi_wip()) {
		rc = 1;
        goto EXIT;
	}
EXIT:
    if(spi_hp_mode_use)
        IC_WriteByte(TWIC_P3, OSPI_INSTRUCTION_REG, FAST_READ_4X);

    return rc;
}

/**
 * @func    ospi_dma_write
 * @brief   Program SPI Flash ROM via DMA moving from sLUT (256/384/HSize/HSize).
 * @param   unsigned long address	NOR Flash of address
			unsigned char cnt_mode	DMA of count (length) mode
 * @return  0 if successfully, or 1 if some error
 */
int
ospi_dma_write (unsigned char *RomData, unsigned long address)
{
	int rc = 0;
	int i= 0;

	IC_WriteByte(TWIC_P3, SOSD_EMU_CTR_REG, 0x80);		// Exit emulation
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, 0x01);		// Disable SPIOSD and update
	IC_WriteByte(TWIC_P3, OSPI_DMA_CTRL_REG1, 0x13);	// Fix 256(0x13)/384(0x17)Bytes for later using 1 page only

	/* write data to LUT port */
	for(i = 0; i < 256; i++)
		IC_WriteByte(TWIC_P3, SLUT_DATA_PORT, RomData[i]);

	IC_WriteByte(TWIC_P3, OSPI_DMA_CTRL_REG1, 0x13);	// Fix 256(0x13)/384(0x17)Bytes for later using 1 page only

	if(ospi_wren()) {
		rc = 1;
		goto EXIT;
	}

	IC_WriteByte(TWIC_P3, OSPI_INSTRUCTION_REG, PAGE_PROGRAM);
	IC_WriteByte(TWIC_P3, OSPI_ADDRESS_REG1, address&0x000000FF);
	IC_WriteByte(TWIC_P3, OSPI_ADDRESS_REG2, ((address>>8)&0x000000FF));
	IC_WriteByte(TWIC_P3, OSPI_ADDRESS_REG3, ((address>>16)&0x000000FF));
	IC_WriteByte(TWIC_P3, OSPI_WR_DATA_REG,0x33);
	IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG,0x72);
	IC_WriteByte(TWIC_P3, OSPI_DMA_CTRL_REG1,0x93);

	for(i=0; i < 255; i++) {
		if(IC_ReadByte(TWIC_P3, OSPI_DMA_CTRL_REG2) & 0x80) 				// [TWIC_P3|OSPI_DMA_CTRL_REG]&OSPI_DMA_GO
			break;
		else {
			usleep(30);
			if(i > 50)
				return 0;
		}
	}
	if(wait_ospi_wip()) {
		rc = 1;
		goto EXIT;
	}

EXIT:
	return rc;
}

/**
 * @func    spi_read_byte
 * @brief   Read byte from SPI Flash ROM.
 * @param   unsigned long address	NOR Flash of address
 * @return  data
 */
unsigned char
ospi_read_byte (unsigned long address)
{
	/* NOR FLASH READ BYTE COMMAND */
	IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, READ_DATA);
	
	/* NOR FLASH ADDRESS */
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG1, address&0xFF);
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, (address>>8)&0xFF);
	IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, (address>>16)&0xFF);
	
	/* SPI READ BYTE COMMAND */
	IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_BYTE);
	
	if(wait_cmd())
		return 0xFF;
    usleep(10);
 	return IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);
}

/**
 * @func    initial_lut
 * @brief   initial SPIOSD LUT of registers for upgrade.
 * @param   none
 * @return  none
 */
void
initial_lut (void)
{
	//init spisod
	IC_WriteByte(TWIC_P3, 0x9e, 0x00);
	IC_WriteByte(TWIC_P3, 0xd0, 0x00);
	IC_WriteByte(TWIC_P3, 0xef, 0x00);
}

/*
 * not recommend use HP mode and not yet advanced test
 */
void __attribute__((unused))
spi_hp_mode_using(void)
{
    spi_hp_mode_use = 1;
}

void __attribute__((unused))
spi_hp_mode_stop_using(void)
{
    spi_hp_mode_use = 0;
}

int __attribute__((unused))
spi_hp_mode_disable(void)
{
    int i = 0;
    IC_WriteByte(TWIC_P3, SPI_ARB_EN_I8051, 0);

    // need to check register 20121107
    // invalid i cache
	//IC_WriteByte(TWIC_P1, 0xFD, IC_ReadByte(TWIC_P1, 0xFD) | 0x80);
	//IC_WriteByte(TWIC_P1, 0xFD, IC_ReadByte(TWIC_P1, 0xFD) & 0x7F);

    IC_WriteByte(TWIC_P3, SPI_HP_REG, IC_ReadByte(TWIC_P3, SPI_HP_REG) & ~SPI_HP_EN);
    for(i = 0; i < 255; i++)
    {
        if(!(IC_ReadByte(TWIC_P3, SPI_HP_REG) & SPI_HP_EXTRA8T))
        {
                DBG_PRINT("SPI High Performance Mode Disable Success\n");
                return 1;
        }
        twdDelay(3);
    }

    DBG_PRINT("SPI High Performance Mode Disable Fail\n");
    IC_WriteByte(TWIC_P3, SPI_ARB_EN_I8051, 0xC7);
    return 0;
}

int __attribute__((unused))
spi_hp_mode_enable(void)
{
    int i = 0;

    IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
    spi_toggle_byte_set();
    ospi_toggle_byte_set();

    // workaround recommand by shawn
    IC_WriteByte(TWIC_P3, SPI_ARB_EN_I8051, 0);
    twdDelay(200);

    // need to check register 20121107
    // invalid i cache
	//IC_WriteByte(TWIC_P1, 0xFD, IC_ReadByte(TWIC_P1, 0xFD) | 0x80);
	//IC_WriteByte(TWIC_P1, 0xFD, IC_ReadByte(TWIC_P1, 0xFD) & 0x7F);

    IC_WriteByte(TWIC_P3, SPI_HP_REG, (IC_ReadByte(TWIC_P3, SPI_HP_REG) | SPI_HP_EN));

    for(i = 0; i < 255; i++)
    {
        if(IC_ReadByte(TWIC_P3, SPI_HP_REG) & SPI_HP_EXTRA8T)
        {
                DBG_PRINT("SPI High Performance Mode Enable Success\n");
                return 1;
        }
        twdDelay(3);
    }

    DBG_PRINT("SPI High Performance Mode Enable Fail\n");
    IC_WriteByte(TWIC_P3, SPI_ARB_EN_I8051, 0xC7);
    return 0;
}
