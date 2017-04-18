/**
 *  @file   spiosd.c
 *  @brief  terawins SPIOSD control function
 *  $Id: spiosd.c,v 1.9 2014/08/11 05:01:59 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.9 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/11/22  yc 	New file.
 *
 */

#include "reg_tw.h"
#include "iccontrol.h"
#include "spiosd.h"
#include "debug.h"

//#ifdef SPIOSD_USE_CQ	/* for root control cq write, maybe someday need it */
#include "./drivers/cq/cq.h"
#define SPIOSD_USE_CQ
//#endif
//#define DEBUG_SOSD	// enable debug massage

#define ENABLE	1
#define DISABLE	0

/* system global variable for SPIOSD status */
static unsigned char SOSD_IS_ENABLE= DISABLE;
static unsigned short HSTART_OFFSET;
static unsigned short VSTART_OFFSET;
static unsigned short SHSTART_OFFSET;
static unsigned short SVSTART_OFFSET;

/**
 * @func    sosd_hvoffset_init
 * @brief   setting SPIOSD image H/V start of offset.
 * @param   h_offset	horizontal of start offset
 * 			v_offset	vertical of start offset
 * @return  none
 */
void
sosd_hvoffset_init (unsigned short h_offset, unsigned short v_offset)
{
	HSTART_OFFSET=h_offset;
	VSTART_OFFSET=v_offset;
}

/**
 * @func    sosd_sp_hvoffset_init
 * @brief   setting SPIOSD sprite H/V start of offset.
 * @param   h_offset	horizontal of start offset
 * 			v_offset	vertical of start offset
 * @return  none
 */
void
sosd_sp_hvoffset_init (unsigned short h_offset, unsigned short v_offset)
{
	SHSTART_OFFSET=h_offset;
	SVSTART_OFFSET=v_offset;
}

/**
 * @func    _sosd_emu_addr
 * @brief   setting SPIOSD emulation of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
_sosd_emu_addr (unsigned long base_address)
{
#ifdef SPIOSD_USE_CQ
    int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_EMU_ADDR_REG1, (base_address&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_EMU_ADDR_REG2, ((base_address>>8)&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_EMU_ADDR_REG3, ((base_address>>16)&0x000000FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_EMU_ADDR_REG1, (base_address&0x000000FF));		// AL
	IC_WriteByte(TWIC_P3, SOSD_EMU_ADDR_REG2, ((base_address>>8)&0x000000FF));	// AM
	IC_WriteByte(TWIC_P3, SOSD_EMU_ADDR_REG3, ((base_address>>16)&0x000000FF));	// AH
#endif
#ifdef DEBUG_SOSD
    dbg(0, "sosd_emu_addr 0x%lX \n", base_address);
#endif
}

/**
 * @func    _sosd_img_addr
 * @brief   setting SPIOSD image of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
_sosd_img_addr (unsigned long base_address)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_IMG_ADDR_REG1, (base_address&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_IMG_ADDR_REG2, ((base_address>>8)&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_IMG_ADDR_REG3, ((base_address>>16)&0x000000FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_IMG_ADDR_REG1, (base_address&0x000000FF));		// AL
	IC_WriteByte(TWIC_P3, SOSD_IMG_ADDR_REG2, ((base_address>>8)&0x000000FF));	// AM
	IC_WriteByte(TWIC_P3, SOSD_IMG_ADDR_REG3, ((base_address>>16)&0x000000FF));	// AH
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_img_addr 0x%lX \n", base_address);
#endif
}

/**
 * @func    _sosd_spr_addr
 * @brief   setting SPIOSD sprite of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
_sosd_spr_addr (unsigned long base_address)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SPRITE_ADDR_REG1, (base_address&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_SPRITE_ADDR_REG2, ((base_address>>8)&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_SPRITE_ADDR_REG3, ((base_address>>16)&0x000000FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SPRITE_ADDR_REG1, (base_address&0x000000FF));			// AL
	IC_WriteByte(TWIC_P3, SOSD_SPRITE_ADDR_REG2, ((base_address>>8)&0x000000FF));	// AM
	IC_WriteByte(TWIC_P3, SOSD_SPRITE_ADDR_REG3, ((base_address>>16)&0x000000FF));	// AH
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_addr 0x%lX \n", base_address);
#endif
}

/**
 * @func    _sosd_pLUT_addr
 * @brief   setting SPIOSD pLUT of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void 
_sosd_pLUT_addr (unsigned long base_address)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_LUT_ADDR_REG1, (base_address&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_LUT_ADDR_REG2, ((base_address>>8)&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_LUT_ADDR_REG3, ((base_address>>16)&0x000000FF));
	if(rc<0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_LUT_ADDR_REG1, (base_address&0x000000FF));		// AL
	IC_WriteByte(TWIC_P3, SOSD_LUT_ADDR_REG2, ((base_address>>8)&0x000000FF));	// AM
	IC_WriteByte(TWIC_P3, SOSD_LUT_ADDR_REG3, ((base_address>>16)&0x000000FF));	// AH
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_pLUT_addr 0x%lX \n", base_address);
#endif
}

/**
 * @func    _sosd_sLUT_addr
 * @brief   setting SPIOSD sLUT of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void 
_sosd_sLUT_addr (unsigned long base_address)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SLUT_ADDR_REG1, (base_address&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_SLUT_ADDR_REG2, ((base_address>>8)&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_SLUT_ADDR_REG3, ((base_address>>16)&0x000000FF));
	if(rc<0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SLUT_ADDR_REG1, (base_address&0x000000FF));			// AL
	IC_WriteByte(TWIC_P3, SOSD_SLUT_ADDR_REG2, ((base_address>>8)&0x000000FF));		// AM
	IC_WriteByte(TWIC_P3, SOSD_SLUT_ADDR_REG3, ((base_address>>16)&0x000000FF));	// AH
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_sLUT_addr 0x%lX \n", base_address);
#endif
}

/**
 * @func    _sosd_hspos
 * @brief   setting SPIOSD image of h start position.
 * @param   h_pos	horizontal of position
 * @return  none
 */
void 
_sosd_hspos (unsigned short h_pos)
{
#ifdef SPIOSD_USE_CQ
    int rc = 0;
	h_pos*= 2;
	h_pos += HSTART_OFFSET;
	rc = cq_write_byte (CQ_P3, SOSD_HSTAR_REG1, (h_pos&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_HSTAR_REG2, ((h_pos&0x7FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	h_pos*= 2;
	h_pos+=HSTART_OFFSET;
	IC_WriteByte(TWIC_P3, SOSD_HSTAR_REG1, (h_pos&0xFF));			// L
	IC_WriteByte(TWIC_P3, SOSD_HSTAR_REG2, ((h_pos&0x7FF)>>8));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_hspos %u \n", h_pos);
#endif
}

/**
 * @func    sosd_vspos
 * @brief   setting SPIOSD image of v start position.
 * @param   v_pos	vertical of position
 * @return  none
 */
void 
_sosd_vspos (unsigned short v_pos)
{	
#ifdef SPIOSD_USE_CQ
    int rc = 0;
	v_pos += VSTART_OFFSET;
	rc = cq_write_byte (CQ_P3, SOSD_VSTAR_REG1, (v_pos&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_VSTAR_REG2, ((v_pos&0x3FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	v_pos += VSTART_OFFSET;
	IC_WriteByte(TWIC_P3, SOSD_VSTAR_REG1, (v_pos&0xFF));			// L
	IC_WriteByte(TWIC_P3, SOSD_VSTAR_REG2, ((v_pos&0x3FF)>>8));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_vspos %u \n", v_pos);
#endif
}

/**
 * @func    _sosd_spr_hspos
 * @brief   setting SPIOSD sprite of h start position.
 * @param   h_pos	horizontal of position
 * @return  none
 */
void 
_sosd_sp_hspos (unsigned short h_pos)
{	
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	h_pos += SHSTART_OFFSET;
	rc = cq_write_byte (CQ_P3, SOSD_SP_HSTAR_REG1, (h_pos&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_SP_HSTAR_REG2, ((h_pos&0x7FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	h_pos+=SHSTART_OFFSET;
	IC_WriteByte(TWIC_P3, SOSD_SP_HSTAR_REG1, (h_pos&0xFF));				// L
	IC_WriteByte(TWIC_P3, SOSD_SP_HSTAR_REG2, ((h_pos&0xFFF)>>8));		// H
#endif
}

/**
 * @func    _sosd_spr_vspos
 * @brief   setting SPIOSD sprite of v start position.
 * @param   v_pos	vertical of position
 * @return  none
 */
void 
_sosd_sp_vspos (unsigned short v_pos)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	v_pos += SVSTART_OFFSET;
	rc = cq_write_byte (CQ_P3, SOSD_SP_VSTAR_REG1, (v_pos&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_SP_VSTAR_REG2, ((v_pos&0x7FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	v_pos+= SVSTART_OFFSET;
	IC_WriteByte(TWIC_P3, SOSD_SP_VSTAR_REG1, (v_pos&0xFF));			// L
	IC_WriteByte(TWIC_P3, SOSD_SP_VSTAR_REG2, ((v_pos&0x3FF)>>8));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_vspos %u \n", v_pos);
#endif
}

/**
 * @func    _sosd_img_loca
 * @brief   setting SPIOSD image of location.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
void 
_sosd_img_loca (unsigned short x, unsigned short y)
{
    _sosd_hspos(x);
    _sosd_vspos(y);
#ifdef DEBUG_SOSD   
    dbg(0, "_sosd_img_loca %u %u \n", x, y);
#endif
}

/**
 * @func    _sosd_spr_loca
 * @brief   setting SPIOSD sprite of location.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
void 
_sosd_spr_loca (unsigned short x, unsigned short y)
{
    if(IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(SOSD_TD_EN))
		x += 4;
	_sosd_sp_hspos(x);
    _sosd_sp_vspos(y);
#ifdef DEBUG_SOSD   
    dbg(0, "_sosd_spr_loca %u %u \n", x, y);
#endif
}

/**
 * @func    _sosd_td_loca
 * @brief   setting SPIOSD TWBC of location.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
void
_sosd_td_loca (unsigned short x, unsigned short y)
{
#ifndef SPIOSD_DCLK_1X
    x+= 9;
	y+= 2;
#else
	x+= 16;
	y+= 2;
#endif

	_sosd_hspos(x);
    _sosd_vspos(y);
#ifdef DEBUG_SOSD   
    dbg(0, "_sosd_td_loca %u %u \n", x, y);
#endif
}

/**
 * @func    _sosd_img_window
 * @brief   setting SPIOSD image of window size.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
void 
_sosd_img_window (unsigned short width, unsigned short height)
{
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_img_window %u, %u \n", width, height);
#endif
#ifdef SPIOSD_USE_CQ
    int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_HSIZE_REG1, (width&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_HSIZE_REG2, ((width&0x7FF)>>8));
	rc = cq_write_byte (CQ_P3, SOSD_VSIZE_REG1, (height&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_VSIZE_REG2, ((height&0x7FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_HSIZE_REG1, (width&0x00FF));				// L
	IC_WriteByte(TWIC_P3, SOSD_HSIZE_REG2, ((width>>8)&0x00FF));		// H
	IC_WriteByte(TWIC_P3, SOSD_VSIZE_REG1, (height&0x00FF));			// L
	IC_WriteByte(TWIC_P3, SOSD_VSIZE_REG2, ((height>>8)&0x00FF));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_img_window %u, %u \n", width, height);
#endif
}

/**
 * @func    _sosd_spr_window
 * @brief   setting SPIOSD sprite of window size.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
void 
_sosd_spr_window (unsigned char width, unsigned short height)
{
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_window %u, %u \n", width, height);
#endif
#ifdef SPIOSD_USE_CQ
    int rc= 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_HSIZE_REG, width);
	rc = cq_write_byte (CQ_P3, SOSD_SP_VSIZE_REG1, (height&0xFF));
	rc = cq_write_byte (CQ_P3, SOSD_SP_VSIZE_REG2, ((height&0x7FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_SP_HSIZE_REG, width);
	IC_WriteByte(TWIC_P3, SOSD_SP_VSIZE_REG1, (height&0x00FF));				// L
	IC_WriteByte(TWIC_P3, SOSD_SP_VSIZE_REG2, ((height>>8)&0x00FF));			// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_window %u, %u \n", width, height);
#endif
}

/**
 * @func    _sosd_td_window
 * @brief   setting SPIOSD TWBC of window size.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
void 
_sosd_td_window (unsigned short width, unsigned short height)
{
//#ifndef SPIOSD_DCLK_1X
//	width += 9;
//#else
//	width += 11;
//#endif
	_sosd_img_window(width, height);
}

/**
 * @func    _sosd_img_ls
 * @brief   setting SPIOSD image of line store.
 * @param   line_store	store size
 * @return  none
 */
void 
_sosd_img_ls (unsigned short line_store)
{
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_img_ls %u \n", line_store);
#endif
#ifdef SPIOSD_USE_CQ
    int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_LJUMP_A_REG1, (line_store&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_LJUMP_A_REG2, ((line_store>>8)&0x00FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_LJUMP_A_REG1, (line_store&0x00FF));		// L
	IC_WriteByte(TWIC_P3, SOSD_LJUMP_A_REG2, ((line_store>>8)&0x00FF));	// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_img_ls %u \n", line_store);
#endif
}

/**
 * @func    _sosd_spr_ls
 * @brief   setting SPIOSD sprite of line store.
 * @param   line_store	store size
 * @return  none
 */
void 
_sosd_spr_ls (unsigned short line_store)
{
#ifdef DEBUG_SOSD   
    dbg(0, "_sosd_spr_ls %u \n", line_store);
#endif
#ifdef SPIOSD_USE_CQ
    int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_LJUMP_A_REG1, (line_store&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_SP_LJUMP_A_REG2, ((line_store>>8)&0x00FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SP_LJUMP_A_REG1, (line_store&0x00FF));        // L
	IC_WriteByte(TWIC_P3, SOSD_SP_LJUMP_A_REG2, ((line_store>>8)&0x00FF));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_ls %u \n", line_store);
#endif
}

/**
 * @func    _sosd_td_ls
 * @brief   setting SPIOSD TWBC of line store.
 * @param   line_store	store size
 * @return  none
 */
void 
_sosd_td_ls (unsigned short line_store)
{
	if(line_store%4)
		line_store += (4-(line_store%4));
	line_store = line_store/4;
	_sosd_img_ls(line_store);
}

/**
 * @func    sosd_set_emu_base_addrss
 * @brief   setting sOSD emulation of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
sosd_set_emu_base_addrss (unsigned long base_address)
{
	_sosd_emu_addr(base_address);
	sosd_update();		
}

/**
 * @func    sosd_set_img_base_address
 * @brief   setting sOSD image of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
sosd_set_img_base_address (unsigned long base_address)
{
	_sosd_img_addr(base_address);
	sosd_update();
}

/**
 * @func    sosd_set_spr_base_address
 * @brief   setting sOSD sprite of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
sosd_set_spr_base_address (unsigned long base_address)
{
	_sosd_spr_addr(base_address);
	sosd_update();
}

/**
 * @func    sosd_set_pLUT_base_address
 * @brief   setting sOSD pLUT of address and sOSD update.
 * @param   unsigned long base_address	NOR Flash of address
 * @return  none
 */
void
sosd_set_pLUT_base_address (unsigned long base_address)
{
	_sosd_pLUT_addr(base_address);
	sosd_update();
}

/**
 * @func    sosd_set_sLUT_base_address
 * @brief   setting sOSD SLUT of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
void
sosd_set_sLUT_base_address (unsigned long base_address)
{
	_sosd_sLUT_addr(base_address);
	sosd_update();
}

/**
 * @func    sosd_set_hspos
 * @brief   setting sOSD image of h position and sOSD update.
 * @param   h_pos	horizontal of start position
 * @return  none
 */
void
sosd_set_hspos (unsigned short h_pos)
{	
	_sosd_hspos(h_pos);
	sosd_update();
}

/**
 * @func    sosd_set_vspos
 * @brief   setting sOSD image of v position and sOSD update.
 * @param   v_pos	vertical of start position
 * @return  none
 */
void
sosd_set_vspos (unsigned short v_pos)
{	
	_sosd_vspos(v_pos);
	sosd_update();
}

/**
 * @func    sosd_set_spr_hspos
 * @brief   setting sOSD sprite of h position and sOSD update.
 * @param   h_pos	horizontal of start position
 * @return  none
 */
void
sosd_set_sp_hspos (unsigned short h_pos)
{	
	_sosd_sp_hspos(h_pos);
	sosd_update();
}

/**
 * @func    sosd_set_spr_vspos
 * @brief   setting sOSD sprite of v position and sOSD update.
 * @param   v_pos	vertical of start position
 * @return  none
 */
void
sosd_set_sp_vspos (unsigned short v_pos)
{
	_sosd_sp_vspos(v_pos);
	sosd_update();
}

/**
 * @func    sosd_set_img_location
 * @brief   setting sOSD image of location and sOSD update.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
void
sosd_set_img_location (unsigned short x, unsigned short y)
{
    _sosd_img_loca(x, y);
	sosd_update();
}

/**
 * @func    sosd_set_spr_location
 * @brief   setting sOSD sprite of location and sOSD update.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
void
sosd_set_spr_location (unsigned short x, unsigned short y)
{
    _sosd_spr_loca(x, y);
	sosd_update();
}

/**
 * @func    sosd_set_td_location
 * @brief   setting sOSD TWBC of location and sOSD update.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
void
sosd_set_td_location (unsigned short x, unsigned short y)
{
    _sosd_td_loca(x, y);
	sosd_update();
}

/**
 * @func    sosd_set_img_active_window
 * @brief   setting sOSD image of window and sOSD update.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
void
sosd_set_img_active_window (unsigned short width, unsigned short height)
{
    _sosd_img_window(width, height);
	sosd_update();
}

/**
 * @func    sosd_set_spr_active_window
 * @brief   setting sOSD sprite of window and sOSD update.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
void
sosd_set_spr_active_window (unsigned char width, unsigned short height)
{
    _sosd_spr_window(width, height);
	sosd_update();
}

/**
 * @func    sosd_set_td_active_window
 * @brief   setting sOSD TWBC of window and sOSD update.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
void
sosd_set_td_active_window (unsigned short width, unsigned short height)
{
    _sosd_td_window(width, height);
	sosd_update();
}

/**
 * @func    sosd_set_img_line_store
 * @brief   setting SPIOSD image of line store and sOSD update.
 * @param   line_store	store size
 * @return  none
 */
void
sosd_set_img_line_store (unsigned short line_store)
{
    _sosd_img_ls(line_store);
	sosd_update();
}

/**
 * @func    sosd_set_spr_line_store
 * @brief   setting SPIOSD sprite of line store and sOSD update.
 * @param   line_store	store size
 * @return  none
 */
void
sosd_set_spr_line_store (unsigned short line_store)
{
    _sosd_spr_ls(line_store);
	sosd_update();
}

/**
 * @func    sosd_set_td_line_store
 * @brief   setting SPIOSD TWBC of line store and sOSD update.
 * @param   line_store	store size
 * @return  none
 */
void
sosd_set_td_line_store (unsigned short line_store)
{
    _sosd_td_ls(line_store);
	sosd_update();
}

/**
 * @func    sosd_img_force_enable
 * @brief   set SOSD image of force alpha level and SOSD update
 * @param   level	alpha level: 0 ~ 0x40
 * @return  none
 */
void
sosd_img_force_ctrl (unsigned char level)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_FORCE_AB_REG, (IC_ReadByte(TWIC_P3, SOSD_FORCE_AB_REG)|((level>64?FORCE_ALPHA_MAX:level))));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_FORCE_AB_REG, (IC_ReadByte(TWIC_P3, SOSD_FORCE_AB_REG)|((level>64?FORCE_ALPHA_MAX:level))));
#endif
	sosd_update();	
}

/**
 * @func    sosd_img_force_enable
 * @brief   enable SOSD image of force alpha and SOSD update
 * @param   none
 * @return  none
 */
void
sosd_img_force_enable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_FORCE_AB_REG, (IC_ReadByte(TWIC_P3, SOSD_FORCE_AB_REG)|SOSD_FORCE_EN));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_FORCE_AB_REG, (IC_ReadByte(TWIC_P3, SOSD_FORCE_AB_REG)|SOSD_FORCE_EN));
#endif
	sosd_update();
}

/**
 * @func    sosd_img_force_disable
 * @brief   disable SOSD image of force alpha and SOSD update
 * @param   none
 * @return  none
 */
void
sosd_img_force_disable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_FORCE_AB_REG, (IC_ReadByte(TWIC_P3, SOSD_FORCE_AB_REG)&(~SOSD_FORCE_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_FORCE_AB_REG, (IC_ReadByte(TWIC_P3, SOSD_FORCE_AB_REG)&(~SOSD_FORCE_EN)));
#endif
	sosd_update();
}

/**
 * @func    _sosd_tdc_addr
 * @brief   set TWBC of Color address
 * @param   hsize	TWBC of Color address
 * @return  none
 */
void
_sosd_tdc_addr (unsigned long address)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_TD_COLOR_ADDR_REG1, (unsigned char)(address&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_TD_COLOR_ADDR_REG2, (unsigned char)((address>>8)&0x000000FF));
	rc = cq_write_byte (CQ_P3, SOSD_TD_COLOR_ADDR_REG3, (unsigned char)((address>>16)&0x000000FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_TD_COLOR_ADDR_REG1, (unsigned char)(address&0x000000FF));
	IC_WriteByte(TWIC_P3, SOSD_TD_COLOR_ADDR_REG2, (unsigned char)((address>>8)&0x000000FF));
	IC_WriteByte(TWIC_P3, SOSD_TD_COLOR_ADDR_REG3, (unsigned char)((address>>16)&0x000000FF));
#endif
#ifdef DEBUG_SOSD   
    dbg(0, "_sosd_tdc_addr 0x%lX \n", address);
#endif
}

/**
 * @func    _sosd_tdc_h_size
 * @brief   set TWBC of Color HSize
 * @param   hsize	TWBC of Color HSize
 * @return  none
 */
void
_sosd_tdc_h_size (unsigned short hsize)
{

#ifdef SPIOSD_USE_CQ
	int rc = 0;
	if(hsize%4) {
		hsize = ((hsize/4)*4);
		hsize += 8;
	} else
		hsize += 8;
	rc = cq_write_byte (CQ_P3, SOSD_TD_COLOR_HSIZE_REG1, (unsigned char)(hsize&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_TD_COLOR_HSIZE_REG2, (unsigned char)((hsize>>8)&0x00FF));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    if(hsize%4) {
		hsize += 4-(hsize%4);
		hsize += 8;
	} else
		hsize += 8;
	IC_WriteByte(TWIC_P3, SOSD_TD_COLOR_HSIZE_REG1, (unsigned char)(hsize&0x00FF));			// L
	IC_WriteByte(TWIC_P3, SOSD_TD_COLOR_HSIZE_REG2, (unsigned char)((hsize>>8)&0x00FF));	// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_tdc_h_size %u \n", hsize);
#endif
}

/**
 * @func    _sosd_td_spilt_mode_en
 * @brief   set enable TWBC of split mode
 * @param   none
 * @return  none
 */
void
_sosd_td_spilt_mode_en (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)|(SOSD_TD_COLOR_SPILT_MODE)|(SOSD_TD_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)|(SOSD_TD_COLOR_SPILT_MODE)|(SOSD_TD_EN)));
#endif
}

/**
 * @func    _sosd_td_spilt_mode_dis
 * @brief   set disable TWBC of split mode
 * @param   none
 * @return  none
 */
void
_sosd_td_spilt_mode_dis (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_COLOR_SPILT_MODE)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_COLOR_SPILT_MODE)));
#endif
 }

/**
 * @func    sosd_td_color_addr
 * @brief   set TWBC of Color address and SOSD update
 * @param   hsize	TWBC of Color address
 * @return  none
 */
void
sosd_td_color_addr (unsigned long address)
{
	_sosd_tdc_addr(address);
	sosd_update();
}

/**
 * @func    sosd_td_color_h_size
 * @brief   set TWBC of Color HSize and SOSD update
 * @param   hsize	TWBC of Color HSize
 * @return  none
 */
void
sosd_td_color_h_size (unsigned short hsize)
{
	_sosd_tdc_h_size(hsize);
	sosd_update();
}

/**
 * @func    sosd_td_spi_gap
 * @brief   unknown
 * @param   gap		gap
 * @return  none
 */
void
sosd_td_spi_gap (unsigned char gap)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_TD_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG) | (gap & SOSD_TD_SPI_GAP) ));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG) | (gap & SOSD_TD_SPI_GAP) ));
#endif
	sosd_update();
}

/**
 * @func    sosd_spr_force_ctrl
 * @brief   setting forced sprite alpha-blending value and sOSD update.
 * @param   level	0~16
 * @return  none
 */
void
sosd_spr_force_ctrl (unsigned char level)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)|(level&FORCE_ALPHA_MASK)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)|(level&FORCE_ALPHA_MASK)));
#endif
	sosd_update();
}

/**
 * @func    sosd_emu_trig
 * @brief   trig SPIOSD of emulation.
 * @param   none
 * @return  none
 */
void
sosd_emu_trig (void)
{
#ifdef SPIOSD_USE_CQ
	cq_write_byte_issue (CQ_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_EMU_TRIG|SOSD_SHADOW_UP, CQ_TRIGGER_VDE);
#else
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_EMU_TRIG|SOSD_SHADOW_UP);
#endif
}

/**
 * @func    sosd_emu_exit
 * @brief   exit from SPIOSD emulation.
 * @param   none
 * @return  none
 */
void
sosd_emu_exit (void)
{
#ifdef SPIOSD_USE_CQ
	cq_write_byte_issue (CQ_P3, SOSD_EMU_CTR_REG, EMU_EXIT, CQ_TRIGGER_VDE);
#else
	IC_WriteByte(TWIC_P3, SOSD_EMU_CTR_REG, EMU_EXIT);
#endif
}

/**
 * @func    sosd_wait_emu_done
 * @brief   waiting SPIOSD emulation to done.
 * @param   none
 * @return  0 if done, or 1 if timeout
 */
unsigned char
sosd_wait_emu_done (void)
{
#if 0
	unsigned short count= 0;

	for(count= 0; count< 65535; count++) {
		if(IC_ReadByte(TWIC_P3,SOSD_CONTROL_REG)&SOSD_EMU_DONE)
			return 0;
	}

	/* maybe this emulation is very long time, so waiting again */
	for(count= 0; count< 65535; count++) {
		if(IC_ReadByte(TWIC_P3,SOSD_CONTROL_REG)&SOSD_EMU_DONE)
			return 0;
	}
	return 1;
#else
	while(!(IC_ReadByte(TWIC_P3,SOSD_CONTROL_REG)&SOSD_EMU_DONE)){};
	return 0;
#endif
}

/**
 * @func    _sosd_swtc_offset
 * @brief   setting horizontal or vertical count for image transition.
 * @param   offset	horizontal or vertical count
 * @return  none
 */
void
_sosd_swtc_offset (unsigned short offset)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_TC_REG1, (unsigned char)(offset&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_TC_REG2, (unsigned char)((IC_ReadByte(TWIC_P3, SOSD_SWITCH_TC_REG2)&(~SOSD_SW_TC2_BIT)) | ((offset>>8)&SOSD_SW_TC2_BIT)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SWITCH_TC_REG1, (unsigned char)(offset&0x00FF));
	IC_WriteByte(TWIC_P3, SOSD_SWITCH_TC_REG2, (unsigned char)((IC_ReadByte(TWIC_P3, SOSD_SWITCH_TC_REG2)&(~SOSD_SW_TC2_BIT)) | ((offset>>8)&SOSD_SW_TC2_BIT)));

#endif
}

/**
 * @func    _sosd_swtc_setting
 * @brief   setting horizontal or vertical count of registers for image transition.
 * @param   value	registers value
 * @return  none
 */
void
_sosd_swtc_setting (unsigned short value)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_TC_REG1, (unsigned char)(value&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_TC_REG2, (unsigned char)(value>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SWITCH_TC_REG1, (unsigned char)(value&0x00FF));
	IC_WriteByte(TWIC_P3, SOSD_SWITCH_TC_REG2, (unsigned char)(value>>8));
#endif
}

/**
 * @func    _sosd_swtc_h_mode
 * @brief   setting SPIOSD image transition of horizontal mode.
 * @param   none
 * @return  none
 */
void
_sosd_swtc_h_mode (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) & (~SOSD_TX_IN_V)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) & (~SOSD_TX_IN_V)));
#endif
}

/**
 * @func    _sosd_swtc_v_mode
 * @brief   setting SPIOSD image transition of vertical mode.
 * @param   none
 * @return  none
 */
void
_sosd_swtc_v_mode (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) | (SOSD_TX_IN_V)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) | (SOSD_TX_IN_V)));
#endif
}

/**
 * @func    _sosd_swtc_enable
 * @brief   enable SPIOSD image transition mode.
 * @param   none
 * @return  none
 */
void
_sosd_swtc_enable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) | SOSD_TX_EN));
	if(rc<0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) | SOSD_TX_EN));
#endif
}

/**
 * @func    _sosd_swtc_disable
 * @brief   disable SPIOSD image transition mode.
 * @param   none
 * @return  none
 */
void
_sosd_swtc_disable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) & (~SOSD_TX_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
    IC_WriteByte(TWIC_P3, SOSD_SWITCH_CTRL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SWITCH_CTRL_REG) & (~SOSD_TX_EN)));
#endif
}

/**
 * @func    sosd_sw_tc_offset
 * @brief   setting horizontal or vertical count for image transition
 * 			and sOSD update.
 * @param   offset	horizontal or vertical count
 * @return  none
 */
void
sosd_sw_tc_offset (unsigned short offset)
{
	_sosd_swtc_offset(offset);
	sosd_update();
}

/**
 * @func    sosd_sw_tc_setting
 * @brief   setting horizontal or vertical count of registers for image transition
 *			and sOSD update.
 * @param   value	registers value
 * @return  none
 */
void
sosd_sw_tc_setting (unsigned short value)
{
	_sosd_swtc_setting(value);
	sosd_update();
}

/**
 * @func    sosd_sw_tc_h_mode
 * @brief   setting SPIOSD image transition of horizontal mode and sOSD update
 * @param   none
 * @return  none
 */
void
sosd_sw_tc_h_mode (void)
{
	_sosd_swtc_h_mode();
	sosd_update();
}

/**
 * @func    sosd_sw_tc_v_mode
 * @brief   setting SPIOSD image transition of vertical mode and sOSD update
 * @param   none
 * @return  none
 */
void
sosd_sw_tc_v_mode (void)
{
	_sosd_swtc_v_mode();
	sosd_update();
}

/**
 * @func    sosd_sw_tc_enable
 * @brief   enable SPIOSD image transition mode and sOSD update
 * @param   none
 * @return  none
 */
void
sosd_sw_tc_enable (void)
{
	_sosd_swtc_enable();
	sosd_update();
}

/**
 * @func    sosd_sw_tc_disable
 * @brief   disable SPIOSD image transition mode and sOSD update
 * @param   none
 * @return  none
 */
void
sosd_sw_tc_disable (void)
{
	_sosd_swtc_disable();
	sosd_update();
}

/**
 * @func    _sosd_load_plut
 * @brief   load SPIOSD pLUT of color table.
 * @param   none
 * @return  none
 */
void
_sosd_load_plut (void)
{
#ifdef SPIOSD_USE_CQ
	// sOSD_En, Shadow, Load pLUT once
	if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_PSLUT|SOSD_SHADOW_UP))
		ERROR ("cq_write_byte()\n");
	if (cq_flush_vde () < 0)
		ERROR ("cq_flush()\n");
#else
	// sOSD_En, Shadow, Load PLUT once
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_PSLUT|SOSD_SHADOW_UP);
#endif
#ifdef DEBUG_SOSD
	INFO("Load SPIOSD pLUT!\n");
#endif
}

/**
 * @func    _sosd_load_slut
 * @brief   load SPIOSD sLUT of color table.
 * @param   none
 * @return  none
 */
void
_sosd_load_slut (void)
{
#ifdef SPIOSD_USE_CQ
	// sOSD_En, Shadow, Load sLUT once
	//IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP);
//	if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP))
//		ERROR ("cq_write_byte()\n");
//	if (cq_flush_vde () < 0)
//		ERROR ("cq_flush()\n");
//	if(cq_available() < CQ_MAX_SIZE) {
//		if (cq_flush_vde () < 0)
//			ERROR ("cq_flush_vde()\n");
//	}
	if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP))
		ERROR ("cq_write_byte()\n");
	if (cq_flush_vde () < 0)
		ERROR ("cq_flush()\n");
	//IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP);
#else
	// sOSD_En, Shadow, Load PLUT once
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP);
#endif
#ifdef DEBUG_SOSD
	INFO("Load SPIOSD sLUT!\n");
#endif
}

/**
 * @func    _sosd_load_pslut
 * @brief   load SPIOSD pLUT and sLUT of color table.
 * @param   none
 * @return  none
 */
void
_sosd_load_pslut (void)
{
#ifdef SPIOSD_USE_CQ
	// sOSD_En, Shadow, Load pLUT and sLUT once
	//IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_PSLUT|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP);
	if(cq_available() < CQ_MAX_SIZE) {
		if (cq_flush_vde () < 0)
			ERROR ("cq_flush_vde()\n");
	}
	if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_PSLUT|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP))
		ERROR ("cq_write_byte()\n");
	if (cq_flush_vde () < 0)
		ERROR ("cq_flush_vde()\n");
#else
	// sOSD_En, Shadow, Load pLUT and sLUT once
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_LUT_ONCE|SOSD_TRIG_LOAD_PSLUT|SOSD_TRIG_LOAD_SSLUT|SOSD_SHADOW_UP);
#endif
#ifdef DEBUG_SOSD
	INFO("Load SPIOSD pLUT & sLUT!\n");
#endif
}

/**
 * @func    sosd_pLUTsLUT_load
 * @brief   load SPIOSD pLUT and sLUT of color table and sOSD update.
 * @param   none
 * @return  none
 */
void
sosd_pLUTsLUT_load (void)
{
#ifdef SPIOSD_USE_CQ
	_sosd_load_pslut();

//	if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN))
//		ERROR ("cq_write_byte()\n");
//	if (cq_flush_vde () < 0)
//		ERROR ("cq_flush_vde()\n");
#else
	_sosd_load_plut();
	//IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_SHADOW_UP);		// sOSD_En, Shadow
#endif
	SOSD_IS_ENABLE= ENABLE;
}

/**
 * @func    sosd_td_enable
 * @brief   enable TWBC and SOSD update.
 * @param   none
 * @return  none
 */
void
sosd_td_enable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)|(SOSD_TD_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)|(SOSD_TD_EN)));
#endif
 	sosd_update();
}

/**
 * @func    sosd_td_disable
 * @brief   disable TWBC and SOSD update.
 * @param   none
 * @return  none
 */
void
sosd_td_disable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_EN)));
#endif
 	sosd_update();
}

/**
 * @func    sosd_td_spilt_mode_disable
 * @brief   enable TWBC of split mode and SOSD update.
 * @param   none
 * @return  none
 */
void
sosd_td_spilt_mode_enable (void)
{
	_sosd_td_spilt_mode_en();
	sosd_update();
}

/**
 * @func    sosd_td_spilt_mode_disable
 * @brief   disable TWBC of split mode and SOSD update.
 * @param   none
 * @return  none
 */
void
sosd_td_spilt_mode_disable (void)
{
	_sosd_td_spilt_mode_dis();
	sosd_update();

}

/**
 * @func    sosd_spr_force_enable
 * @brief   enable to force overwriting sprite alpha-blending percentage
 * 			and sOSD update.
 * @param   none
 * @return  none
 */
void
sosd_spr_force_enable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)|SOSD_SP_FORCE));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)|SOSD_SP_FORCE));
#endif
	sosd_update();
}

/**
 * @func    sosd_spr_enable
 * @brief   enable SPIOSD sprite and load LUT.
 * @param   load	0: unload LUT, 1: load LUT
 * @return  none
 */
void
sosd_spr_enable (unsigned char load)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	if(load)
		_sosd_load_slut();
	rc = cq_write_byte (CQ_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)|(SOSD_SP_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	if(load)
		_sosd_load_slut();
	IC_WriteByte(TWIC_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)|(SOSD_SP_EN)));
#endif
	sosd_update();
}

/**
 * @func    sosd_spr_enable
 * @brief   disable SPIOSD sprite.
 * @param   none
 * @return  none
 */
void
sosd_spr_disable (void)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)&(~SOSD_SP_EN)));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_SP_CONTROL_REG, (unsigned char)(IC_ReadByte(TWIC_P3, SOSD_SP_CONTROL_REG)&(~SOSD_SP_EN)));
#endif
	sosd_update();
}

/**
 * @func    sosd_enable
 * @brief   enable SPIOSD and load LUT.
 * @param   unsigned char load	0: unload LUT, 1: load LUT
 * @return  none
 */
void
sosd_enable (unsigned char load)
{
#ifdef SPIOSD_USE_CQ
	if(load) {
		/* must disable twbc for image(png8) */
		cq_write_byte (CQ_P3, SOSD_TD_CTRL_REG, (IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_EN)));
		_sosd_load_plut();
	} else {
		if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN))
			ERROR ("cq_write_byte()\n");
		if (cq_flush_vde () < 0)
			ERROR ("cq_flush_vde()\n");
	}
			
#else
	if(load)
		_sosd_load_plut();
	else
		IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_SHADOW_UP);		// sOSD_En, Shadow
#endif
	SOSD_IS_ENABLE= ENABLE;
}

/**
 * @func    sosd_disable
 * @brief   disable SPIOSD.
 * @param   none
 * @return  none
 */
void
sosd_disable (void)
{
#ifdef SPIOSD_USE_CQ
	if(SOSD_IS_ENABLE == ENABLE) {
		if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_SHADOW_UP))
			ERROR ("cq_write_byte()\n");
		if (cq_flush_vde () < 0)
			ERROR ("cq_flush_vde()\n");
	} else {
		IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_SHADOW_UP);		// sOSD disable, Shadow
	}
#else
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_SHADOW_UP);		// sOSD disable, Shadow
#endif
	SOSD_IS_ENABLE= DISABLE;
}

/**
 * @func    sosd_update
 * @brief   SPIOSD update.
 * @param   none
 * @return  none
 */
void
sosd_update (void)
{
#ifdef SPIOSD_USE_CQ
	if(SOSD_IS_ENABLE) {
		if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_SHADOW_UP))
			ERROR ("cq_write_byte()\n");
		if (cq_flush_vde () < 0)
			ERROR ("cq_flush()\n");
	} else {
		if (cq_write_byte (CQ_P3, SOSD_CONTROL_REG, SOSD_SHADOW_UP))
			ERROR ("cq_write_byte()\n");
		if (cq_flush_now () < 0)
			ERROR ("cq_flush()\n");
	}
#else
	if(SOSD_IS_ENABLE)
		IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_EN|SOSD_SHADOW_UP);		// sOSD_En, Shadow Update
	else
		IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_SHADOW_UP);		// Shadow Update
#endif
}

/**
 * @func    spiosd_quad_mode_enable
 * @brief	enable SPIOSD Quad mode
 * @param   none
 * @return  none
 */
void
spiosd_quad_mode_enable (void)
{
    IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG, ((IC_ReadByte(TWIC_P3, OSPI_COMMAND_REG)&~0x82) | OSPI_4X));  //wip can't be enabled
}

/**
 * @func    spiosd_quad_mode_disable
 * @brief	disable SPIOSD Quad mode
 * @param   none
 * @return  none
 */
void
spiosd_quad_mode_disable (void)
{
    IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG, (IC_ReadByte(TWIC_P3, OSPI_COMMAND_REG) & ~OSPI_4X));
}

/**
 * @func    sosd_dma_init
 * @brief	initial SPIOSD of DMA configure
 * @param   none
 * @return  none
 */
void
sosd_dma_init (void)
{
	IC_WriteByte(TWIC_P3, OSPI_COMMAND_REG, 0x6D);				// Control
	IC_WriteByte(TWIC_P3,
		OSPI_DMA_CTRL_REG1, OSPI_XFER_CNT | HS_LEAD_EDGE );		// DMA Control (but Trig by Video Timing)
	IC_WriteByte(TWIC_P3,
		SOSD_CLK_SEL_REG, OSPI_H_FREQ_CLK | 0x10);				// DMA Control (but Trig by Video Timing)
}

/**
 * @func    spiosd_init
 * @brief	initial SPIOSD and set h/v of offset
 * @param   h_offset	image of horizontal offset
 * 			v_offset	image of vertical offset
 * 			s_h_offset	sprite of horizontal offset
 * 			s_v_offset	sprite of vertical offset
 * @return  none
 */
void
spiosd_init (unsigned short h_offset, unsigned short v_offset,
			 unsigned short s_h_offset, unsigned short s_v_offset)
{
	/* sOSD Disable */
	IC_WriteByte(TWIC_P3, SOSD_CONTROL_REG, SOSD_SHADOW_UP);		// sOSD disable, Shadow

	/* sOSD TD Disable */
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG,
		(IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_EN)));
	IC_WriteByte(TWIC_P3, SOSD_TD_CTRL_REG,
		(IC_ReadByte(TWIC_P3, SOSD_TD_CTRL_REG)&(~SOSD_TD_COLOR_SPILT_MODE)));

	spiosd_quad_mode_enable();          // oSPI 4x

	/* sOSD DMA Control */
	sosd_dma_init();

	/* set SPIOSD of  h/v offset */
	HSTART_OFFSET = 	h_offset;
	VSTART_OFFSET =		v_offset;
	SHSTART_OFFSET =	s_h_offset;
	SVSTART_OFFSET =	s_v_offset;

	DBG_PRINT("\n\r");
	DBG_PRINT("SPIOSD Init\n\r");
}

/**
 * @func    _get_img_idx_addr
 * @brief   getting current SPIOSD image of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
void
_get_img_idx_addr (unsigned long *addr)
{
	*addr = IC_ReadByte(TWIC_P3, SOSD_IMG_ADDR_REG3);
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_IMG_ADDR_REG2));
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_IMG_ADDR_REG1));
}

/**
 * @func    _get_spr_idx_addr
 * @brief   getting current SPIOSD sprite of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
void
_get_spr_idx_addr(unsigned long *addr)
{
	*addr = IC_ReadByte(TWIC_P3, SOSD_SPRITE_ADDR_REG3);
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_SPRITE_ADDR_REG2));
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_SPRITE_ADDR_REG1));
}

/**
 * @func    _get_img_ls
 * @brief   getting current SPIOSD image of line store size.
 * @param   *ls		line store size will store to this point
 * @return  none
 */
void
_get_img_ls (unsigned short *ls)
{
	*ls = IC_ReadByte(TWIC_P3, SOSD_LJUMP_A_REG2);
	*ls <<= 8;
	*ls |= (IC_ReadByte(TWIC_P3, SOSD_LJUMP_A_REG1));
}

/**
 * @func    _get_spr_ls
 * @brief   getting current SPIOSD sprite of line store size.
 * @param   *ls		line store size will store to this point
 * @return  none
 */
void
_get_spr_ls(unsigned short *ls)
{
	*ls = IC_ReadByte(TWIC_P3, SOSD_SP_LJUMP_A_REG2);
	*ls <<= 8;
	*ls |= (IC_ReadByte(TWIC_P3, SOSD_SP_LJUMP_A_REG1));
}

/**
 * @func    _get_plut_addr
 * @brief   getting current SPIOSD pLUT of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
void
_get_plut_addr(unsigned long *addr)
{
	*addr = IC_ReadByte(TWIC_P3, SOSD_LUT_ADDR_REG3);
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_LUT_ADDR_REG2));
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_LUT_ADDR_REG1));
}

/**
 * @func    _get_slut_addr
 * @brief   getting current SPIOSD sLUT of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
void
_get_slut_addr(unsigned long *addr)
{
	*addr = IC_ReadByte(TWIC_P3, SOSD_SLUT_ADDR_REG3);
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_SLUT_ADDR_REG2));
	*addr <<= 8;
	*addr |= (IC_ReadByte(TWIC_P3, SOSD_SLUT_ADDR_REG1));
}

/**
 * @func    _get_swtc_offset
 * @brief   getting current SPIOSD image transition of offset.
 * @param   *offset		offset will store to this point
 * @return  none
 */
void
_get_swtc_offset(unsigned short *offset)
{
	*offset = ((IC_ReadByte(TWIC_P3, SOSD_SWITCH_TC_REG2)&SOSD_SW_TC2_BIT));
	*offset <<= 8;
	*offset |= IC_ReadByte(TWIC_P3, SOSD_SWITCH_TC_REG1);
}

/**
 * @func    _get_img_h_position
 * @brief   getting current SPIOSD image h position of offset.
 * @param   *h_position		offset will store to this point
 * @return  none
 */
void
_get_img_h_position(unsigned short *h_position)
{
	*h_position = ((IC_ReadByte(TWIC_P3, SOSD_HSTAR_REG2)&0x7F));
	*h_position <<= 8;
	*h_position |= IC_ReadByte(TWIC_P3, SOSD_HSTAR_REG1);
}

/**
 * @func    _get_img_v_position
 * @brief   getting current SPIOSD image v position of offset.
 * @param   *v_position		offset will store to this point
 * @return  none
 */
void
_get_img_v_position(unsigned short *v_position)
{
	*v_position = ((IC_ReadByte(TWIC_P3, SOSD_VSTAR_REG2)&0x3F));
	*v_position <<= 8;
	*v_position |= IC_ReadByte(TWIC_P3, SOSD_VSTAR_REG1);
}

/**
 * @func    _get_spr_h_position
 * @brief   getting current SPIOSD sprite h position of offset.
 * @param   *h_position		offset will store to this point
 * @return  none
 */
void
_get_spr_h_position(unsigned short *h_position)
{
	*h_position = ((IC_ReadByte(TWIC_P3, SOSD_SP_HSTAR_REG2)&0x7F));
	*h_position <<= 8;
	*h_position |= IC_ReadByte(TWIC_P3, SOSD_SP_HSTAR_REG1);
}

/**
 * @func    _get_spr_v_position
 * @brief   getting current SPIOSD sprite v position of offset.
 * @param   *v_position		offset will store to this point
 * @return  none
 */
void
_get_spr_v_position(unsigned short *v_position)
{
	*v_position = ((IC_ReadByte(TWIC_P3, SOSD_SP_VSTAR_REG2)&0x3F));
	*v_position <<= 8;
	*v_position |= IC_ReadByte(TWIC_P3, SOSD_SP_VSTAR_REG1);
}

/**
 * @func    _wr_sosd_hspos
 * @brief   write SPIOSD image of h start position and no offset.
 * @param   h_pos	horizontal of position
 * @return  none
 */
void
_wr_sosd_hspos (unsigned short h_pos)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_HSTAR_REG1, (unsigned char)(h_pos&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_HSTAR_REG2, (unsigned char)((h_pos&0x07FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_HSTAR_REG1, (unsigned char)(h_pos&0x00FF));			// L
	IC_WriteByte(TWIC_P3, SOSD_HSTAR_REG2, (unsigned char)((h_pos&0x07FF)>>8));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_hspos %u \n", h_pos);
#endif
}

/**
 * @func    _wr_sosd_vspos
 * @brief   write SPIOSD image of v start position and no offset.
 * @param   v_pos	vertical of position
 * @return  none
 */
void
_wr_sosd_vspos (unsigned short v_pos)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_VSTAR_REG1, (unsigned char)(v_pos&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_VSTAR_REG2, (unsigned char)((v_pos&0x03FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_VSTAR_REG1, (unsigned char)(v_pos&0x00FF));			// L
	IC_WriteByte(TWIC_P3, SOSD_VSTAR_REG2, (unsigned char)((v_pos&0x03FF)>>8));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_vspos %u \n", v_pos);
#endif
}

/**
 * @func    _wr_sosd_spr_hspos
 * @brief   write SPIOSD sprite of h start position and no offset.
 * @param   h_pos	horizontal of position
 * @return  none
 */
void
_wr_sosd_spr_hspos (unsigned short h_pos)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_HSTAR_REG1, (unsigned char)(h_pos&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_SP_HSTAR_REG2, (unsigned char)((h_pos&0x0FFF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_SP_HSTAR_REG1, (unsigned char)(h_pos&0x00FF));				// L
	IC_WriteByte(TWIC_P3, SOSD_SP_HSTAR_REG2, (unsigned char)((h_pos&0x0FFF)>>8));			// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_hspos %u \n", h_pos);
#endif
}

/**
 * @func    _wr_sosd_spr_vspos
 * @brief   write SPIOSD sprite of v start position and no offset.
 * @param   v_pos	vertical of position
 * @return  none
 */
void
_wr_sosd_spr_vspos (unsigned short v_pos)
{
#ifdef SPIOSD_USE_CQ
	int rc = 0;
	rc = cq_write_byte (CQ_P3, SOSD_SP_VSTAR_REG1, (unsigned char)(v_pos&0x00FF));
	rc = cq_write_byte (CQ_P3, SOSD_SP_VSTAR_REG2, (unsigned char)((v_pos&0x03FF)>>8));
	if(rc < 0)
		ERROR ("cq_write_byte()\n");
#else
	IC_WriteByte(TWIC_P3, SOSD_SP_VSTAR_REG1, (unsigned char)(v_pos&0x00FF));			// L
	IC_WriteByte(TWIC_P3, SOSD_SP_VSTAR_REG2, (unsigned char)((v_pos&0x03FF)>>8));		// H
#endif
#ifdef DEBUG_SOSD
    dbg(0, "_sosd_spr_vspos %u \n", v_pos);
#endif
}
