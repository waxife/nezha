/**
 *  @file   osd2.c
 *  @brief  terawins OSD2 control function (osd2api.c in thetis)
 *  $Id: osd2.c,v 1.13 2015/04/08 09:29:12 onejoe Exp $
 *  $Author: onejoe $
 *  $Revision: 1.13 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/11/22  yc 	New file.
 *
 */
#include <config.h>
#include "debug.h"
#include "reg_tw.h"
#include "iccontrol.h"

#include "./drivers/spirw/spirw.h"
#include "osd2.h"

#define ENABLE	1
#define DISABLE	0

//#ifdef USE_CQ_WRITE	/* for root control CQ write, maybe someday need it */
#include "./drivers/cq/cq.h"
#define OSD2_USE_CQ
#define OSD2_CQ_MAX_SIZE	(CQ_MAX_SIZE-3)
//#endif
//#define DEBUG_OSD2	// debug massage control

/* system global variable for OSD2 status */
static int OSD2_IS_ENABLE = DISABLE;
static int OSD2_MENU_ADDR= 0;
static int OSD2_MENU_WIDTH= 0;
static int OSD2_MENU_HEIGHT= 0;

/**
 * @func    osd_en_status
 * @brief	return OSD2_IS_ENABLE
 * @param   none
 * @return  OSD2_IS_ENABLE
 */
unsigned char osd_en_status()
{
    return OSD2_IS_ENABLE;
}

/**
 * @func    osd2_cfg_read
 * @brief	read back value(byte) from OSD2 configure register.
 * @param   index	register index
 * @return  register value
 */
unsigned char
osd2_cfg_read (unsigned char index)
{
	IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, index);
	return IC_READBYTE(TWIC_P0, OSD_CFG_DATA_REG);
}

/**
 * @func    osd2_cfg_wr
 * @brief	write value(byte) to OSD2 configure register.
 * @param   index	register index
 * 			dat		register value
 * @return  none
 */
void
osd2_cfg_wr (unsigned char index, unsigned char dat)
{
	IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, index);
	IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, dat);
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_cfg_wr 0x%X, 0x%X \n", index, dat);
#endif
}

/**
 * @func    osd2_set_ram_addr
 * @brief	setting OSDRAM of address point at specified address.
 * @param   address		address point
 * @return  none
 */
void
osd2_set_ram_addr (unsigned short addr)
{
  	/* word access, LSB first, then MSB byte */
	IC_WRITEBYTE(TWIC_P0, OSD_RAM_AL_REG, (unsigned char)(addr & 0xff));
	IC_WRITEBYTE(TWIC_P0, OSD_RAM_AH_REG, (unsigned char)(addr>>8));
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_set_ram_addr 0x%X \n", addr);
#endif
}

/**
 * @func    osd2_set_ram_addr
 * @brief	write word(2bytes) to OSDRAM by data port.
 * @param   address		address point
 * @return  none
 */
void
osd2_set_ram_data (unsigned short wdata)
{
	/* word access, LSB first, then MSB byte */
	IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, (unsigned char)(wdata & 0xff));
  	IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, (unsigned char)(wdata>>8));
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_set_ram_data 0x%X \n", wdata);
#endif
}	   

/**
 * @func    osd2_set_ram_data_byte
 * @brief	write data(byte) to OSDRAM by data port, this function for
 * 			access byte mode only.
 * @param   address		address point
 * @return  none
 */
void
osd2_set_ram_data_byte(unsigned char dat)
{			   
	IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, dat);
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_set_ram_data_byte 0x%02X \n", dat);
#endif
}

/**
 * @func    _osd2_set_menu_addr
 * @brief	set current OSD2 menu's start address
 * @param   unsigned short addr		menu start address
 * @return  none
 */
void
_osd2_set_menu_addr (unsigned short addr)
{
	OSD2_MENU_ADDR= addr;
}

/**
 * @func    _osd2_get_menu_addr
 * @brief	get current OSD2 menu's start address
 * @param   none
 * @return  menu start address
 */
unsigned short
_osd2_get_menu_addr (void)
{
	return (unsigned short) OSD2_MENU_ADDR;
}

/**
 * @func    _osd2_set_menu_width
 * @brief	set current OSD2 menu's ROW of total number
 * @param   unsigned char num 	ROW of total number
 * @return  none
 */
void
_osd2_set_menu_width (unsigned char num)
{
	OSD2_MENU_WIDTH= num;
}

/**
 * @func    _osd2_get_menu_width
 * @brief	get current OSD2 menu's ROW of total number
 * @param   none
 * @return  ROW of total number
 */
unsigned char
_osd2_get_menu_width (void)
{
	return OSD2_MENU_WIDTH;
}

/**
 * @func    _osd2_set_menu_height
 * @brief	set current OSD2 menu's total ROWs
 * @param   unsigned char rows 	MENU of total ROWs
 * @return  none
 */
void
_osd2_set_menu_height (unsigned char rows)
{
	OSD2_MENU_HEIGHT= rows;
}

/**
 * @func    _osd2_get_menu_height
 * @brief	get current OSD2 menu's total ROWs
 * @param   none
 * @return  MENU of total ROWs
 */
unsigned char
_osd2_get_menu_height (void)
{
	return OSD2_MENU_HEIGHT;
}

#ifdef OSD2_USE_CQ
/**
 * @func    _osd2_cq_cfg
 * @brief	write a OSD2 of register by CQ.
 * @param   unsigned char index		register of index
 * 			unsigned char dat		register of data
 * @return  none
 */
void
_osd2_cq_cfg (unsigned char index, unsigned char dat)
{
	/* write OSD RAM by CQ when OSD enable */
//	cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source

	cq_write_word(CQ_P0, OSD_CFG_INDEX_REG, ((dat<<8)|index), 1);
}

/**
 * @func    _osd2_cq_word
 * @brief	write a data to OSD2 RAM by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned short dat		word data
 * @return  none
 */
void
_osd2_cq_word (unsigned short addr, unsigned short dat)
{
	/* write OSD RAM by CQ when OSD enable */
//	cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source

	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);
	cq_write_word(CQ_P0, OSD_RAM_DL_REG, dat, 0);
}

/**
 * @func    _osd2_cq_oram
 * @brief	put data to OSD2 RAM from table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned char *tbl		table of point (1byte type, LSB first)
 * 			unsigned short length	fill of length
 * @return  none
 */
void
_osd2_cq_oram (unsigned short addr, unsigned char *tbl, unsigned short length)
{
	int i= 0;
	int cnt= 0;
	int j= 0;

	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}

	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);

	for(j= 0; j < (length/OSD2_CQ_MAX_SIZE); j++) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, OSD2_CQ_MAX_SIZE);
		for(i=0; i < OSD2_CQ_MAX_SIZE; i+=2, cnt+=2) {
			cq_data(tbl[cnt]);		// put data to CQ of queue
			cq_data(tbl[cnt+1]);	// put data to CQ of queue
		}
	}
	j= (length % OSD2_CQ_MAX_SIZE);
	if(j) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, j);
		for(i=0; i<j; i+=2, cnt+=2) {
			cq_data(tbl[cnt]);		// put data to CQ of queue
			cq_data(tbl[cnt+1]);	// put data to CQ of queue
		}
	}
}

/**
 * @func    _osd_cq_tw_font
 * @brief	put font data to OSD1 RAM from table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 *          unsigned char color     font color
 *          unsigned char font      font idx
 * 			unsigned short length	fill of length
 *          unsigned char height    font height
 *          unsigned char row       font row
 * @return  none
 */
void _osd_cq_tw_font (unsigned char color, unsigned char idx, unsigned short length, 
    unsigned char h, unsigned char row)
{
    unsigned short i= 0;
    unsigned short cnt= 0;
    unsigned char j= 0;
    unsigned short dat;

    length*= 2; // word 2 bytes

    if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}

    /* setting OSD RAM of address */
	//cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);    


    for(j= 0; j < (length/OSD2_CQ_MAX_SIZE); j++) {
        /* set target data port and length and no inc. */
        cq_config (CQ_P0, OSD_RAM_DL_REG, 0, OSD2_CQ_MAX_SIZE);
        for(i=0; i < OSD2_CQ_MAX_SIZE; i+=2, cnt+=h) {
            dat = (color<<9) + idx + cnt + row;
            cq_data(dat&0xff);  // put data to CQ of queue
            cq_data(dat>>8);   // put data to CQ of queue
        }
    }
    j= (length % OSD2_CQ_MAX_SIZE);
    if(j) {
        /* set target data port and length and no inc. */
        cq_config (CQ_P0, OSD_RAM_DL_REG, 0, j);
        for(i=0; i<j; i+=2, cnt+=h) {
            dat = (color<<9) + idx + cnt + row;
            cq_data(dat&0xff);  // put data to CQ of queue
            cq_data(dat>>8);   // put data to CQ of queue
        }
    }
}

#if 0
/**
 * @func    osd2_cq_oram_cut
 * @brief	put data to OSD2 RAM from table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned char *buf		canvas of buffer point (1byte type, LSB first)
 * 			unsigned short length	fill of length
 * @return  none
 */
void
osd2_cq_oram_cut (int font_w, int font_h, int bp, int width, int height,
	int addr, unsigned char *dbuf, int size)
{
	int i= 0, j = 0, k = 0, idx = 0;
	int length = 0;

	if(font_w != 16 && font_w != 12) {
		ERROR("not support a font width of %d!\n", font_w);
		return;
	}
	if(font_h % 2 || font_h > 24 || font_h <= 0) {
		ERROR("not support a font height of %d!\n", font_h);
		return;
	}
	if(bp > 2 || bp < 1) {
		ERROR("not support a bit mode of %d!\n", bp);
		return;
	}
	if(size < 2) {
		ERROR("too small a size of %d!\n", size);
		return;
	}

	addr = (addr & 0x1FFF);	// 8K Word ORAM
//	if(OSD2_IS_ENABLE) {
//		/* write OSD RAM by CQ when OSD enable */
//		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
//	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
//	}

	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);
	/* set target data port and length and no inc. */
	if(size > OSD2_CQ_MAX_SIZE) length = OSD2_CQ_MAX_SIZE;
	else length = size;
	size -= length;
	cq_config (CQ_P0, OSD_RAM_DL_REG, 0, length);
	for(j = 0; j < (height/font_h); j++) {
		for(i = 0; i < (width/font_w); i++) {
			for(k = 0; k < font_h; k++) {
				idx = i + (k * (width/font_w));
				idx += ((j * font_h) * (width/font_w));
				/* word access, LSB first, then MSB byte */
				cq_data(dbuf[(idx * 2) + 1]);	// put data to CQ of queue
				cq_data(dbuf[(idx * 2)]);		// put data to CQ of queue
				length -= 2;
				if(length <= 0 && size > 0) {
					cq_flush();
					if(size > OSD2_CQ_MAX_SIZE) length = OSD2_CQ_MAX_SIZE;
					else length = size;
					size -= length;
					/* set target data port and length and no inc. */
					cq_config (CQ_P0, OSD_RAM_DL_REG, 0, length);
				}
			}
		}
	}
	if(cq_available() < CQ_MAX_SIZE) {
		if (cq_flush () < 0)
			ERROR ("cq_flush()\n");
	}
}
#endif

/**
 * @func    _osd2_cq_oram2
 * @brief	put character data to OSD2 RAM by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned short dat		character of  attribute
 * 			unsigned short length	fill of length
 * 			unsigned char auto_inc	put one data and LSB will auto increase
 * @return  none
 */
void
_osd2_cq_oram2 (unsigned short addr, unsigned short dat, unsigned short length, unsigned char auto_inc)
{
	int i= 0;
	int j= 0;
	unsigned short tmp = dat;

	length*= 2;	// word 2 bytes

	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}

	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);

	for(j= 0; j<(length/OSD2_CQ_MAX_SIZE); j++) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, OSD2_CQ_MAX_SIZE);
		for(i=0; i < OSD2_CQ_MAX_SIZE; i+=2) {
			cq_data((tmp&0xFF));		// put LSB data to CQ of queue
			cq_data(((tmp>>8)&0xFF));	// put MSB data to CQ of queue
			if(auto_inc)
				tmp++;
		}
	}
	j= (length % OSD2_CQ_MAX_SIZE);
	if(j) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, j);
		for(i=0; i < j; i+=2) {
			cq_data((tmp&0xFF));		// put LSB data to CQ of queue
			cq_data(((tmp>>8)&0xFF));	// put MSB data to CQ of queue
			if(auto_inc)
				tmp++;
		}
	}
}

/**
 * @func    _osd2_cq_oram3
 * @brief	put character data to OSD2 RAM from font table by CQ(not yet CQ flush).
 * @param   unsigned short addr		OSD RAM of address
 * 			unsigned char color		character of color attribute
 * 			unsigned char *tbl		font table of point
 * 			unsigned short length	fill of length
 * @return  none
 */
void
_osd2_cq_oram3 (unsigned short addr, unsigned char color, unsigned char *tbl, unsigned short length)
{
	int i= 0;
	int cnt= 0;
	int j= 0;

	length*= 2;	// word 2 bytes

	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}

	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);

	for(j= 0; j<(length/OSD2_CQ_MAX_SIZE); j++) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, OSD2_CQ_MAX_SIZE);
		for(i=0; i < OSD2_CQ_MAX_SIZE; i+=2, cnt++) {
			cq_data(tbl[cnt]);		// put data to CQ of queue
			cq_data(color);			// put data to CQ of queue
		}
	}
	j= (length % OSD2_CQ_MAX_SIZE);
	if(j) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, j);
		for(i=0; i<j; i+=2, cnt++) {
			cq_data(tbl[cnt]);		// put data to CQ of queue
			cq_data(color);			// put data to CQ of queue
		}
	}
}

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
_osd2_cq_oram4 (unsigned short addr, unsigned char color, unsigned char *tbl, unsigned short length, unsigned char offset)
{
	int i= 0;
	int cnt= 0;
	int j= 0;
	unsigned short tmp = 0;

	length*= 2;	// word 2 bytes

	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}
	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);

	for(j= 0; j < (length/OSD2_CQ_MAX_SIZE); j++) {
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_RAM_DL_REG, 0, OSD2_CQ_MAX_SIZE);
		for(i=0; i < OSD2_CQ_MAX_SIZE; i+=2, cnt++) {
			tmp = ((color<<8)|(tbl[cnt]+offset));
			cq_data((tmp&0xFF));		// put LSB data to CQ of queue
			cq_data(((tmp>>8)&0xFF));		// put MSB data to CQ of queue
		}
	}
	j= (length % OSD2_CQ_MAX_SIZE);
	if(j) {
		/* set target data port and length and no inc. */
		cq_config (TWIC_P0, OSD_RAM_DL_REG, 0, j);
		for(i=0; i<j; i+=2, cnt++) {
			tmp = ((color<<8)|(tbl[cnt]+offset));
			cq_data((tmp&0xFF));		// put LSB data to CQ of queue
			cq_data(((tmp>>8)&0xFF));		// put MSB data to CQ of queue
		}
	}
}

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
void
_osd2_cq_burst (unsigned char row, unsigned char num, unsigned char width,
	unsigned char hight, unsigned char color, unsigned short dat, unsigned char auto_inc)
{
	int i= 0, j= 0;

	/* modify width if out off range */
	if((width+num) > OSD2_MENU_WIDTH)
		j= width-((width+num)-OSD2_MENU_WIDTH);
	else
		j= width;

	for(i= 0; i<hight; i++) {
		if((i+row) >= OSD2_MENU_HEIGHT)
			break;	// out off range

		/* write to OSD RAM by CQ */
		_osd2_cq_oram2((OSD2_MENU_ADDR + ((row+i)*(OSD2_MENU_WIDTH+4)) + num  + 4),
			((color<<8)|dat), j, auto_inc);

		if(auto_inc)
			dat+= width;
	}
}

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
void
_osd2_cq_burst2 (unsigned char row, unsigned char num, unsigned char width,
	unsigned char hight, unsigned char color, unsigned char *font)
{
	int i= 0, j= 0;

	/* modify width if out off range */
	if((width+num) > OSD2_MENU_WIDTH)
		j= width-((width+num)-OSD2_MENU_WIDTH);
	else
		j= width;

	for(i= 0; i<hight; i++) {
		if((i+row) >= OSD2_MENU_HEIGHT)
			break;	// out off range

		/* write to OSD RAM by CQ */
		_osd2_cq_oram3((OSD2_MENU_ADDR + ((row+i)*(OSD2_MENU_WIDTH+4)) + num  + 4),
			color, font, j);

		font+= width;
	}
}

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
void
_osd2_cq_burst3 (unsigned char row, unsigned char num, unsigned char width,
	unsigned char hight, unsigned char color, unsigned char *font, unsigned char offset)
{
	int i= 0, j= 0;

	/* modify width if out off range */
	if((width+num) > OSD2_MENU_WIDTH)
		j= width-((width+num)-OSD2_MENU_WIDTH);
	else
		j= width;

	for(i= 0; i<hight; i++) {
		if((i+row) >= OSD2_MENU_HEIGHT)
			break;	// out off range

		/* write to OSD RAM by CQ */
		_osd2_cq_oram4((OSD2_MENU_ADDR + ((row+i)*(OSD2_MENU_WIDTH+4)) + num  + 4),
			color, font, j, offset);

		font+= width;
	}
}

/**
 * @func    _osd2_cq_line_config
 * @brief
 * @param   unsigned char row		row of start no. in the menu
 * 			unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * @return  none
 */
void
_osd2_cq_line_config (unsigned char row, unsigned char num, unsigned char width)
{
	int j= 0;

	/* modify width if out off range */
	if((width+num) > OSD2_MENU_WIDTH)
		j= width-((width+num)-OSD2_MENU_WIDTH);
	else
		j= width;

	j *= 2;
	/* setting OSD RAM of address */
	cq_write_word(CQ_P0, OSD_RAM_AL_REG, (OSD2_MENU_ADDR + (row*(OSD2_MENU_WIDTH+4)) + num  + 4), 0);
	cq_config (CQ_P0, OSD_RAM_DL_REG, 0, j);
}

/**
 * @func    _osd2_cq_line_data
 * @brief
 * @param   unsigned char num		character of start no. in the menu
 * 			unsigned char width		character of number
 * 			unsigned short wdata	character of attribute
 * @return  none
 */
void
_osd2_cq_line_data (unsigned char num, unsigned char width, unsigned short wdata)
{
	/* modify width if out off range */
	if((width+num) > OSD2_MENU_WIDTH) {
		WARN("out menu width %d!\n", OSD2_MENU_WIDTH);
		return;
	}
	cq_data((wdata&0xFF));		// put LSB data to CQ of queue
	cq_data(((wdata>>8)&0xFF));	// put MSB data to CQ of queue
}
#endif

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
void
_osd2_set_loca_addr (unsigned char x, unsigned char y,
					unsigned short addr, unsigned char width)
{
	addr = addr + ((width + 4) * y + x + 4);	// '4' is ROW Attribute

	/* setting OSD RAM of address */
#ifdef OSD2_USE_CQ
//	if(OSD2_IS_ENABLE) {
		cq_write_word(CQ_P0, OSD_RAM_AL_REG, addr, 0);
//	} else {
//		IC_WRITEBYTE(TWIC_P0, OSD_RAM_AL_REG, (unsigned char)(addr & 0xff));
//		IC_WRITEBYTE(TWIC_P0, OSD_RAM_AH_REG, (unsigned char)(addr>>8));
//	}
#else
	IC_WRITEBYTE(TWIC_P0, OSD_RAM_AL_REG, (unsigned char)(addr & 0xff));
	IC_WRITEBYTE(TWIC_P0, OSD_RAM_AH_REG, (unsigned char)(addr>>8));
#endif

#ifdef	DEBUG_OSD2
	dbg(0, "_osd2_set_loca_addr \n");
	dbg(0, "x:%d, y:%d, addr:0x%04hX, width:%d \n", x, y, addr, width);
#endif
}

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
void
osd2_set_location_addr (unsigned char x, unsigned char y,
						unsigned short addr, unsigned char width)
{
	/* setting OSD RAM of address */
	_osd2_set_loca_addr(x, y, addr, width);
	osd2_update();
}

/**
 * @func    osd2_oram_fill
 * @brief	fill block at specified word(2bytes) to OSDRAM.
 * @param   addr	base address
 * 			length	block length
 * 			dat		specified data
 * @return  none
 */
void
osd2_oram_fill (unsigned short addr, unsigned short length, unsigned short dat)
{								   
	unsigned short count = 0;	

	/* check block length */
	if(length > OSD2_RAM_MAX_SIZE) {
		ERROR("osd2_oram_fill: length too long(%d > %d).\n", length, OSD2_RAM_MAX_SIZE);
		return;
	}
	 
	/* setting specified data */
	osd2_cfg_wr(OSD_BLK_WRT_LSB, (dat & 0x00ff));
	osd2_cfg_wr(OSD_BLK_WRT_MSB, ((dat & 0xff00)>>8));
	/* setting block of base address */
	osd2_cfg_wr(OSD_BLK_WRT_SADD_LSB, (addr & 0x00ff));
	osd2_cfg_wr(OSD_BLK_WRT_SADD_MSB, ((addr & 0xff00)>>8));
	/* setting block length & trigger */
	osd2_cfg_wr(OSD_BLK_WRT_LENGTH, (length & 0x00ff));
	osd2_cfg_wr(OSD_BLK_WRT_CTR, (((length & 0xff00)>>8) | 0x80));

	/* check fill done */
	while(!(IC_READBYTE(TWIC_P0, OSD_CFG_DATA_REG) & 0x80)) {
		count++;
		if(count >= 65535) {
			ERROR("osd2_oram_fill: time out.\n");	
			return;
		}
	}
}

/**
 * @func    osd2_wr_regs_tbl
 * @brief	load(configure) registers table to OSD2 REG.
 * @param   addr	resource of base address
 * 			length	table of length
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because memory can't DMA from SPIOSD NOR.
 */
void
osd2_wr_regs_tbl (unsigned long addr, unsigned char length)
{
	unsigned char i=0;
	char *src = (char*)addr;

	if (length == 0) {
		ERROR("length can't is zero.\n");
		return;
	} else if (length > OSD2_REG_MAX_SIZE) {
		ERROR("osd2_wr_regs_tbl: length too long(%d > %d).\n", length, OSD2_REG_MAX_SIZE);
		return;
	}
#ifdef	DEBUG_OSD2
    dbg(0, "osd2_wr_regs_tbl(%lX, %u) \n", addr, length);
#endif
	for (i = 0; i < length; i+=2)   {
		osd2_cfg_wr(src[i], src[i+1]);
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_cfg_wr(0x%02X, 0x%02X)\n", src[i], src[i+1]);
#endif
	}
}

/**
 * @func    osd2_wr_lut_tbl
 * @brief	load colors to OSD2 of LUT.
 * @param   addr	resource of base address
 * 			length	colors of total length (1color is 4bytes)
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because OSD2 LUT can't DMA from SPIOSD NOR.
 */
void
osd2_wr_lut_tbl (unsigned long addr, unsigned short length)
{
	int i = 0;
	char *src = (char*)addr;

	if (length == 0) {
		ERROR("length can't is zero.\n");
		return;
	} else if (length > OSD2_MAIN_LUT_SIZE) {
		ERROR("osd2_wr_lut_tbl: length too long(%d > %d).\n", length, OSD2_MAIN_LUT_SIZE);
		return;
	}
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_wr_lut_tbl(0x%lX, %u) \n", addr, length);
#endif

#if 0//def OSD2_USE_CQ
	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}

#if 1
		_osd2_cq_cfg(OSD_COLOR_LUT_ADR_PORT, 0x00);	// OSD LUT address start point
		cq_write_byte(CQ_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
		for(j= 0; j<(length/OSD2_CQ_MAX_SIZE); j++) {
			/* set target data port and length and no inc. */
			cq_config (CQ_P0, OSD_CFG_DATA_REG, 0, OSD2_CQ_MAX_SIZE);
			for(i=0; i < OSD2_CQ_MAX_SIZE; i++) {
				cq_data(src[cnt++]);		// put data to CQ of queue
			}
		}
		j= (length % OSD2_CQ_MAX_SIZE);
		if(j) {
			/* set target data port and length and no inc. */
			cq_config (CQ_P0, OSD_CFG_DATA_REG, 0, j);
			for(i=0; i < j; i++) {
				cq_data(src[cnt++]);		// put data to CQ of queue
			}
            printf("cnt = %d\n", cnt);
		}
#else
		_osd2_cq_cfg(OSD_COLOR_LUT_ADR_PORT, 0x00);	// OSD LUT address start point
		cq_write_byte(CQ_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_CFG_DATA_REG, 0, length);
		for(i=0; i < length; i++)
			cq_data(src[i]);	// put data to CQ of queue
#endif
//	} else {
//		osd2_cfg_wr(OSD_COLOR_LUT_ADR_PORT, 0x00);	// OSD LUT address start point
//		IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
//		/* open data port, writing datas now */
//		for (i = 0; i < length; i++) {
//			IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, src[i]);
//		}
//	}
#else
	osd2_cfg_wr(OSD_COLOR_LUT_ADR_PORT, 0x00);	// OSD LUT address start point
	IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
	/* open data port, writing datas now */
	for (i = 0; i < length; i++) {
		IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, src[i]);
    }
#endif
#ifdef	DEBUG_OSD2
	for (i = 0; i < length; i++) {
		DBG_PRINT("0x%X, ", src[i]);
		if(!((i+1)%16))
			DBG_PRINT("\n");
		if((i+1) >= length)
			DBG_PRINT("\n");
	}
#endif
}

/**
 * @func    osd2_wr_2bp_lut_tbl
 * @brief	load colors to OSD2 of 2BP Remap LUT.
 * @param   addr	resource of base address
 * 			length	remap of total length (1 entries is 2bit(4) select)
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because OSD2 LUT can't DMA from SPIOSD NOR.
 */
void
osd2_wr_2bp_lut_tbl (unsigned long addr, unsigned char length)
{
	int i = 0;
	char *src = (char*)addr;

	if (length == 0) {
		ERROR("length can't is zero.\n");
		return;
	} else if (length > OSD2_2BP_LUT_SIZE) {
		ERROR("osd2_wr_2bp_lut_tbl: length too long(%d > %d).\n", length, OSD2_2BP_LUT_SIZE);
		return;
	}

#ifdef	DEBUG_OSD2
	dbg(0, "osd2_wr_2bp_lut_tbl(0x%lX, %u) \n", addr, length);
#endif

#if 0//def OSD2_USE_CQ
	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}
		_osd2_cq_cfg(OSD_COLOR_LUT_ADR_PORT, 0x90);	// OSD LUT address start point (<<2)
		cq_write_byte(CQ_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_CFG_DATA_REG, 0, length);
		for (i = 0; i < length; i++) {
			if((i%4) == 0)	// X must dummy write in T582 version for fixed bug. O tool data start from 2bp lut "+1"
				cq_data(0x00);
			else
				cq_data(src[(i-1)]);	// put data to CQ of queue
		}
//	} else {
//		osd2_cfg_wr(OSD_COLOR_LUT_ADR_PORT, 0x90);	// OSD LUT address start point (<<2)
//		IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
//		/* open data port, writing datas now */
//		for (i = 0; i < length; i++) {
//			if((i%4) == 0)	// must dummy write in T582 version for fixed bug
//				IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, 0x00);
//			else
//				IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, src[(i-1)]);
//		}
//	}
#else
	osd2_cfg_wr(OSD_COLOR_LUT_ADR_PORT, 0x90);	// OSD LUT address start point (<<2)
	IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
	/* open data port, writing datas now */
	for (i = 0; i < length; i++) {
		if((i%4) == 0)	// X must dummy write in T582 version for fixed bug. O tool data start from 2bp lut "+1"
			IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, 0x00);
		else
			IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, src[(i-1)]);
	}
#endif
#ifdef	DEBUG_OSD2
	for (i = 0; i < length; i++) {
		if((i%4)) DBG_PRINT("0x%02X, ", (unsigned char)src[i]);
		else	 DBG_PRINT("0x00, ");
		if(!(i%16)) 	DBG_PRINT("\n");
		if(i >= length) DBG_PRINT("\n");
	}
#endif
}

/**
 * @func    osd2_wr_bg_lut_tbl
 * @brief	load colors to OSD2 of BG Remap LUT.
 * @param   addr	resource of base address
 * 			length	remap of total length (only 6bytes can configure)
 * @return  none
 * @note	this colors resource must save in CODE NOR,
 * 			because OSD2 LUT can't DMA from SPIOSD NOR.
 */
void
osd2_wr_bg_lut_tbl(unsigned long addr, unsigned char length)
{
	int i = 0;
	char *src = (char*)addr;

	if (length == 0) {
		ERROR("length can't is zero.\n");
		return;
	} else if (length > OSD2_BG_LUT_SIZE) {
		ERROR("osd2_wr_bg_lut_tbl: length too long(%d > %d).\n", length, OSD2_BG_LUT_SIZE);
		return;
	}
#ifdef	DEBUG_OSD2
	dbg(0, "osd2_wr_bg_lut_tbl(0x%lX, %u) \n", addr, length);
#endif

#if 0//def OSD2_USE_CQ
	if(OSD2_IS_ENABLE) {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_VDE);	// select trigger source
	} else {
		/* write OSD RAM by CQ when OSD enable */
		cq_trigger_source(CQ_TRIGGER_SW);	// select trigger source
	}
		_osd2_cq_cfg(OSD_COLOR_LUT_ADR_PORT, 0x98);	// OSD LUT address start point (<<2)
		cq_write_byte(CQ_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
		/* set target data port and length and no inc. */
		cq_config (CQ_P0, OSD_CFG_DATA_REG, 0, length);
		for (i = 0; i < length; i++) {
			if((i%4) == 0)	// must dummy write in T582 version for fixed bug
				cq_data(0x00);
			else
				cq_data(src[(i-1)]);	// put data to CQ of queue
		}
//	} else {
//		osd2_cfg_wr(OSD_COLOR_LUT_ADR_PORT, 0x98);	// OSD LUT address start point (<<2)
//		IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
//		/* open data port, writing datas now */
//		for (i = 0; i < length; i++) {
//			if(i == 0)	// must dummy write in T582 version for fixed bug
//				IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, 0x00);
//			else
//				IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, src[(i-1)]);
//		}
//	}
#else
	osd2_cfg_wr(OSD_COLOR_LUT_ADR_PORT, 0x98);	// OSD LUT address start point (<<2)
	IC_WRITEBYTE(TWIC_P0, OSD_CFG_INDEX_REG, OSD_COLOR_LUT_DATA_PORT);
	/* open data port, writing datas now */
	for (i = 0; i < length; i++) {
		if((i%4) == 0)	// must dummy write in T582 version for fixed bug
			IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, 0x00);
		else
			IC_WRITEBYTE(TWIC_P0, OSD_CFG_DATA_REG, src[(i-1)]);
	}
#endif
#ifdef	DEBUG_OSD2
	for (i = 0; i < length; i++) {
		if((i%4)) DBG_PRINT("0x%02X, ", (unsigned char)src[i]);
		else	 DBG_PRINT("0x00, ");
		if(!(i%16)) 	DBG_PRINT("\n");
		if(i >= length) DBG_PRINT("\n");
	}
#endif
}

/**
 * @func    osd2_ram_clear
 * @brief	clear OSDRAM(fill '0').
 * @param   none
 * @return  none
 */
void
osd2_ram_clear (void)
{
	/* clear OSD RAM */
	osd2_oram_fill(0x0000, OSD2_RAM_MAX_SIZE, 0x0000);
}

/**
 * @func    osd2_menu1_enable
 * @brief	enable OSD2 and MENU1.
 * @param   none
 * @return  none
 * @note	driver and OSDTool just support MENU1
 * 			(OSD2 has two menu, MENU1&MENU2)
 */
void
osd2_menu1_enable (void)
{
    osd2_cfg_wr(OSD_MENU1_ENABLE, 0x80);	// MENU1 enable
    osd2_cfg_wr(OSD_CTRL, 0x80);      	// OSD1 enable
    IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
    OSD2_IS_ENABLE = ENABLE;
}

/**
 * @func    osd2_menu1_disable
 * @brief	disable OSD2 and MENU1.
 * @param   none
 * @return  none
 * @note	driver and OSDTool just support MENU1
 * 			(OSD2 has two menu, MENU1&MENU2)
 */
void
osd2_menu1_disable (void)
{
    osd2_cfg_wr(OSD_MENU1_ENABLE, 0x00);	// MENU1 disable
    osd2_cfg_wr(OSD_CTRL, 0x00);      	// OSD1 disable
    IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
    OSD2_IS_ENABLE = DISABLE;
}

/**
 * @func    osd2_enable
 * @brief	enable OSD2 and MENU1.
 * @param   none
 * @return  none
 */
void
osd2_enable (void)
{
	unsigned char tmp= 0;

	tmp= osd2_cfg_read(OSD_CTRL);
	tmp|= 0x80;	// [7] enable OSD2

#ifdef OSD2_USE_CQ
//	if(OSD2_IS_ENABLE) {
		_osd2_cq_cfg(OSD_MENU1_ENABLE, 0x80);
		_osd2_cq_cfg(OSD_CTRL, tmp);
//	} else {
//		osd2_cfg_wr(OSD_MENU1_ENABLE, 0x80);	// MENU1 enable
//		osd2_cfg_wr(OSD_CTRL, tmp);      	// OSD1 enable
//		IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
//	}
    OSD2_IS_ENABLE= ENABLE;
	osd2_update();
	
#else
	osd2_cfg_wr(OSD_MENU1_ENABLE, 0x80);	// MENU1 enable
	osd2_cfg_wr(OSD_CTRL, tmp);      	// OSD1 enable
	IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
	OSD2_IS_ENABLE= ENABLE;
#endif
}

/**
 * @func    osd2_disable
 * @brief	disable OSD2 and MENU1.
 * @param   none
 * @return  none
 */
void
osd2_disable (void)
{
	unsigned char tmp= 0;

	tmp= osd2_cfg_read(OSD_CTRL);
	tmp&= 0x7F;	// [7] disable OSD1

#ifdef OSD2_USE_CQ
	_osd2_cq_cfg(OSD_CTRL, tmp);
	osd2_update();
	OSD2_IS_ENABLE= DISABLE;
#else
	osd2_cfg_wr(OSD_MENU1_ENABLE, 0x00);	// MENU1 disable
	osd2_cfg_wr(OSD_CTRL, 0x00);      	// OSD1 disable
	IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
	OSD2_IS_ENABLE= DISABLE;
#endif
}

/**
 * @func    osd2_update
 * @brief	update OSD2 of shadow registers or CQ flush.
 * @param   none
 * @return  none
 * @note	must used shadow or CQ when OSD2 enable
 */
void
osd2_update (void)
{
#ifdef OSD2_USE_CQ
	if(OSD2_IS_ENABLE) {
		if (cq_flush_vde () < 0)
			ERROR ("cq_flush_vde ()\n");
	} else {
		if (cq_flush_now () < 0)
			ERROR ("cq_flush_now ()\n");
	}
	/* must shadow update when shadow enable */
	if(IC_READBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG) & 0x10) {
		IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
	}
#else
	if(OSD2_IS_ENABLE) {
		IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);		// Shadow Update
	}
#endif
}

/**
 * @func    osd2_init
 * @brief	initialize for OSD2(just disable) and clear OSDRAM.
 * @param   none
 * @return  none
 * @note	must initialize OSD2(clear OSDRAM) when system boot
 */
void
osd2_init (void)
{
	/* OSD Disable */
    osd2_disable();
	//osd2_cfg_wr(OSD_CTRL, 0x00);
	/* clear OSD RAM */
	osd2_ram_clear();
}

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
void
osd2_blink (unsigned char freq, unsigned char duty)
{
	freq = ((freq<<2)&0x0C);
	duty = (duty&0x03);
#ifdef OSD2_USE_CQ
	if(OSD2_IS_ENABLE) {
		_osd2_cq_cfg(OSD_CTRL, (0x80|freq|duty));		// OSD1 enable
	} else {
		osd2_cfg_wr(OSD_CTRL, (0x80|freq|duty));		// OSD1 enable
	}
	osd2_update();
#else
	osd2_cfg_wr(OSD_CTRL, (0x80|freq|duty));		// OSD1 enable
	osd2_update();
#endif
}

/**
 * @func    osd2_set_access_mode_LSB
 * @brief	set OSD2 access LSB mode at data port.
 * @param   none
 * @return  none
 * @note	just access LSB of the OSDRAM
 * 			when write data through data port
 */
void
osd2_set_access_mode_LSB (void)
{
	osd2_cfg_wr(OSD_CTRL, (osd2_cfg_read(OSD_CTRL) & 0xCF) | 0x20);
}

/**
 * @func    osd2_set_access_mode_MSB
 * @brief	set OSD2 access MSB mode at data port.
 * @param   none
 * @return  none
 * @note	just access MSB of the OSDRAM
 * 			when write data through data port
 */
void
osd2_set_access_mode_MSB (void)
{
	osd2_cfg_wr(OSD_CTRL, osd2_cfg_read(OSD_CTRL) | 0x30);
}

/**
 * @func    osd2_set_access_mode_word
 * @brief	set OSD2 access word mode(default) at data port.
 * @param   none
 * @return  none
 * @note	just access word of the OSDRAM
 * 			when write data through data port
 */
void
osd2_set_access_mode_word (void)
{
	osd2_cfg_wr(OSD_CTRL, osd2_cfg_read(OSD_CTRL) & 0xCF);
}

/**
 * @func    osd2_set_menu_start_end
 * @brief	set OSD2 MENU of start and end base address.
 * @param   star_addr	MENU of start base address
 * 			end_addr	MENU of end base address
 * @return  none
 */
void
osd2_set_menu_start_end (unsigned short star_addr, unsigned short end_addr)
{
#ifdef OSD2_USE_CQ
	if(OSD2_IS_ENABLE) {
		_osd2_cq_cfg( OSD_MENU1_START_ADR_L, (star_addr & 0xff));
		_osd2_cq_cfg( OSD_MENU1_START_ADR_M, ((star_addr>>8) & 0xff));
		_osd2_cq_cfg( OSD_MENU1_END_ADR_L, (end_addr & 0xff));
		_osd2_cq_cfg( OSD_MENU1_END_ADR_M, ((end_addr>>8) & 0xff));
		osd2_update();

	} else {
		osd2_cfg_wr( OSD_MENU1_START_ADR_L, (star_addr & 0xff));
		osd2_cfg_wr( OSD_MENU1_START_ADR_M, ((star_addr>>8) & 0xff));
		osd2_cfg_wr( OSD_MENU1_END_ADR_L, (end_addr & 0xff));
		osd2_cfg_wr( OSD_MENU1_END_ADR_M, ((end_addr>>8) & 0xff));

		if((IC_READBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG) & SHADOW_CTRL_CONF_DEFAULT))
			IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
	}
#else
	osd2_cfg_wr( OSD_MENU1_START_ADR_L, (star_addr & 0xff));
	osd2_cfg_wr( OSD_MENU1_START_ADR_M, ((star_addr>>8) & 0xff));
	osd2_cfg_wr( OSD_MENU1_END_ADR_L, (end_addr & 0xff));
	osd2_cfg_wr( OSD_MENU1_END_ADR_M, ((end_addr>>8) & 0xff));

	if((IC_READBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG) & SHADOW_CTRL_CONF_DEFAULT))
		IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_CONF);
#endif
}

/**
 * @func    osd2_com_color_atb
 * @brief	combine FG index, BG index and Blink flag
 * 			to color attribute(MSB).
 * @param   fg		FG index
 * 			bg		BG index
 * 			blink	Blink flag
 * @return  color attribute(character format of MSB)
 */
unsigned char
osd2_com_color_atb (unsigned char fg, unsigned char bg, unsigned char blink)
{
	/*
	 * Character Format
	 * 	[15:14]	BG_Color[1:0]
	 * 	[13]	Blink
	 * 	[12:9]	FG_Color[3:0]
	 * 	[8:0]	Char_Index[8:0]
	 */
	unsigned char color = 0;

	color = (fg << 1) | (bg << 5) | (blink << 4);

	return color;
}

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
void
osd2_menu_write_byte (unsigned char x, unsigned char y, unsigned char dat,
					  unsigned short addr, unsigned char width)
{							
#ifdef OSD2_USE_CQ
	if (OSD2_IS_ENABLE) {
		_osd2_set_loca_addr(x, y, addr, width);
		cq_write_byte(CQ_P0, OSD_RAM_AL_REG, dat);
		osd2_update();
	} else {
		_osd2_set_loca_addr(x, y, addr, width);
		OSD_SET_RAM_DATA_BYTE(dat);
	}
#else
	osd2_set_location_addr(x, y, addr, width);
    OSD_SET_RAM_DATA_BYTE(dat);
#endif
}

/**
 * @func    osd2_menu_write
 * @brief	write a word(font&color) to OSDRAM with MENU of location.
 * @param   x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			dat		character format of data(word, 2bytes)
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
void
osd2_menu_write (unsigned char x, unsigned char y, unsigned short dat,
				 unsigned short addr, unsigned char width)
{										   
#ifdef OSD2_USE_CQ
//	if (OSD2_IS_ENABLE) {
		_osd2_set_loca_addr(x, y, addr, width);
		cq_write_word(CQ_P0, OSD_RAM_DL_REG, dat, 0);
		osd2_update();
//	} else {
//		_osd2_set_loca_addr(x, y, addr, width);
//		OSD_SET_RAM_DATA(dat);
//	}
#else
	osd2_set_location_addr(x, y, addr, width);
	OSD_SET_RAM_DATA(dat);
#endif
}

/**
 * @func    osd2_menu_write_byte
 * @brief	fill a word(font&color) to OSDRAM with MENU a ROW(line).
 * @param   y		y coord. of the MENU, unit is font
 * 			dat		character format of data(word, 2bytes)
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
void
osd2_menu_write_line (unsigned char y, unsigned short dat,
					  unsigned short addr, unsigned char width)
{
//	int w = 0;

#ifdef OSD2_USE_CQ
//	if (OSD2_IS_ENABLE) {
//		_osd2_set_loca_addr(0, y, addr, width);
//		for(w=0; w < width; w++) {
//			cq_write_word(CQ_P0, OSD_RAM_AL_REG, dat, 0);
//		}
		_osd2_set_menu_addr(addr);
		_osd2_cq_burst(y, 0, width, 1, ((dat>>8)&0xFF), (dat&0xFF), 0);
		osd2_update();
//	} else {
//		osd2_set_location_addr(0, y, addr, width);
//		for(w=0; w < width; w++) {
//			OSD_SET_RAM_DATA(dat);
//		}
//	}
#else
	osd2_set_location_addr(0, y, addr, width);
	for(w=0; w < width; w++) {
		OSD_SET_RAM_DATA(dat);
	}
#endif
}

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
void
osd2_menu_write_block (unsigned char x, unsigned char y, unsigned char width,
				  	   unsigned char height, unsigned short dat,
				  	   unsigned short addr, unsigned char m_width)
{
//    int w, h = 0;

#ifdef OSD2_USE_CQ
//	if (OSD2_IS_ENABLE) {

//		for(h=0; h < height; h++) {
//			_osd2_set_loca_addr(x, y+h, addr, width);
//			for(w=0; w < width; w++) {
//				cq_write_word(CQ_P0, OSD_RAM_AL_REG, dat, 0);
//			}
//		}
		_osd2_set_menu_addr(addr);
		_osd2_set_menu_width(m_width);
    	_osd2_cq_burst(y, x, width, height, ((dat>>8)&0xFF), (dat&0xFF), 0);
		osd2_update();
//	} else {
//	    for(h=0; h < height; h++) {
//	        osd2_set_location_addr(x, y+h, addr, m_width);
//	        for(w=0; w< width; w++) {
//	        	OSD_SET_RAM_DATA(dat);
//	        }
//	    }
//	}
#else
    for(h=0; h < height; h++) {
        osd2_set_location_addr(x, y+h, addr, m_width);
        for(w=0; w< width; w++) {
        	OSD_SET_RAM_DATA(dat);
        }
    }
#endif
}

/**
 * @func    osd2_clear_char
 * @brief	put a blank font(0) to OSDRAM with MENU of location.
 * @param   y		y coord. of the MENU, unit is font
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
void
osd2_clear_char (unsigned char x, unsigned char y, unsigned short addr, unsigned char width)
{
	/* DEFALUT BLANK FONT INDEX IS 0 */
	osd2_menu_write(x, y, 0, addr, width);
}

/**
 * @func    osd2_clear_line
 * @brief	fill a blank font(0) to OSDRAM with MENU a ROW(line).
 * @param   y		y coord. of the MENU, unit is font
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * @return  none
 */
void
osd2_clear_line (unsigned char y, unsigned short addr, unsigned char width)
{
	/* DEFALUT BLANK FONT INDEX IS 0 */
	osd2_menu_write_line(y, 0, addr, width);
}

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
void
osd2_clear_block (unsigned char x, unsigned char y, unsigned char width,
				  unsigned char height, unsigned short addr, unsigned char m_width)
{
    /* DEFALUT BLANK FONT INDEX IS 0 */
	osd2_menu_write_block(x, y, width, height, 0, addr, m_width);
}

/**
 * @func    osd2_clear_menu
 * @brief	fill a blank font(0) to OSDRAM with MENU a block.
 * @param   addr	MENU of base address
 * 			width	MENU of width, unit is font
 * 			height	MENU of height, unit is font
 * @return  none
 */
void
osd2_clear_menu (unsigned short addr, unsigned char width, unsigned char height)
{
    /* DEFALUT BLANK FONT INDEX IS 0 */
	osd2_clear_block(0, 0, width, height, addr, width);
}

/**
 * @func    osd2_menu_start_h
 * @brief	set a specified ROW of H start position of the MENU.
 * @param   x		x coord. of the display, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			row		specified ROW of the MENU
 * @return  none
 */
#define OSD2_RATTV	0x4000	// RATT_V of Attribute ID
#define OSD2_RATTH	0x6000	// RATT_H of Attribute ID
#define OSD2_RATTA	0xD000	// RATT_A of Attribute ID
void
osd2_menu_start_h (unsigned short x, unsigned short addr,
				  unsigned char width, unsigned char row)
{
	x &= 0x07FF;	// HStart[10:0]

#ifdef OSD2_USE_CQ
	addr += ((width + 4)*row) + 1;	// RATT_H at every ROW of second entry
	cq_write_word (CQ_P0, OSD_RAM_AL_REG, addr, 0);
	cq_write_word (CQ_P0, OSD_RAM_DL_REG, (OSD2_RATTH|x), 0);
	if (cq_flush_vde () < 0)
		ERROR ("cq_flush()\n");
#else
	addr += ((width + 4)*row) + 1;	// RATT_H at every ROW of second entry
	OSD_SET_RAM_ADDR(addr);
	OSD_SET_RAM_DATA((OSD2_RATTH|x));
#endif

#ifdef	DEBUG_OSD2
	dbg(0, "osd_menu_start_h(%u, 0x%X, %u, %u) RATTH = 0x%X \n", x, addr, width, row, (OSD2_RATTH|x));
#endif
}

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
void
osd2_menu_location (unsigned short x, unsigned short y, unsigned short addr,
				   unsigned char width, unsigned char height)
{
	int i;

	x &= 0x07FF;
	y &= 0x07FF;

#ifdef OSD2_USE_CQ
	for(i=0; i < height; i++) {
		cq_write_word (CQ_P0, OSD_RAM_AL_REG, addr, 0);
		if(i==0)
			cq_write_word (CQ_P0, OSD_RAM_DL_REG, (OSD2_RATTV|y), 0);
		else
			cq_write_word (CQ_P0, OSD_RAM_DL_REG, 0xE000, 0);
		cq_write_word (CQ_P0, OSD_RAM_DL_REG, (OSD2_RATTH|x), 0);
		cq_write_word (CQ_P0, OSD_RAM_DL_REG, OSD2_RATTA, 0);
		cq_write_word (CQ_P0, OSD_RAM_DL_REG, width, 0);
#ifdef	DEBUG_OSD2
		dbg(0, "osd_menu_location(%u, %u, 0x%X, %u, %u) RATTV = 0x%X, RATTH = 0x%X \n",
			x, y, addr, width, height, (OSD2_RATTV|y), (OSD2_RATTH|x));
#endif
		addr += (width + 4);
	}
	if (cq_flush_vde () < 0)
		ERROR ("cq_flush()\n");
#else
	for(i=0; i < height; i++) {
		OSD_SET_RAM_ADDR(addr);
		if(i==0)
			OSD_SET_RAM_DATA((OSD2_RATTV|y));
		else
			OSD_SET_RAM_DATA(0xE000);		// ROW Attribute Dummy Type
		OSD_SET_RAM_DATA((OSD2_RATTH|x));
		OSD_SET_RAM_DATA(OSD2_RATTA);
		OSD_SET_RAM_DATA(width);
#ifdef	DEBUG_OSD2
		dbg(0, "osd2_menu_location(%u, %u, 0x%X, %u, %u) RATTV = 0x%X, RATTH = 0x%X \n",
			x, y, addr, width, height, (OSD2_RATTV|y), (OSD2_RATTH|x));
		osd2_oram_dump(addr, 4);
#endif
		addr += (width + 4);
	}
#endif
}

/**
 * @func    mem2oram
 * @brief   write data to OSDRAM from memory.
 * @param   base_address	NOR Flash of base address
 * 			oram_address	OSDRAM of point address
 * 			length			DMA of length
 * @return  0 if DAMA done, 1 if timeout
 */
int
mem2oram (unsigned long base_address, unsigned short oram_address, unsigned int length)
{
	int i = 0;
	char *src = (char*)base_address;

#ifdef OSD2_USE_CQ
	if (OSD2_IS_ENABLE) {
		/* set OSDRAM of point address */
		_osd2_cq_oram(oram_address, src, length);
		osd2_update();
	} else {
		/* set OSDRAM of point address */
		osd2_set_ram_addr(oram_address);
		/* write data to OSDRAM through data port and auto inc.  */
		for(i=0; i < (length/2); i++) {
			/* word access, LSB first, then MSB byte */
			IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, (unsigned char)src[i*2+0]);
			IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, (unsigned char)src[i*2+1]);
		}
	}
#else
	/* set OSDRAM of point address */
	osd2_set_ram_addr(oram_address);
	/* write data to OSDRAM through data port and auto inc.  */
	for(i=0; i < (length/2); i++) {
		/* word access, LSB first, then MSB byte */
		IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, (unsigned char)src[i*2+0]);
		IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, (unsigned char)src[i*2+1]);
	}
#endif

	return 0;
}

/**
 * @func    osd2_oram_dump
 * @brief	set OSD2 MENU of location.
 * @param   addr	OSDRAM of base address
 * 			length	dump data of length
 * @return  none
 */
void
osd2_oram_dump (unsigned short addr, unsigned short length)
{		
	unsigned short count, idx, j, i;
	unsigned short read[8];
	unsigned short low, high;

	if (length < 16) {
		length = 16;		// once time is 16 bytes
	} else if (length > OSD2_RAM_MAX_SIZE) {
		WARN("length(%d) > OSDRAM of max size(%d)!\n", length, OSD2_RAM_MAX_SIZE);
		length = OSD2_RAM_MAX_SIZE;
	}
	if((length + addr) > OSD2_RAM_MAX_SIZE) {
		WARN("start address + length (%d) > OSDRAM of max size(%d)!\n",
			(length + addr), OSD2_RAM_MAX_SIZE);
		length = OSD2_RAM_MAX_SIZE - addr;
	}

	addr = (addr>>4) << 4;	// alignment to 4 bytes boundary
								  
	DBG_PRINT("\n                 "); 
	for(j = 0; j < 16; j++) {
		DBG_PRINT("  %1hXh ", j);	
	}							
	DBG_PRINT("\n");

	for( idx = addr, i = 0; idx < addr + length; idx+=8, i++)
	{
		osd2_set_ram_addr(idx);
		/* trigger OSDRAM read back */
		osd2_cfg_wr(0x48, 0x80);

		count = 0;
		while(!(IC_READBYTE(TWIC_P0, OSD_RAM_AL_REG) & 0x04)) {
			/* check OSDRAM read back finish */
			count++;
			if(count >= 65535) {
				/* time out! */
				ERROR("osd2_dump_memory: time out.\n");	
				break;
			}
		}	

		for(j = 0; j < 8; j++) {
			low = IC_ReadByte(TWIC_P0, OSD_RAM_DL_REG);
			high = IC_ReadByte(TWIC_P0, OSD_RAM_DH_REG);
			read[j] = low + (high<<8);	 	
		}
													
		if(i%2 == 0) {
			DBG_PRINT("ORAM Dump %04hXh:  ", idx);
		}
		for(j = 0; j < 8; j++) {
			DBG_PRINT("%04hX ", read[j]);		
		} 
		if(i%2 == 1) {
			DBG_PRINT("\n");
		}
	}

	DBG_PRINT("\n");
}



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
void
_osd2_bmp_pos (unsigned short hstar, unsigned short vstar)
{
	/* Set BMP Horizintal Start Position */
	osd2_cfg_wr(OSD_BMP_HSTART_L, (unsigned char)(hstar & 0xff));
	osd2_cfg_wr(OSD_BMP_HSTART_M, (unsigned char)(hstar>>8));
	/* Set BMP Vertical Start Position */
	osd2_cfg_wr(OSD_BMP_VSTART_L, (unsigned char)(vstar & 0xff));
	osd2_cfg_wr(OSD_BMP_VSTART_M, (unsigned char)(vstar>>8));
}

/**
 * @func    osd2_bmp_star_position
 * @brief	set OSD2 BMP of X,Y coord. position of the display.
 * @param   hstar	x coord. of the display, unit is pixel
 * 			vstar	y coord. of the display, unit is pixel
 * @return  none
 * @note	just BMP mode used only and OSDTool not support
 */
void
osd2_bmp_star_position (unsigned short hstart, unsigned short vstart)
{
	_osd2_bmp_pos(hstart, vstart);
	//osd2_update();
	if(OSD2_IS_ENABLE) {
		IC_WRITEBYTE(TWIC_P0, SHADOW_CTRL_CONF_REG, 0x00);		// Shadow Update
	}
}

/**
 * @func    osd2_bmp_disable
 * @brief	disable OSD2 BMP.
 * @param   none
 * @return  none
 * @note	just BMP mode used only and OSDTool not support
 */
void
osd2_bmp_disable (void)
{
	osd2_cfg_wr(OSD_BMP_CTRL, 0x00); // disable BMP
}

#if 0
/**
 * @func    _twf_get_bmp_offset
 * @brief	.
 * @param   none
 * @return  none
 */
unsigned long
_twf_get_bmp_offset (unsigned long address, unsigned short uc)
{
	unsigned long gnum, i, j, fac;
	unsigned long bmpo;
    unsigned char c, gap;
	unsigned short fcode = 0, imin = 0, imax = 0, imid = 0;
	unsigned char count = 50;

	c = spi_read_byte(address + 0x4fL);
    gap = 6L;
    if (c == 0L)
        gap = 5L;

    gnum = 0L;
    for(i=0L, fac=1L; i<4L; i++, fac*=256L) {
    	c = spi_read_byte(address +0x58L + i);
        gnum+=c*fac;
    }

	imin = 1;
	imax = gnum;

	while (imax >= imin && count--) {
		imid = (imin+imax)/2L;

		fcode = spi_read_byte(address + 0x5dL + (imid-1L)*gap);

		if(gap == 6)
			fcode += (spi_read_byte(address + 0x5eL + (imid-1L)*gap)<<8L);

		if(fcode <  uc) {
			imin = imid + 1L;
		}
		else if(fcode > uc ) {
			imax = imid - 1L;
		}
		else {
			break;
		}
	}

	if(fcode != uc) {
		return 0;
	}

    bmpo = 0;
    for(j=0L, fac=1L; j<4L; j++, fac*=256L) {
    	c = spi_read_byte(address + 0x5dL + (imid-1L)*gap + (gap - 4L) + j);
        bmpo+=(c*fac);
    }

    bmpo = 0x5dL + gnum*gap + bmpo;
    bmpo += address;

    return bmpo;
}
#else
/**
 * @func    _twf_get_bmp_offset
 * @brief	.
 * @param   none
 * @return  none
 */
unsigned long
_twf_get_bmp_offset (unsigned long address, unsigned short uc)
{
	unsigned long gnum, i, j, fac;
	unsigned long bmpo;
    unsigned char c, gap;
	unsigned short fcode = 0, imin = 0, imax = 0, imid = 0;
	unsigned char count = 50;
	unsigned char *src = (unsigned char*) address;

	c = (unsigned char)src[0x4fL];
    gap = 6L;
    if (c == 0L)
        gap = 5L;

    gnum = 0L;
    for(i=0L, fac=1L; i<4L; i++, fac*=256L) {
        c = (unsigned char)src[0x58L + i];
        gnum+=c*fac;
    }

	imin = 1;
	imax = gnum;

	while (imax >= imin && count--) {
		imid = (imin+imax)/2L;

		fcode = (unsigned char)src[0x5dL + (imid-1L)*gap];

		if(gap == 6)
			fcode += ((unsigned char)src[0x5eL + (imid-1L)*gap]<<8L);

		if(fcode <  uc) {
			imin = imid + 1L;
		}
		else if(fcode > uc ) {
			imax = imid - 1L;
		}
		else {
			break;
		}
	}

	if(fcode != uc) {
		return 0;
	}

    bmpo = 0;
    for(j=0L, fac=1L; j<4L; j++, fac*=256L) {
        c = (unsigned char)src[0x5dL + (imid-1L)*gap + (gap - 4L) + j];
        bmpo+=(c*fac);
    }

    bmpo = 0x5dL + gnum*gap + bmpo;
    //bmpo += address;

    return bmpo;
}
#endif
#if 0
/**
 * @func    osd2_put_twf_char_to_oram
 * @brief	.
 * @param   none
 * @return  none
 */
unsigned char 
osd2_put_twf_char_to_oram (unsigned long flash_address, unsigned long file_address,
						   unsigned short oram_address, unsigned short uc, unsigned short byte_size, unsigned char isHSA)
{
	unsigned long bmpo;	
	unsigned char w_count; 
	  
	bmpo = _twf_get_bmp_offset(file_address, uc);
	
	if(bmpo == 0) {
        ERROR("twf_put_char_to_oram: Unexpected character.\n");
        return 0;
    }
	
	w_count = isHSA ? ospi_read_byte(flash_address + bmpo) : 1; 
	spi_dma2oram(flash_address + bmpo+5, oram_address, byte_size*w_count);

	return w_count;
}
#endif
/**
 * @func    osd2_put_twf_char_to_oram
 * @brief	.
 * @param   none
 * @return  none
 */
unsigned char 
osd2_put_twf_char_to_oram (unsigned long flash_address, unsigned long file_address,
						   unsigned short oram_address, unsigned short uc, unsigned short byte_size, unsigned char isHSA)
{
	
	unsigned long bmpo;
	unsigned char w_count; 
	if(config_osd2_only){
		bmpo = _twf_get_bmp_offset(file_address, uc);
		if(bmpo == 0){
			ERROR ("twf_put_char_to_oram: Unexpected character.\n");
			return 0;
		}
		w_count = isHSA ? spi_read_byte(file_address+bmpo) : 1; 
		mem2oram(file_address+bmpo+5, oram_address, byte_size*w_count);
		return w_count;
	}else{
		bmpo = _twf_get_bmp_offset(file_address, uc);
	
		if(bmpo == 0) {
			ERROR("twf_put_char_to_oram: Unexpected character.\n");
			return 0;
		}
		
		w_count = isHSA ? ospi_read_byte(flash_address + bmpo) : 1; 
		spi_dma2oram(flash_address + bmpo+5, oram_address, byte_size*w_count);

		return w_count;
	}
}
