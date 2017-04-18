/**
 *  @file   osd2.h
 *  @brief  head file for osd2 api (osd2.api.h in thetis)
 *  $Id: osd2.h,v 1.3 2015/04/08 09:29:12 onejoe Exp $
 *  $Author: onejoe $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/11/22  yc 	New file.
 *
 */

#ifndef _OSD2API_H
#define _OSD2API_H

#define OSD2_RAM_MAX_SIZE	(8*1024)	// 8K word
#define OSD2_MAIN_LUT_SIZE	(256*4)		// 256 colors, 1color RGBA 4bytes
#define OSD2_WRAP_LUT_SIZE	(32*4)		// 32 colors for non-char8bp charaters when set wrap over
										// but OSDTool not support
#define OSD2_2BP_LUT_SIZE	(16*4)		// 16 entries, 1 entries is 2bit(4) select remap from LUT
#define OSD2_BG_LUT_SIZE	(6)			// just limit to 6 BG for OSDTool
#define OSD2_REG_MAX_SIZE	(66*2)		// just limit to 66 REG for OSDTool


#include "iccontrol.h"

#if 0
#define IC_WRITEBYTE(PAGE, ADDR, VALUE)  IC_WritByte(PAGE, ADDR, VALUE)
#define IC_READBYTE(PAGE, ADDR)         IC_ReadByte(PAGE, ADDR)
/* Speed-up Macro */
#define OSD_CFG_WR(INDEX, DATA) do { IC_WRITBYTE(TWIC_P0, 0xA8, INDEX); IC_WRITBYTE(TWIC_P0, 0xA9, DATA); } while (0)
#define OSD_SET_RAM_ADDR(ADDR) do {	IC_WRITBYTE(TWIC_P0, 0xAA, (unsigned char)(ADDR)); IC_WRITBYTE(TWIC_P0, 0xAA, (unsigned char)(ADDR >> 8)); } while (0)
#define OSD_SET_RAM_DATA(DATA) do { IC_WRITBYTE(TWIC_P0, 0xAB, (unsigned char)(DATA)); IC_WRITBYTE(TWIC_P0, 0xAB, (unsigned char)(DATA >> 8)); } while (0)
#define OSD_SET_RAM_DATA_BYTE(DATA) do { IC_WRITBYTE(TWIC_P0, 0xAB, DATA); } while (0)
#else
#define IC_WRITEBYTE  			IC_WriteByte
#define IC_READBYTE         	IC_ReadByte
#define OSD_CFG_RD 				osd2_cfg_read
#define OSD_CFG_WR 				osd2_cfg_wr
#define OSD_SET_RAM_ADDR		osd2_set_ram_addr
#define OSD_SET_RAM_DATA		osd2_set_ram_data
#define OSD_SET_RAM_DATA_BYTE	osd2_set_ram_data_byte
#endif

/* ADVANCE FUNCTION */
/**
 * @func    osd_en_status
 * @brief	return OSD2_IS_ENABLE
 * @param   none
 * @return  OSD2_IS_ENABLE
 */
extern unsigned char osd_en_status();

extern unsigned char
osd2_cfg_read (unsigned char index);
/**
 * @func    osd2_cfg_wr
 * @brief	write value(byte) to OSD2 configure register.
 * @param   index	register index
 * 			dat		register value
 * @return  none
 */
extern void
osd2_cfg_wr (unsigned char index, unsigned char dat);
/**
 * @func    osd2_set_ram_addr
 * @brief	setting OSDRAM of address point at specified address.
 * @param   address		address point
 * @return  none
 */
extern void
osd2_set_ram_addr (unsigned short addr);
/**
 * @func    osd2_set_ram_addr
 * @brief	write word(2bytes) to OSDRAM by data port.
 * @param   address		address point
 * @return  none
 */
extern void
osd2_set_ram_data (unsigned short wdata);
/**
 * @func    osd2_set_ram_data_byte
 * @brief	write data(byte) to OSDRAM by data port, this function for
 * 			access byte mode only.
 * @param   address		address point
 * @return  none
 */
extern void
osd2_set_ram_data_byte(unsigned char dat);
/**
 * @func    _osd2_set_menu_addr
 * @brief	set current OSD2 menu's start address
 * @param   unsigned short addr		menu start address
 * @return  none
 */
extern void
_osd2_set_menu_addr (unsigned short addr);
/**
 * @func    _osd2_get_menu_addr
 * @brief	get current OSD2 menu's start address
 * @param   none
 * @return  menu start address
 */
extern unsigned short
_osd2_get_menu_addr (void);
/**
 * @func    _osd2_set_menu_width
 * @brief	set current OSD2 menu's ROW of total number
 * @param   unsigned char num 	ROW of total number
 * @return  none
 */
extern void
_osd2_set_menu_width (unsigned char num);
/**
 * @func    _osd2_get_menu_width
 * @brief	get current OSD2 menu's ROW of total number
 * @param   none
 * @return  ROW of total number
 */
extern unsigned char
_osd2_get_menu_width (void);
/**
 * @func    _osd2_set_menu_height
 * @brief	set current OSD2 menu's total ROWs
 * @param   unsigned char rows 	MENU of total ROWs
 * @return  none
 */
extern void
_osd2_set_menu_height (unsigned char rows);
/**
 * @func    _osd2_get_menu_height
 * @brief	get current OSD2 menu's total ROWs
 * @param   none
 * @return  MENU of total ROWs
 */
extern unsigned char
_osd2_get_menu_height (void);
/**
 * @func    _osd2_cq_cfg
 * @brief	write a OSD2 of register by CQ.
 * @param   unsigned char index		register of index
 * 			unsigned char dat		register of data
 * @return  none
 */
void
_osd2_cq_cfg (unsigned char index, unsigned char dat);
/**
 * @func    _osd2_cq_word
 * @brief	write a data to OSD2 RAM by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned short dat		word data
 * @return  none
 */
extern void
_osd2_cq_word (unsigned short addr, unsigned short dat);
/**
 * @func    _osd2_cq_oram
 * @brief	put data to OSD2 RAM from table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned char *tbl		table of point (1byte type, LSB first)
 * 			unsigned short length	fill of length
 * @return  none
 */
extern void
_osd2_cq_oram (unsigned short addr, unsigned char *tbl, unsigned short length);
/**
 * @func    _osd_cq_tw_font
 * @brief	put font data to OSD1 RAM from table by CQ(not yet CQ flush).
 * @param   unsigned char color     font color
 *          unsigned char font      font idx
 * 			unsigned short length	fill of length
 *          unsigned char height    font height
 *          unsigned char row       font row
 * @return  none
 */
extern void 
_osd_cq_tw_font(unsigned char color, unsigned char font, unsigned short length, 
                    unsigned char height, unsigned char row);
/**
 * @func    _osd2_cq_oram2
 * @brief	put character data to OSD2 RAM by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned short dat		character of  attribute
 * 			unsigned short length	fill of length
 * 			unsigned char auto_inc	put one data and LSB will auto increase
 * @return  none
 */
extern void
_osd2_cq_oram2 (unsigned short addr, unsigned short dat, unsigned short length, unsigned char auto_inc);
/**
 * @func    _osd2_cq_oram3
 * @brief	put character data to OSD2 RAM from font table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned char color		character of color attribute
 * 			unsigned char *tbl		font table of point
 * 			unsigned short length	fill of length
 * @return  none
 */
extern void
_osd2_cq_oram3 (unsigned short addr, unsigned char color, unsigned char *tbl, unsigned short length);
/**
 * @func    _osd2_cq_oram4
 * @brief	put character data to OSD2 RAM from font table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned char color		character of color attribute
 * 			unsigned char *tbl		font table of point
 * 			unsigned short length	fill of length
 *			unsigned char offset	index of offset
 * @return  none
 */
void
_osd2_cq_oram4 (unsigned short addr, unsigned char color, unsigned char *tbl, unsigned short length, unsigned char offset);
/**
 * @func    _osd2_cq_burst
 * @brief	OSD2 put character data with font index by CQ(not yet CQ flush).
 * @param   unsigned char row		row of start no. in the menu
 * 			unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * 			unsigned char hight		row of number
 * 			unsigned char color		character of color attribute
 * 			unsigned short dat		character of  attribute
 * 			unsigned char auto_inc	put one character and font index will auto increase
 * 									(Icon type should set this)
 * @return  none
 */
extern void
_osd2_cq_burst (unsigned char row, unsigned char num, unsigned char width,
	unsigned char hight, unsigned char color, unsigned short dat, unsigned char auto_inc);
/**
 * @func    _osd_cq_burst2
 * @brief	OSD2 put character data with font table by CQ(not yet CQ flush).
 * @param   unsigned char row		row of start no. in the menu
 * 			unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * 			unsigned char hight		row of number
 * 			unsigned char color		character of color attribute
 * 			unsigned char *font		font table of point
 * @return  none
 */
extern void
_osd2_cq_burst2 (unsigned char row, unsigned char num, unsigned char width,
	unsigned char hight, unsigned char color, unsigned char *font);
/**
 * @func    _osd2_cq_burst3
 * @brief	OSD2 put character data with font table by CQ(not yet CQ flush).
 * @param   unsigned char row		row of start no. in the menu
 * 			unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * 			unsigned char hight		row of number
 * 			unsigned char color		character of color attribute
 * 			unsigned char *font		font table of point
 *			unsigned char offset	index of offset
 * @return  none
 */
extern void
_osd2_cq_burst3 (unsigned char row, unsigned char num, unsigned char width,
	unsigned char hight, unsigned char color, unsigned char *font, unsigned char offset);
/**
 * @func    _osd2_cq_line_config
 * @brief
 * @param   unsigned char row		row of start no. in the menu
 * 			unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * @return  none
 */
extern void
_osd2_cq_line_config (unsigned char row, unsigned char num, unsigned char width);
/**
 * @func    _osd2_cq_line_data
 * @brief
 * @param   unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * 			unsigned short wdata	character of attribute
 * @return  none
 */
extern void
_osd2_cq_line_data (unsigned char num, unsigned char width, unsigned short wdata);
/**
 * @func    _osd2_set_loca_addr
 * @brief	setting address port of point of the OSDRAM with
 * 			X,Y coord position of the MENU.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
_osd2_set_loca_addr (unsigned char x, unsigned char y,
	unsigned short addr, unsigned char width);
/**
 * @func    osd2_set_location_addr
 * @brief	setting address port of point of the OSDRAM with
 * 			X,Y coord position of the MENU.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_set_location_addr (unsigned char x, unsigned char y,
	unsigned short addr, unsigned char width);
/**
 * @func    osd2_oram_fill
 * @brief	fill block at specified word(2bytes) to OSDRAM.
 * @param   addr	base address
 * 			length	block length
 * 			dat		specified data
 * @return  none
 */
extern void
osd2_oram_fill (unsigned short addr, unsigned short length, unsigned short dat);
/**
 * @func    osd2_wr_regs_tbl
 * @brief	load(configure) registers table to OSD2 REG.
 * @param   addr	resource of base address
 * 			length	table of length
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because memory can't DMA from SPIOSD NOR.
 */
extern void
osd2_wr_regs_tbl (unsigned long addr, unsigned char length);
/**
 * @func    osd2_wr_lut_tbl
 * @brief	load colors to OSD2 of LUT.
 * @param   addr	resource of base address
 * 			length	colors of total length (1color is 4bytes)
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because OSD2 LUT can't DMA from SPIOSD NOR.
 */
extern void
osd2_wr_lut_tbl (unsigned long addr, unsigned short length);
/**
 * @func    osd2_wr_2bp_lut_tbl
 * @brief	load colors to OSD2 of 2BP Remap LUT.
 * @param   addr	resource of base address
 * 			length	remap of total length (1 entries is 2bit(4) select)
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because OSD2 LUT can't DMA from SPIOSD NOR.
 */
extern void
osd2_wr_2bp_lut_tbl (unsigned long addr, unsigned char length);
/**
 * @func    osd2_wr_bg_lut_tbl
 * @brief	load colors to OSD2 of BG Remap LUT.
 * @param   addr	resource of base address
 * 			length	remap of total length (only 6bytes can configure)
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because OSD2 LUT can't DMA from SPIOSD NOR.
 */
extern void
osd2_wr_bg_lut_tbl(unsigned long addr, unsigned char length);
/**
 * @func    osd2_ram_clear
 * @brief	clear OSDRAM(fill '0').
 * @param   none
 * @return  none
 */
extern void
osd2_ram_clear (void);
/**
 * @func    osd2_menu1_enable
 * @brief	enable OSD2 and MENU1.
 * @param   none
 * @return  none
 * @note	driver and OSDTool just support MENU1
 * 			(OSD2 has two menu, MENU1&MENU2)
 */
extern void
osd2_menu1_enable (void);
/**
 * @func    osd2_menu1_disable
 * @brief	disable OSD2 and MENU1.
 * @param   none
 * @return  none
 * @note	driver and OSDTool just support MENU1
 * 			(OSD2 has two menu, MENU1&MENU2)
 */
extern void
osd2_menu1_disable (void);
/**
 * @func    osd2_enable
 * @brief	enable OSD2 and MENU1.
 * @param   none
 * @return  none
 */
extern void
osd2_enable (void);
/**
 * @func    osd2_disable
 * @brief	disable OSD2 and MENU1.
 * @param   none
 * @return  none
 */
extern void
osd2_disable (void);
/**
 * @func    osd2_update
 * @brief	update OSD2 of shadow registers or CQ flush.
 * @param   none
 * @return  none
 * @note	must used shadow or CQ when OSD2 enable
 */
extern void
osd2_update (void);
/**
 * @func    osd2_init
 * @brief	initialize for OSD2(just disable) and clear OSDRAM.
 * @param   none
 * @return  none
 * @note	must initialize OSD2(clear OSDRAM) when system boot
 */
extern void
osd2_init (void);
/**
 * @func    osd2_blink
 * @brief	enable OSD2 of blinking function and configure it's
 * 			frequency and duty.
 * @param   freq	blinking frequency, 00b for refresh rate /16,
 * 					01b for 1/32, 10b for 1/64, 11b for 1/128
 * 			duty	blinking duty cycie,
 * 					00b for Global blink off, 0% BK, 100% OSD2
 * 					01b for 25% BK, 75% OSD2
 * 					10b for 50% BK, 50% OSD2
 * 					11b for 75% BK, 25% OSD2
 * @return  none
 */
extern void
osd2_blink (unsigned char freq, unsigned char duty);
/**
 * @func    osd2_set_access_mode_LSB
 * @brief	set OSD2 access LSB mode at data port.
 * @param   none
 * @return  none
 * @note	just access LSB of the OSDRAM
 * 			when write data through data port
 */
extern void
osd2_set_access_mode_LSB (void);
/**
 * @func    osd2_set_access_mode_MSB
 * @brief	set OSD2 access MSB mode at data port.
 * @param   none
 * @return  none
 * @note	just access MSB of the OSDRAM
 * 			when write data through data port
 */
extern void
osd2_set_access_mode_MSB (void);
/**
 * @func    osd2_set_access_mode_word
 * @brief	set OSD2 access word mode(default) at data port.
 * @param   none
 * @return  none
 * @note	just access word of the OSDRAM
 * 			when write data through data port
 */
extern void
osd2_set_access_mode_word (void);
/**
 * @func    osd2_set_menu_start_end
 * @brief	set OSD2 MENU of start and end base address.
 * @param   star_addr	MENU of start base address
 * 			end_addr	MENU of end base address
 * @return  none
 */
extern void
osd2_set_menu_start_end (unsigned short star_addr, unsigned short end_addr);
/**
 * @func    osd2_com_color_atb
 * @brief	combine FG index, BG index and Blink flag
 * 			to color attribute(MSB).
 * @param   fg		FG index
 * 			bg		BG index
 * 			blink	Blink flag
 * @return  color attribute(character format of MSB)
 */
extern unsigned char
osd2_com_color_atb (unsigned char fg, unsigned char bg, unsigned char blink);
/**
 * @func    osd2_menu_write_byte
 * @brief	write a data(byte) to OSDRAM with MENU of location.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			dat		specified data(byte)
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 * @note	must at access MSB/LSB mode
 */
extern void
osd2_menu_write_byte (unsigned char x, unsigned char y, unsigned char dat,
					  unsigned short addr, unsigned char width);
/**
 * @func    osd2_menu_write_byte
 * @brief	write a word(font&color) to OSDRAM with MENU of location.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			dat		character format of data(word, 2bytes)
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_menu_write (unsigned char x, unsigned char y, unsigned short dat,
				 unsigned short addr, unsigned char width);
/**
 * @func    osd2_menu_write_byte
 * @brief	fill a word(font&color) to OSDRAM with MENU a ROW(line).
 * @param   y		y coord. of the MENU, unit is font
 * 			dat		character format of data(word, 2bytes)
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_menu_write_line (unsigned char y, unsigned short dat,
					  unsigned short addr, unsigned char width);
/**
 * @func    osd2_menu_write_block
 * @brief	fill a blank font(0) to OSDRAM with MENU a block.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			width	block of width, unit is font
 * 			height	block of height, unit is font
 * 			dat		character format of data(word, 2bytes)
 * 			addr	MENU of base address
 * 			m_width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_menu_write_block (unsigned char x, unsigned char y, unsigned char width,
				  	   unsigned char height, unsigned short dat,
				  	   unsigned short addr, unsigned char m_width);
/**
 * @func    osd2_clear_char
 * @brief	put a blank font(0) to OSDRAM with MENU of location.
 * @param   y		y coord. of the MENU, unit is font
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_clear_char (unsigned char x, unsigned char y, unsigned short addr, unsigned char width);
/**
 * @func    osd2_clear_line
 * @brief	fill a blank font(0) to OSDRAM with MENU a ROW(line).
 * @param   y		y coord. of the MENU, unit is font
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_clear_line (unsigned char y, unsigned short addr, unsigned char width);
/**
 * @func    osd2_clear_block
 * @brief	fill a blank font(0) to OSDRAM with MENU a block.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			width	block of width, unit is font
 * 			height	block of height, unit is font
 * 			addr	MENU of base address
 * 			m_width	MENU of width (ROW of characters)
 * @return  none
 */
extern void
osd2_clear_block (unsigned char x, unsigned char y, unsigned char width,
				  unsigned char height, unsigned short addr, unsigned char m_width);
/**
 * @func    osd2_clear_menu
 * @brief	fill a blank font(0) to OSDRAM with MENU a block.
 * @param   addr	MENU of base address
 * 			width	MENU of width, unit is font
 * 			height	MENU of height, unit is font
 * @return  none
 */
extern void
osd2_clear_menu (unsigned short addr, unsigned char width, unsigned char height);
/**
 * @func    osd2_menu_start_h
 * @brief	set a specified ROW of H start position of the MENU.
 * @param   x		x coord. of the display, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			row		specified ROW of the MENU
 * @return  none
 */
extern void
osd2_menu_start_h (unsigned short x, unsigned short addr,
				  unsigned char width, unsigned char row);
/**
 * @func    osd2_menu_location
 * @brief	set OSD2 MENU of location.
 * @param   x		x coord. of the MENU, unit is pixel
 * 			y		y coord. of the MENU, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			height	MENU of height (ROW)
 * @return  none
 */
extern void
osd2_menu_location (unsigned short x, unsigned short y, unsigned short addr,
				   unsigned char width, unsigned char height);
/**
 * @func    mem2oram
 * @brief   write data to OSDRAM from memory.
 * @param   base_address	NOR Flash of base address
 * 			oram_address	OSDRAM of point address
 * 			length			DMA of length
 * @return  0 if DAMA done, 1 if timeout
 */
extern int
mem2oram (unsigned long base_address, unsigned short oram_address, unsigned int length);
/**
 * @func    osd2_oram_dump
 * @brief	set OSD2 MENU of location.
 * @param   addr	OSDRAM of base address
 * 			length	dump data of length
 * @return  none
 */
extern void
osd2_oram_dump (unsigned short addr, unsigned short length);
/**
 * OSD2 BMP of control function,
 * current OSDTool not support BMP mode.
 */
/**
 * @func    _osd2_bmp_pos
 * @brief	configure OSD2 BMP of X,Y coord. position of the display.
 * @param   hstar	x coord. of the display, unit is pixel
 * 			vstar	y coord. of the display, unit is pixel
 * @return  none
 * @note	configure by shadow or CQ method, so need update to set.
 * 			just BMP mode used only and OSDTool not support
 */
extern void
_osd2_bmp_pos (unsigned short hstar, unsigned short vstar);
/**
 * @func    osd2_bmp_star_position
 * @brief	set OSD2 BMP of X,Y coord. position of the display.
 * @param   hstar	x coord. of the display, unit is pixel
 * 			vstar	y coord. of the display, unit is pixel
 * @return  none
 * @note	just BMP mode used only and OSDTool not support
 */
extern void
osd2_bmp_star_position (unsigned short hstart, unsigned short vstart);
/**
 * @func    osd2_bmp_disable
 * @brief	disable OSD2 BMP.
 * @param   none
 * @return  none
 * @note	just BMP mode used only and OSDTool not support
 */
extern void
osd2_bmp_disable (void);

extern unsigned char osd2_put_twf_char_to_oram (unsigned long flash_address, unsigned long file_address, unsigned short oram_address, unsigned short uc,  unsigned short byte_size, unsigned char isHSA);
extern unsigned long _twf_get_bmp_offset (unsigned long address, unsigned short uc);

#endif	/* _OSD2API_H */
