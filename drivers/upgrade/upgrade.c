/**
 *  @file   sd_upgrade.c
 *  @brief  SD card upgrade function
 *  $Id: upgrade.c,v 1.26 2016/07/06 03:31:13 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.26 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/09/04  hugo    New file.
 *
 */

#include <config.h>
#include <stdarg.h>
#include "upgrade.h"
#include <drivers/spirw/spirw.h>
#include <codec_eng/ringbell.h>
#include <fat32.h>
#include "reg_tw.h"
#include "default_config.h"
         
#define REG_SCALAR_BASE 0xb0400000
#define REG_OSD_INDEX   (REG_SCALAR_BASE | (0xa0 << 4))
#define REG_OSD_DATA    (REG_SCALAR_BASE | (0xa1 << 4))
#define REG_OSD_ORAM_AL (REG_SCALAR_BASE | (0xa2 << 4))
#define REG_OSD_ORAM_AH (REG_SCALAR_BASE | (0xa3 << 4))
#define REG_OSD_RAM     (REG_SCALAR_BASE | (0xa4 << 4))

struct upgrade_ctx _ctx;
//extern unsigned int _fr1, _r1_size, _fr2, _r2_size, _fr3, _r3_size;
extern int ugprintf16(const char *fmt, ...);
extern unsigned int _fringtong;
extern char time_str[32];
extern void settime2nor(void);

__text__
void _putb(const char c)
{
    int i;

    for (i = 0;  i < 100000; i++) {
        if (readb(0xb2000014) & 0x40) /* until Trasnsmitter empty */
            break;
    }

    writeb(c, 0xb2000000);
}

__text__
void _putstring(const char *c)
{
	int i=0;
	while(c[i]!=0)
	{
		_putb(c[i]);
		if(c[i]=='\n')
			_putb('\r');
		i++;
	}
}

__text__
void _twdDelay(unsigned int wLoops) 
{
	unsigned int count = 0;
	
	while(count < wLoops*100) {
		count++;
	}
}

__text__
unsigned char _IC_ReadByte(unsigned short bPage, unsigned char bAdd)
{
    unsigned char ret  = 0;
    unsigned long addr = (unsigned long)bAdd;
    unsigned long page = (unsigned long)bPage;    
	unsigned long Address = REG_SCALAR_BASE + page + (addr << 4);

#if 0
	ret = readb(Address);
#else
	ret = (unsigned char)(*(volatile unsigned int *)(Address));
#endif
    return ret;
}

__text__
void _IC_WriteByte(unsigned short bPage, unsigned char bAdd, unsigned char bData)
{
    unsigned long addr = (unsigned long)bAdd;
    unsigned long page = (unsigned long)bPage;    
    unsigned long data = (unsigned long)bData;
	unsigned long Address = REG_SCALAR_BASE + page + (addr << 4);

#if 0
	writel(data, Address);
#else
	(*(volatile unsigned int *)(Address)) = data;
    _IC_ReadByte(bPage, bAdd);
    _twdDelay(10);
#endif
}

#define UPGRADE_MSG  0
#define FINISHED_MSG 1

#define OSD2_CFG_IDX_PORT 0xA8
#define OSD2_CFG_DAT_PORT 0xA9

#define OSD2_RAM_IDX_PORT 0xAA
#define OSD2_RAM_DAT_PORT 0xAB

__text__
void _draw_upgrading(int step)
{
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);   
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'U'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'P'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'G'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'R'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'A'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'D'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'I'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'N'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'G'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, '.'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	if(step > 0) {
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, '.'-10); 
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);	
	}
	else {
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00);
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);
		
	}
	if(step > 1) {
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, '.'-10); 
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);	
	}
	else {
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00); 
		_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);
	}
}

__text__
void _update_rom_message(int count)
{
	count = count / 50;

	_IC_WriteByte(TWIC_P0, OSD2_RAM_IDX_PORT, 0x68);	
	_IC_WriteByte(TWIC_P0, OSD2_RAM_IDX_PORT, 0x1A);
	
	_draw_upgrading(count%3);
}

__text__
void _finish_rom_message()
{	
	_IC_WriteByte(TWIC_P0, OSD2_RAM_IDX_PORT, 0x68);	 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_IDX_PORT, 0x1A);
	
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);   
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'F'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'I'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'N'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'I'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'S'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'H'-10);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'E'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 'D'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, '.'-10); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x44);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x00); 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, 0x42);
}

__text__
void _show_init_rom_message()
{
	unsigned char OSD2_REG_VALS[] = {                                                                                                                                                                                       
	    0x01, //more left 16x24                                                                                          
	    0x78,                                                                                                            
	    0x02, //2bp start at idx 1*2                                                                                     
	    0x40,                                                                                                            
	    0x03, //4bp start at idx 2*2                                                                                     
	    0x40,                                                                                                            
	    0x04, //2bp start address: 0030h                                                                                 
	    0x30,                                                                                                            
	    0x05,                                                                                                            
	    0x00,                                                                                                            
	    0x06, //4bp start address: 0090h                                                                                 
	    0x90,                                                                                                            
	    0x07,                                                                                                            
	    0x00,                                                                                                            
	    0x0c, //1bp FG high bit: 0000h                                                                                   
	    0x00,                                                                                                            
	    0x0d, //ROM font start at 40h*2                                                                                  
	    0x00,                                                                                                            
	    0x11, //menu1 start addr                                                                                         
	    0x50,                                                                                                            
	    0x12,                                                                                                            
	    0x1a,                                                                                                            
	    0x13,  //menu1 end addr                                                                                          
	    0x8D,                                                                                                            
	    0x14,                                                                                                            
	    0x1a,                                                                                                            
	    0x0b,  //0b  alpha blending via RGBA = 1                                                                         
	    0x40,                                                                                                            
	    0x0e,  //0e  ChromaKey100h = 0                                                                                   
	    0x00,                                                                                                            
	    0x50,  //8bp idx                                                                                                 
	    0x40,                                                                                                            
	    0x51,  //8bp addr                                                                                                
	    0x50,                                                                                                            
	    0x52,                                                                                                            
	    0x01,
	    0xFF,
	};                                                                                                              
	unsigned char OSD2_LUT_VALS[] = {                                                                                
	    0x00,0x00,0x00,0x00,                                                                                                   
	    0x00,0xFF,0xFF,0xFF,                                                                                                   
	    0x00,0xFF,0x00,0x00,                                                                                                 
	};                                                                                                              
	unsigned char OSD2_BGLUT_VALS[] = {                                                                                                
	    0x00,0x01,0x02,0x03,0x04,0x05                                                                                                                        
	};
	unsigned char OSD2_INITIAL_MENU_VALS[] = {                                                                                                               
	    0x32,    /* first blank row */                                                                                         
	    0x40,                                                                                                            
	    0x82,                                                                                                            
	    0x60,                                                                                                            
	    0x00,                                                                                                            
	    0xd0,                                                                                                            
	    0x10,                                                                                                            
	    0x00,  
	    0x00,0x42,0x01,0x42,0x02,0x42,0x03,0x42,0x04,0x42,0x05,0x42,0x06,0x42,0x07,0x42,
	    0x00,0x42,0x01,0x42,0x02,0x42,0x03,0x42,0x04,0x42,0x05,0x42,0x06,0x42,0x07,0x42,
	    
	    0x32,        /* message */                                                                                          
	    0xe0,                                                                                                            
	    0x82,                                                                                                            
	    0x60,                                                                                                            
	    0x00,                                                                                                            
	    0xd0,                                                                                                            
	    0x10,                                                                                                            
	    0x00,  
	    0x00,   0x42,   0x00, 0x42, 'U'-10, 0x44, 'P'-10, 0x44,
	    'G'-10, 0x44, 'R'-10, 0x44, 'A'-10, 0x44, 'D'-10, 0x44,
	    'I'-10, 0x44, 'N'-10, 0x44, 'G'-10, 0x44, '.'-10, 0x44, 
	    0x00,   0x42,   0x00, 0x42,   0x00, 0x42,   0x00, 0x42,
	 
	    0x32,     /* last blank row */                                                                                          
	    0xe0,                                                                                                            
	    0x82,                                                                                                            
	    0x60,                                                                                                            
	    0x00,                                                                                                            
	    0xd0,                                                                                                            
	    0x10,                                                                                                            
	    0x00,  
	    0x00,0x42,0x01,0x42,0x02,0x42,0x03,0x42,0x04,0x42,0x05,0x42,0x06,0x42,0x07,0x42,
	    0x00,0x42,0x01,0x42,0x02,0x42,0x03,0x42,0x04,0x42,0x05,0x42,0x06,0x42,0x07,0x42, 
	    0xff
	}; 
	
	int i;
	
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x00);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, 0x00);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x10);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, 0x00);	 
	
	for(i=0; OSD2_REG_VALS[i]!=0xFF; i+=2) {
		_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, OSD2_REG_VALS[i]);	    
		_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, OSD2_REG_VALS[i+1]);
	}
	
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x08);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, 0x80);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x09);	 
	for(i=0; i<12; i++) { 
		_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, OSD2_LUT_VALS[i]);	 	
	}
	
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x08);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, 0x98);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x09);	 
	for(i=0; i<6; i++) { 
		_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, OSD2_BGLUT_VALS[i]);	 	
	}

	_IC_WriteByte(TWIC_P0, OSD2_RAM_IDX_PORT, 0x50);	 
	_IC_WriteByte(TWIC_P0, OSD2_RAM_IDX_PORT, 0x1A);	 
	for(i=0; OSD2_INITIAL_MENU_VALS[i] != 0xFF; i++) { 
	    _IC_WriteByte(TWIC_P0, OSD2_RAM_DAT_PORT, OSD2_INITIAL_MENU_VALS[i]);
	}	
	
	_twdDelay(1000);
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x00);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, 0x86);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_IDX_PORT, 0x10);	 
	_IC_WriteByte(TWIC_P0, OSD2_CFG_DAT_PORT, 0x80);	 
}


__text__
static int
_wait_wip (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if((_IC_ReadByte(TWIC_P3, SPI_COMMAND_REG)&SPI_CHECK_WIP))
			return 0;
		else
			_twdDelay(3);
	}
	return 1;
}

__text__
int
_spi_wren (void)
{
    int rc = 0;

    _IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, WRITE_ENABLE);	// WREN command
	if(_wait_wip()) {
		rc = 1;
		goto EXIT;
	}
	_IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, WRITE_INSTRUCTION);	// Control
    
EXIT:
//    if(spi_hp_mode_use)
//        _IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);

    return rc;
}

__text__
static void
_spi_toggle_byte_set (void)
{   
    _IC_WriteByte(TWIC_P3, SPI_WR_DATA_REG, NONE_TOGGLE_VALUE);
    _IC_WriteByte(TWIC_P3, SPI_TOGGLE_WR_DATA, HPM_TOGGLE_VALUE);
}

__text__
int _spi_wrsr_c(unsigned char reg)
{
	int rc = 0;
	if(_spi_wren()) {
		rc = 1;
        goto EXIT; 
	}
    _IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, WS_REG);	    	// WRSR command
	_IC_WriteByte(TWIC_P3, SPI_WR_DATA_REG, reg);			    // register value
	_IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, WRITE_RSR);	    	// Control
	
	if(_wait_wip()) {
		rc = 1;
        goto EXIT;
	}

 EXIT:
//    if(spi_hp_mode_use)
//        _IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
        
    _spi_toggle_byte_set();
    return rc;
}

__text__
static int
_wait_cmd (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if(!(_IC_ReadByte(TWIC_P3, SPI_COMMAND_REG)&SPI_ISSUE_CMD))
			return 0;
		else
			_twdDelay(30);
		_update_rom_message(i);
	}
	return 1;
}

__text__
int
_spi_rdsr_c (unsigned char* states)
{
    int rc = 0;

    _IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, RS_REG); 	// RDSR command
	_IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_RSR);		// Control
	if(_wait_cmd())
		goto EXIT;
	*states = _IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);	// register value
    rc = 1;

  EXIT:
//    if(spi_hp_mode_use)
//        _IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, FAST_READ_4X);
    return rc;    
}

__text__
int
_ounprotected (void)
{
	int rc = 0;
	int i = 0;
	unsigned char state = 0;

	if(_spi_wrsr_c(NO_PROTECT)) {
		rc = 1;
		goto EXIT;
	}

	rc = 1;
	for(i=0; i < 5; i++) {
		if(_spi_rdsr_c(&state)) {
			if((state&0x3c) == 0)
				return 0;
		}
	}

EXIT:
	return rc;
}

__text__
int
_spi_chip_erase (void)
{
	int i = 0;

	if(_spi_wren()) {
		return 1;
	}

	_IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, CHIP_ERASE);
	_IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, BULK_ERASE);

	for(i=0; i < 65535; i++) {
		if(_IC_ReadByte(TWIC_P3, SPI_COMMAND_REG) & 0x02)
			return _wait_cmd();
		_twdDelay(100000);
		_update_rom_message(i*10);
		_twdDelay(100000);
		_update_rom_message(i*10);
	}

	return 1;	// timeout
}

__text__
static int
_wait_ospi_wip (void)
{
	unsigned short i = 0;
	for(i = 0; i < 65535  ; i++) {
		if((_IC_ReadByte(TWIC_P3, OSPI_COMMAND_REG)&OSPI_CHECK_WIP))
			return 0;
		else
			_twdDelay(3);
	}
	return 1;
}
__text__
int
_ospi_wren (void)
{
	int rc = 0;

	_IC_WriteByte(TWIC_P3, OSPI_INSTRUCTION_REG, WRITE_ENABLE);	// WREN command
	_IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG, 0x82);				// Control

	if(_wait_ospi_wip()) {
		rc = 1;
        goto EXIT;
	}
EXIT:
//    if(spi_hp_mode_use)
//        _IC_WriteByte(TWIC_P3, OSPI_INSTRUCTION_REG, FAST_READ_4X);

    return rc;
}


__text__
int
_ospi_dma_write (unsigned char *RomData, unsigned long address)
{
	int rc = 0;
	int i= 0;
	
	_IC_WriteByte(TWIC_P3, SOSD_EMU_CTR_REG, 0x80);		// Exit emulation
	_IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, 0x01);		// Disable SPIOSD and update
	_IC_WriteByte(TWIC_P3, OSPI_DMA_CTRL_REG1, 0x13);	// Fix 256(0x13)/384(0x17)Bytes for later using 1 page only

	/* write data to LUT port */
	for(i = 0; i < 256; i++)
		_IC_WriteByte(TWIC_P3, SLUT_DATA_PORT, RomData[i]);

	_IC_WriteByte(TWIC_P3, OSPI_DMA_CTRL_REG1, 0x13);	// Fix 256(0x13)/384(0x17)Bytes for later using 1 page only

	if(_ospi_wren()) {
		rc = 1;
		goto EXIT;
	}

	_IC_WriteByte(TWIC_P3, OSPI_INSTRUCTION_REG, PAGE_PROGRAM);
	_IC_WriteByte(TWIC_P3, OSPI_ADDRESS_REG1, address&0x000000FF);
	_IC_WriteByte(TWIC_P3, OSPI_ADDRESS_REG2, ((address>>8)&0x000000FF));
	_IC_WriteByte(TWIC_P3, OSPI_ADDRESS_REG3, ((address>>16)&0x000000FF));
	_IC_WriteByte(TWIC_P3, OSPI_WR_DATA_REG,0x33);
	_IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG,0x72);
	_IC_WriteByte(TWIC_P3, OSPI_DMA_CTRL_REG1,0x93);

	for(i=0; i < 255; i++) {
		if(_IC_ReadByte(TWIC_P3, OSPI_DMA_CTRL_REG2) & 0x80) 				// [TWIC_P3|OSPI_DMA_CTRL_REG]&OSPI_DMA_GO
			break;
		else {
			_twdDelay(30);
			if(i > 50)
				return 0;
		}
	}
	if(_wait_ospi_wip()) {
		rc = 1;
		goto EXIT;
	}

EXIT:
	return rc;
}

__text__
void
_initial_lut (void)
{
	//init spisod
	_IC_WriteByte(TWIC_P3, 0x9e, 0x00);
	_IC_WriteByte(TWIC_P3, 0xd0, 0x00);
	_IC_WriteByte(TWIC_P3, 0xef, 0x00);
}

__text__
unsigned char
_ospi_read_byte (unsigned long address)
{
	/* NOR FLASH READ BYTE COMMAND */
	_IC_WriteByte(TWIC_P3, SPI_INSTRUCTION_REG, READ_DATA);
	
    /* NOR FLASH ADDRESS */
    _IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG1, address&0xFF);
    _IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG2, (address>>8)&0xFF);
    _IC_WriteByte(TWIC_P3, SPI_ADDRESS_REG3, (address>>16)&0xFF);
    
    /* SPI READ BYTE COMMAND */
    _IC_WriteByte(TWIC_P3, SPI_COMMAND_REG, READ_BYTE);
    
    if(_wait_cmd())
        return 0xFF;
 
     return _IC_ReadByte(TWIC_P3, SPI_RD_BACK_DATA_REG1);
}


#define readb(addr)		(*(volatile unsigned char *)(addr))
#define readw(addr)		(*(volatile unsigned short *)(addr))
#define readl(addr)		(*(volatile unsigned int *)(addr))

#define writeb(b,addr)		((*(volatile unsigned char *)(addr)) = (b))
#define writew(b,addr)		((*(volatile unsigned short *)(addr)) = (b))
#define writel(b,addr)		((*(volatile unsigned int *)(addr)) = (b))

#define	EIO		 5	/* I/O error */

#define NOR_CTRL_BASE               0xBD800000
#define NOR_RD_PORT                 (NOR_CTRL_BASE + 0x00) 
#define NOR_WR_PORT                 (NOR_CTRL_BASE + 0x04)
#define IP_STATUS                   (NOR_CTRL_BASE + 0x10)
#define NOR_GENERAL_CTRL            (NOR_CTRL_BASE + 0x14)
#define SPI_CMD                     (NOR_CTRL_BASE + 0x20)

#define OPC_WREN            (0x06 << 24)
#define SPI_CMD_TYPE_RD     (0 << 22)
#define AUTO_RDSR_DIS       (0 << 21)
#define ADDR_CYCLE_0        (0 << 18)
#define CMD_SPI_WREN        (OPC_WREN | SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0)

#define OPC_RDSR            (0x05 << 24)
#define SPI_CMD_TYPE_RD     (0 << 22)
#define AUTO_RDSR_DIS       (0 << 21)
#define CMD_SPI_RDSR        (OPC_RDSR|SPI_CMD_TYPE_RD | AUTO_RDSR_DIS | ADDR_CYCLE_0 | 1)


#define RESET_WRITE_POINT   0x100

__text__
int _nor_wren(void)
{
	uint32_t ti;
	//Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;
		_twdDelay(100);
    }
	//Write Enable
    writel(CMD_SPI_WREN, SPI_CMD);
	//Wait until IP_Status[1] = 1
    for (ti = 0; ti < 90000; ti++){
        if(readl(IP_STATUS) & (1<<1))
            break;
		_twdDelay(100);
    }
	if(ti>=80000)
		goto EXIT;
	return 0;
EXIT:
	return -EIO;
}

__text__
static __mips32__
void _a_spi_cmd(void)
{
    __asm__ __volatile__ (
      ".align 4 ;"\
      "li       $8, 0x1400001; "\
      "li       $9, 0xbd800020;"\
      "li       $10, 0x400000;" \
      "nop; nop; nop;" \
      "sw       $8, 0($9); "\
      "1: "\
      "sub      $10, 1; "\
      "bnez     $10, 1b;" \
      );
}

__text__
int _nor_wrsr(unsigned int value)
{
	uint32_t ti;
	/*write enable*/
	_nor_wren();
	/*reset data point*/
	writel(RESET_WRITE_POINT, NOR_GENERAL_CTRL);
	/*write status*/
	writel(value, NOR_WR_PORT);
	for (ti = 0; ti < 1000; ti++) {
        if (readl(IP_STATUS) & (1 << 1))
            break;
		_twdDelay(1000);
    }
	if(ti>900)
		goto EXIT;
	/*execute command*/
	_a_spi_cmd();

	return 0;
EXIT:
	return -EIO;
}

__text__
int _nor_rdsr(int *state)
{
	uint32_t ti;
	writel(CMD_SPI_RDSR, SPI_CMD);
	for (ti = 0; ti < 90000; ti++){
        if(readl(IP_STATUS) & (1<<1))
            break;
		_twdDelay(1000);
    }
	if(ti>80000)
		goto EXIT;
	*state=readl(NOR_RD_PORT);
	return 0;
EXIT:
	return -EIO;
}	

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
};

__text__
void _nor_writeprotect(struct norc_t *norc, char enable)
{
    if (enable)
        norc->wp_cnt++;
    else
        norc->wp_cnt--;

    if (norc->wp_cnt == 1)
        _nor_wrsr (0x3C);
    else if (norc->wp_cnt == 0)
        _nor_wrsr (0x00);
}

__text__
int _nor_is_writeprotect(struct norc_t *norc)
{
    unsigned int status;

//    if (!_nor_rdsr(&status))
//        return 0;
    _nor_rdsr(&status);
    
    return ((status & 0x1C) == 0x1C);
}


__text__
static __inline__ void
___cli(void) 
{
    __asm__ __volatile__ (
        ".set push;"
        ".set noat;"
        ".set nomips16;"
        "mfc0 $1, $12;"
        "ori  $1, 1;"
        "xori $1, 1;"
        ".set noreorder;"
        "mtc0 $1, $12;"
        "nop;nop;nop;"
        ".set pop;"
    );
}


__text__
void
*_memcpy (void *_dst, unsigned long _src, int n)
{
	int i;
	char *dst = (char *) _dst;
	char *src = (char *) _src;

	if ((unsigned int) dst < (unsigned int) src) {
        if (((unsigned int)dst & 0x03) == 0 && ((unsigned int)src & 0x03) == 0 && n >= 16) {
            long *ld = (long *)dst;
            long *ls = (long *)src;
            for (; n >= 4; n-=4) {
                *ld++ = *ls++;
            }    
            dst = (char *)(ld);
            src = (char *)(ls);
        }

		for (i = 0; i < n; i++)
			dst[i] = src[i];
	} else
	if ((unsigned int) dst > (unsigned int) src) {
        long *ld = (long *)(dst+n-4);
        long *ls = (long *)(src+n-4);

        if (((unsigned int)ld & 0x03) == 0 && ((unsigned int)ls & 0x03) == 0 && n >= 16) {
            for (; n >= 4; n-=4) {
                *ld-- = *ls--;
            }    
        }

		for (i = n - 1; i >= 0; i--)
			dst[i] = src[i];
	}

	return _dst;
}


#define REG_RTC_BASE            (0xB0800000)
#define REG_RTC_INFOR_1         (REG_RTC_BASE + 0x10c) 
#define REG_UPGRADE_STATUS      (REG_RTC_BASE + 0x11c)

#define INFO_DFU_EN             0
#define INFO_SPI_EN             1
#define INFO_RING_EN            2
#define INFO_DFU_DONE           3
#define INFO_SPI_DONE           4
#define INFO_RING_DONE          5
#define INFO_DFU_FAIL           6
#define INFO_SPI_FAIL           7
#define INFO_RING_FAIL          8

#define BIT_ON  1

__text__
void init_upgrade_status()
{
   writel(0,REG_UPGRADE_STATUS);
}

__text__
void set_upgrade_status(unsigned char type, unsigned char value)
{
    //unsigned long sta=0;
    switch(type) {
        case INFO_DFU_EN:      writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_DFU_EN))   , REG_UPGRADE_STATUS);    break;
        case INFO_SPI_EN:      writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_SPI_EN))   , REG_UPGRADE_STATUS);    break;
        case INFO_RING_EN:     writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_RING_EN))  , REG_UPGRADE_STATUS);    break;
        case INFO_DFU_DONE:    writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_DFU_DONE)) , REG_UPGRADE_STATUS);    break;
        case INFO_SPI_DONE:    writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_SPI_DONE)) , REG_UPGRADE_STATUS);    break;
        case INFO_RING_DONE:   writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_RING_DONE)), REG_UPGRADE_STATUS);    break;
        case INFO_DFU_FAIL:    writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_DFU_FAIL)) , REG_UPGRADE_STATUS);    break;
        case INFO_SPI_FAIL:    writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_SPI_FAIL)) , REG_UPGRADE_STATUS);    break;
        case INFO_RING_FAIL:   writel( (readl(REG_UPGRADE_STATUS)|(value<<INFO_RING_FAIL)), REG_UPGRADE_STATUS);    break;
    }        
    //sta = readl(REG_UPGRADE_STATUS);
    //printf("sta =%08X\n",(unsigned int)sta);     
}


__text__
__mips32__
void _sys_reset(void)
{
#if 0
    __asm__ __volatile__("di");
#else
    ___cli();
#endif

    writel(0x04, SYSMGM_OP);
    __asm__ __volatile__ (
        "1: b 1b;" 
    );
}




#define addrs(idx)  (unsigned int)((&_fringtong) + idx * config_ringtong_unitsize/4)

__text__
void _upgrade_ringtone(int r_index)
{
    int i, j,k;
#if 1
    int bFirst = 1;
#endif
    unsigned int addr = addrs(r_index);
    unsigned char buf[SECT_SIZE];    
    unsigned char wchecksum_buf[SECT_SIZE];
    unsigned char rchecksum_buf[SECT_SIZE];
    int *int_pb = (int *)buf;
    int block_index=0;
    int re_j,re_addr=0;
    unsigned char error_cnt=0,re_sta=0;

    /* begin... */
    _putstring("Ring update start\n");   
    
    set_upgrade_status(INFO_RING_EN,BIT_ON);
    for (i = 0; i < _ctx.fd.region_cnt; i++) {
        for (j = _ctx.fd.region[i].start; j < _ctx.fd.region[i].end; j++,block_index++) {
            if (addr % NOR_BLOCK_SIZE == 0) {
                _nor_erase (addr);
                block_index=0;
                re_addr = addr;
            }  
            _sdc_read_sector (j, buf);            
            
#if 1
            /* put header to the audio */
            if(bFirst) {
                if(int_pb[0] !=0x21872187) {   //ring not have header
                   
                    //printf("buf[0]= %08X\n",int_pb[0]);
                    int_pb[0] = 0x21872187;
                    int_pb[1] = _ctx.fd.size;
                    int_pb[2] = 0x00000004;
                    int_pb[3] = 0x00000000;             
                    bFirst = 0;
                }
            }
#endif
            _nor_write_sector (addr, buf);  
            //checksum             
            //if(addr == 0)    _putstring("Checksum...\n");
            for(k=0;k<512;k++)    wchecksum_buf[k] = buf[k];        

            _memcpy(rchecksum_buf, addr| 0x80000000, 512);                
            for(k=0;k<512;k++) {                 
                if(wchecksum_buf[k] != rchecksum_buf[k])  {                      
                    _putstring("Checksum error\n");    
                    re_sta=1; 
                    error_cnt++;
                }                
            }                
_RE_W:      
             //re write this block data
            if(re_sta ==1)
            {
                _nor_erase ( re_addr );
                for( re_j = j-block_index ; re_j< j+1 ;re_j++)   
                {   
                    _sdc_read_sector (re_j, buf);   
                    _nor_write_sector (re_addr, buf);
                    for(k=0;k<512;k++)  wchecksum_buf[k] = buf[k];
				
                   
                    _memcpy(rchecksum_buf, re_addr| 0x80000000, 512);                         
                    for(k=0;k<512;k++) 
                    {                 
                        if(wchecksum_buf[k] != rchecksum_buf[k]) {   
                            if(error_cnt >=3) {
                                _putstring("Ring update fail\n");
                                set_upgrade_status(INFO_RING_FAIL,BIT_ON);                                
                                return;
                            }
                            _putstring("Checksum error\n");
                            error_cnt++;
                            re_sta=1;
                            goto _RE_W;
                        }
                        else re_sta =0;
                    }  
                    re_addr+=SECT_SIZE;
                    _update_rom_message(re_j);
                }           
            }             
            addr += SECT_SIZE;
            _update_rom_message(j);       
        }
    }
  
    _putstring("Ring update finish\n");
    set_upgrade_status(INFO_RING_DONE,BIT_ON);   
    //while (1);
}

__text__
void _upgrade (void)
{
    int i, j, k, addr = 0;    
    unsigned char buf[SECT_SIZE];
    unsigned char wchecksum_buf[SECT_SIZE];
    unsigned char rchecksum_buf[SECT_SIZE];
    int block_index=0;
    int re_j,re_addr=0;
    unsigned char error_cnt=0,re_sta=0;
   
    /* begin... */
    _putstring("Dfu update start\n");
    set_upgrade_status(INFO_DFU_EN,BIT_ON);
    for (i = 0; i < _ctx.fd.region_cnt; i++) {
        for (j = _ctx.fd.region[i].start; j < _ctx.fd.region[i].end; j++,block_index++) {     
            if (addr % NOR_BLOCK_SIZE == 0) {
                _nor_erase (addr);
                block_index=0;
                re_addr = addr;              
            }           
            
            _sdc_read_sector (j, buf);                     
            _nor_write_sector (addr, buf);
        
            //checksum         
            //if(addr == 0)    _putstring("Checksum...\n");
            for(k=0;k<512;k++)    wchecksum_buf[k] = buf[k];
       
            //printf("addr = %p\n", addr);           
            _memcpy(rchecksum_buf, addr| 0x80000000, 512);                
            for(k=0;k<512;k++) {                 
                if(wchecksum_buf[k] != rchecksum_buf[k])  {                      
                    _putstring("Checksum error\n");    
                    re_sta=1; 
                    error_cnt++;
                }                
            }                 
_RE_W:      
            //re write this block data
            if(re_sta ==1)
            {
                _nor_erase ( re_addr );
                for( re_j = j-block_index ; re_j< j+1 ;re_j++)   
                {   
                    _sdc_read_sector (re_j, buf);   
                    _nor_write_sector (re_addr, buf);
                    for(k=0;k<512;k++)   wchecksum_buf[k] = buf[k];
                     
                    _memcpy(rchecksum_buf, re_addr| 0x80000000, 512);                    
                    for(k=0;k<512;k++) 
                    {                 
                        if(wchecksum_buf[k] != rchecksum_buf[k])  {   
                            if(error_cnt >=3) {
                                _putstring("Dfu update fail\n");
                                set_upgrade_status(INFO_DFU_FAIL,BIT_ON);                                
                                return;
                            }
                            _putstring("Checksum error\n");
                            error_cnt++;
                            re_sta=1;
                            goto _RE_W;
                        }
                        else re_sta =0;
                    }  
                    re_addr+=SECT_SIZE;
                    _update_rom_message(re_j);
                }           
            }   
            addr += SECT_SIZE;
            _update_rom_message(j);             
        }        
    }     
    _putstring("Dfu update finish\n");  
    set_upgrade_status(INFO_DFU_DONE,BIT_ON);     
}

//64k byte
__text__
void _upgrade_resource (void)
{
    int i, j, k, address = 0;
    unsigned char buf[SECT_SIZE];
    unsigned char wchecksum=0;
    unsigned char rchecksum=0;

    _putstring("Resource update start\n");
    set_upgrade_status(INFO_SPI_EN,BIT_ON);
    //initial lut
    _initial_lut();
    /*unprotect*/
    if(_ounprotected()) {
        _putstring("ounprotected fail\n");
        goto EXIT;
    }
    /* first... chip erase*/
    _putstring("chip_erase\n");
    if(_spi_chip_erase())
        goto EXIT;
    
    /* second ... update*/
    _putstring("write data\n");
    address = 0;
    for (i = 0; i < _ctx.fd.region_cnt; i++) {
        for (j = _ctx.fd.region[i].start; j < _ctx.fd.region[i].end; j++) {
            _sdc_read_sector (j, buf);
            for(k=0;k<2;k++)
            {
                if(!_ospi_dma_write((buf+256*k),address))
                {
                    address+=256;
                }
                else
                {
                    goto EXIT;
                }
            }
            wchecksum=wchecksum+buf[0]+buf[255]+buf[256]+buf[511];
            _update_rom_message(j);
        }
    }
    /*third... checksum*/
    _putstring("checksum...\n");
    address = 0;
    for (i = 0; i < _ctx.fd.region_cnt; i++) {
        for (j = _ctx.fd.region[i].start; j < _ctx.fd.region[i].end; j++) {
            rchecksum+=_ospi_read_byte(address);
            rchecksum+=_ospi_read_byte(address+255);
            rchecksum+=_ospi_read_byte(address+256);
            rchecksum+=_ospi_read_byte(address+511);
            address+=512;
            _update_rom_message(j);
        }
    }
    
    /*Write Nor flash Protect*/
    _spi_wrsr_c(0x3C);
    _putstring("Resource nor protect\n");
    
    if(wchecksum == rchecksum) {
        _putstring("Resource update finish\n");
        set_upgrade_status(INFO_SPI_DONE,BIT_ON);
    }
    else{
        _putstring("Resource update fail\n");
        set_upgrade_status(INFO_SPI_FAIL,BIT_ON);
    }
    return;
EXIT:
    _putstring("Resource update fail\n");
}

__text__
void upgrade_dfu (void)
{
	_putstring("Upgrade dfu\n");
	_upgrade();
	
}

__text__
void upgrade_spi(void)
{
    _putstring("Upgrade spi\n");
    _upgrade_resource();
}

__text__
void upgrade_ring(int r_index)
{
    _putstring("Upgrade ring\n");
    _upgrade_ringtone(r_index);
}


__text__
char check_ringtone_layout_size(unsigned int ring_size)
{
    extern unsigned int _ringtong_unitsize;
    unsigned int config_ring_size = ((unsigned int)&_ringtong_unitsize);    
    //printf("config_ringtone_unitsize=%u\n",(unsigned int)config_ring_size);
    //printf("ring_size = %u\n",ring_size);  
    if(config_ring_size >= ring_size)
        return 1;
    else {
        _putstring("Ringtong size can not more than config_ringtong_unitsize!!!\n");
        return 0;
    }
}

__text__
void _set_time_stamp_mark()
{
	writel(_ctx.time_stamp_mark, REG_RTC_INFOR_1);
}

__text__
void upgrade_sequence(void)
{
    struct fat_t *fat = gSYSC.pfat;
    int *orders = _ctx.upgrade_orders;
    struct fd32 *fds = (struct fd32 *)_ctx.upgrade_fds;
    int i = -1, j, pro_count = 0;

    void (*FUNC_TBL[]) (void) = {	
            upgrade_dfu,
            upgrade_spi
        }; 
    init_upgrade_status();
    _set_time_stamp_mark();
    _show_init_rom_message();
    _putstring("Sequential upgrade start. \n");

	while(orders[++i] != -1 && pro_count++ < 16) {	
		if(orders[i]==  5) {  		//defint clean =5
			_putstring("Erase nor flash\n");
			_chip_erase();
			break;
        }
	}
	i = -1, pro_count = 0;
	while(orders[++i] != -1 && pro_count++ < 16) {         
		_ctx.fd.region_cnt = fds[i].region_cnt;   
		for (j = 0; j < fds[i].region_cnt; j++) {
			_ctx.fd.region[j].start = fat->root_start +
				(fds[i].region[j].start - FIRST_ROOTCLUSTER) * fat->sect_per_clus;
			_ctx.fd.region[j].end = _ctx.fd.region[j].start +
				fds[i].region[j].count * fat->sect_per_clus;
			_ctx.fd.size = fds[i].size;         
		}	
		if(orders[i]<2 ) 	
			FUNC_TBL[orders[i]]();
		else if(orders[i] < config_ringtong_num + 2 &&orders[i] !=5) {	
            if( check_ringtone_layout_size(fds[i].size) )   
                upgrade_ring(orders[i]-2);
          }
	 }
			
		
    _nor_wrsr (0x3C); //enable write protect
    //settime2nor();//nor flash size is not same
	//_putstring("time set done.\n");
    _finish_rom_message();
    _putstring("set nor-flash write protect.\n");
    _putstring("All component upgrade finished. Now restart system...\n");
    
    _twdDelay(5000000);
    _sys_reset();
}

#define AFC_UART_BASE       0xb2400000
#define UART_BASE	        0xb2000000
#define UART_RBR(x)         ((x) + 0x00)
#define UART_THR(x)         ((x) + 0x00)
#define UART_DLL(x)	        ((x) + 0x00)
#define UART_DLH(x)         ((x) + 0x04)
#define UART_IER(x)		    ((x) + 0x04)
#define UART_IIR(x)         ((x) + 0x08)
#define UART_FCR(x)         ((x) + 0x08)
#define UART_LCR(x)			((x) + 0x0C)
#define UART_MCR(x)			((x) + 0x10)
#define UART_LSR(x)         ((x) + 0x14)
#define UART_MSR(x)         ((x) + 0x18)
#define UART_USR(x)			((x) + 0x7C)

#define MAX_PACKAGE_LEN 1536
//#define PROTOCOL_DEBUG
#define MODE_CHOICE 1
#define DATA_PACKET	3
#define ERROR_CRC 	5
#define DATA_FINISH 7

#define CODE 		0
#define RESOURCE	1

#define NULL 0

__text__
void _memset (void *s, int c, int n)
{
	char *ptr = (char *) s;
	while (n--)
		*ptr++ = c;
}


__text__
int _upgrade_resource_uart (unsigned char *buffer,int addr)
{
    int k = 0;
	int address = addr;
    unsigned char buf[SECT_SIZE];
    unsigned char wchecksum=0;
    unsigned char rchecksum=0;

    
	for(k=0;k<512;k++)
		buf[k]=buffer[5+k];
	
	if(address == 0){
		_putstring("Resource Update Start\n");
		_putstring("chip_erase start\n");
		_initial_lut();
		if(_ounprotected()) {
			_putstring("ounprotected fail\n");
			goto EXIT;
		}else{
			_putstring("ounprotected finish\n");
		}
		if(_spi_chip_erase()){
			_putstring("chip erase failed\n");
			goto EXIT;
		}else{
			_putstring("chip erase finish\n");
		}
	}
    
    /* second ... update*/
    //_putstring("write data\n");
	for(k=0;k<2;k++)
	{
		if(!_ospi_dma_write((buf+256*k),address))
		{
			address+=256;
		}
		else
		{
			_putstring("write data failed\n");
		}
	}
    wchecksum=wchecksum+buf[0]+buf[255]+buf[256]+buf[511];
	_putstring("checksum\n");
	rchecksum+=_ospi_read_byte(addr);
    rchecksum+=_ospi_read_byte(addr+255);
    rchecksum+=_ospi_read_byte(addr+256);
    rchecksum+=_ospi_read_byte(addr+511);
  
    
    if(wchecksum == rchecksum) {
        _putstring("Resource update finish\n");
        //set_upgrade_status(INFO_SPI_DONE,BIT_ON);
    }
    else{
        _putstring("Resource update fail\n");
		goto EXIT;
        //set_upgrade_status(INFO_SPI_FAIL,BIT_ON);
    }
	return 0;
EXIT:
	return -1;
}

__text__
int _upgrade_uart (unsigned char *buf,int addr)
{
    int k = 0;  
    unsigned char rchecksum_buf[SECT_SIZE];
	unsigned char wchecksum_buf[SECT_SIZE];
	if(addr == 0x00){
		_nor_wrsr (0x00);
		_putstring("Code Upgrade Start\n");
		_putstring("Erase nor flash\n");
		_chip_erase();
		_putstring("CHIP_ERASE FINISH\n");
	}
	for(k=0;k<512;k++)
		wchecksum_buf[k]=buf[k+5];
	_nor_write_sector (addr, wchecksum_buf);
	_memcpy(rchecksum_buf, addr| 0x80000000, 512); 
	for(k=0;k<512;k++) {                 
		if(rchecksum_buf[k] != wchecksum_buf[k])  {                      
			_putstring("Checksum error\n");
			return -1;
		}                
	}
	return 0;
}

__text__
void UART2_Send_DATA(const char c)
{
    int i;
    for (i = 0;  i < 100000; i++) {
        if (readb(UART_LSR(AFC_UART_BASE)) & 0x40) /* until Trasnsmitter empty */
            break;
    }
    writeb(c, UART_THR(AFC_UART_BASE));
}

__text__
void Pro_UART_SendBuf(unsigned char *Buf, int PackLen)
{
    unsigned short i;
    for(i=0; i<PackLen; i++)
    {
        UART2_Send_DATA(Buf[i]);
        if(i >=2 && Buf[i] == 0xFF)
        {
            UART2_Send_DATA(0x55);
        }
    }
}

__text__
unsigned char CheckSum( unsigned char *buf, int packLen )
{
    int				i;
    unsigned char		sum;

    if(buf == NULL || packLen <= 0) return 0;
    sum = 0;
    for(i=2; i<packLen-1; i++)
        sum += buf[i];

    return sum;
}

__text__
void send_ack_start()
{
	unsigned char buf[0x06]={0};
	buf[0]=0xff;buf[1]=0xff;buf[2]=0x00;buf[3]=0x02;buf[4]=0x02;buf[5]=0x04;
	Pro_UART_SendBuf(buf,0x06);
}

__text__
void send_ack_packet()
{
	unsigned char buf[0x06]={0};
	buf[0]=0xff;buf[1]=0xff;buf[2]=0x00;buf[3]=0x02;buf[4]=0x04;buf[5]=0x06;
	Pro_UART_SendBuf(buf,0x06);
}

__text__
void send_ack_fiinish()
{
	unsigned char buf[0x06]={0};
	buf[0]=0xff;buf[1]=0xff;buf[2]=0x00;buf[3]=0x02;buf[4]=0x08;buf[5]=0x0a;
	Pro_UART_SendBuf(buf,0x06);
}

__text__
void send_error_packet()
{
	unsigned char buf[0x06]={0};
	buf[0]=0xff;buf[1]=0xff;buf[2]=0x00;buf[3]=0x02;buf[4]=0x05;buf[5]=0x07;
	Pro_UART_SendBuf(buf,0x06);
}

__text__
void parser_cmd(unsigned char *buffer)
{
	static int addr = 0;
	static unsigned char type = 0;
	switch(buffer[0x04])
	{
		case 1:
			    /*unprotect*/
			if(buffer[5]==CODE)
				type = 0;
			else if(buffer[5]==RESOURCE)
				type = 1;
			send_ack_start();
			//printf("send_ack_start cmd = %x\n",cmd);
			break;
		case 3:
			if(type == CODE){
				if(_upgrade_uart(buffer,addr)== -1){
					addr = 0;
					send_error_packet();
				}else{
					addr+=512;
					send_ack_packet();
				}
			}else if(type == RESOURCE){
				if(_upgrade_resource_uart(buffer,addr)==-1){
					addr = 0;
					send_error_packet();
				}else{
					addr+=512;
					send_ack_packet();
				}
			}
			break;
		case 7:
			send_ack_fiinish();
			break;
		default:
			_putstring("unknown cmd \n");
			break;
	}
}

__text__
void Uart_Upgrade(void)
{
	unsigned short dataLen = 0;
	unsigned short count = 0;
	unsigned char buffer[MAX_PACKAGE_LEN] = {0};
	unsigned char  lastValue = 0;
	unsigned char  curValue = 0;
	//unsigned char type = 0;
	_putstring("Uart_Upgrade process\n");
	
	while(1){
		if (readb(UART_LSR(AFC_UART_BASE)) & 0x01) {
			curValue = readb(UART_RBR(AFC_UART_BASE));	
			if((lastValue == 0xFF)&&(curValue == 0xFF))
			{
				buffer[0] = 0xFF;
				buffer[1] = 0xFF;
				count = 2;
				continue;
			}
			if((lastValue == 0xFF)&&(curValue == 0x55))
			{
				lastValue = curValue;
				continue;
			}
			buffer[count] = curValue;
			count ++ ;
			lastValue = curValue;

			if(count ==4)
			{
				dataLen = buffer[2]*256+  buffer[3];
			}
			if(count ==  (dataLen + 4))
			{
				if(CheckSum(buffer,dataLen+4)!=buffer[dataLen+4-1]){
					_putstring("check sum error\n");
					send_error_packet();
				}else{
					parser_cmd(buffer);
					if(buffer[0x04]==DATA_FINISH){
						_putstring("DATA_FINISH\n");
						_nor_wrsr (0x3C);
						_sys_reset();
					}
					_memset(buffer, 0, sizeof(buffer));
					lastValue = curValue =0;
				}
			}
		}
	}
}
