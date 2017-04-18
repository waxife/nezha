/**
 *  @file   tw_widget_sys.c
 *  @brief  terawins UI widget system
 *  $Id: tw_widget_sys.c,v 1.18 2016/06/16 02:14:03 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.18 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 *
 *  @date   2013/11/15  Ken		New file.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <io.h>
#include <unistd.h>
#include <watchdog.h>
#include <heap.h>
#include <codec_eng/ringbell.h>

#ifdef DBG_LEVEL
#undef DBG_LEVEL
#endif
#define DBG_LEVEL   0
#include <debug.h>

#include "rc.h"
#include "reg_tw.h"
#include "iccontrol.h"
#include "tw_widget_sys.h"
#include "gpio.h"
#include "video_det.h"

#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"
#include "./drivers/spirw/spirw.h"
#include "./drivers/cq/cq.h"

#include "./drivers/scaler/scaler.h"
#include "./drivers/ir/ir.h"
#include "./drivers/sarkey/sarkey.h"
#include "./drivers/touch/touch.h"

//#ifdef USE_CQ_WRITE	/* for root control CQ write, maybe someday need it */
#define TW_USE_CQ
//#endif
/* DEBUG MODE SETTING */
//#define TW_WG_DEBUG_MODE
#define _CQ_DRAW_FONT_MENU  1

/* Status variable
 * 		CUR_PAGE_ID		current of page ID
 *		CUR_MENU_P		current of OSD2 menu of struct
 *		CUR_MENU_RES	current of OSD2 menu of resource
 *		CUR_TOF_RES		current of TOF	of resource
 *		CUR_BK_ID		current of Background (IMG/TWBC) of ID
 *		CUR_SPR_ID	    current of Sprite of ID
 *		CUR_ICONSET_NUM current of ICON SET of number
 *      CUR_2BP_IDX 	current of OSD2 2BP start index
 */

static const struct
tw_menu null_menu = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};
static unsigned char 	CUR_PAGE_ID = 0xFF;
static struct tw_menu* 	CUR_MENU_P = (struct tw_menu*)&null_menu;
struct res_menu_t 		CUR_MENU_RES;
struct res_tof_t 		CUR_TOF_RES;
static unsigned short 	CUR_BK_ID = 0xFFFF;
static unsigned short 	CUR_SPR_ID = 0xFFFF;
static unsigned char 	CUR_ICONSET_NUM = 0;
static unsigned short 	CUR_2BP_IDX = 0;
static unsigned char 	menu_string_start_index = 0;
//static unsigned char 	SUPPORT_1BP_ROM= 0;

extern void endian_change(int size,char *data);
#define endian_inv(data) endian_change(sizeof(data),(char*)&(data))

#define ID_TEXT_ROM          0	
#define ID_TEXT_PIC_2BP 	0xFFFF

// should need?
void tw_init (void)
{
	/* power on BIAS  */
    IC_WriteByte(TWIC_P0, 0xE3, 0x80);
	/* power on DPLL */
    IC_WriteByte(TWIC_P0, 0xCA, IC_ReadByte(TWIC_P0, 0xCA) | 0x20);
	/* power on Total Pad */
    IC_WriteByte(TWIC_P0, 0xE0, IC_ReadByte(TWIC_P0, 0xE0) | 0x80);
	/* disable shadow */
    IC_WriteByte(TWIC_P0, 0xE2, 0x00);
	/* turn on free run */
    IC_WriteByte(TWIC_P0, 0xC2, 0x12);
	/* ORAM IO WaitState */
    //IC_WriteByte(TWIC_P1, 0xFE, 0xAD);
}

/**
 * @func    region_check
 * @brief   check touch event of location whether or not in UI of region.
 * @param   TW_EVENT* event		tw widget system of event
 * 			x		region of x start
 * 			y		region of y start
 * 			w		region of width
 * 			h		region of height
 * 			type	UI of type
 * @return  0 if not in region, or 1 if in region
 */
unsigned char
region_check (TW_EVENT* event, unsigned int x, unsigned int y,
			  unsigned int w, unsigned int h, unsigned char type)
{
    char ret = 0;

	if(type == TW_UI_TYPE_OSD2) {
		/* location must do pixel to font of transform if UI is OSD2 type*/
        x = x*CUR_MENU_P->font_w;
        y = y*CUR_MENU_P->font_h;
        w = w*CUR_MENU_P->font_w;
        h = h*CUR_MENU_P->font_h;
	}		

    dbg(3, "x=[%u] y=[%u] w=[%u] h=[%u]!\n", x, y, w, h);

    /* check event type */
    if((event->type==TW_EVENT_TYPE_TOUCH_DOWN) ||
    	(event->type==TW_EVENT_TYPE_TOUCH_RELEASE) ||
    	(event->type==TW_EVENT_TYPE_TOUCH_MOTION) ||
    	(event->type==TW_EVENT_TYPE_TOUCH_REPEAT)) {
    	/* check location of region  */
    	if((event->touch_panel.x > x) && (event->touch_panel.x < (x + w)) &&
    		(event->touch_panel.y > y) && (event->touch_panel.y < (y + h))) {
            dbg(3, "in region!\n");
            ret = 1;
        }
    }
 
    return ret;
}

/**
 * @func    next_iconset
 * @brief   change to next icon set(changeable, multi-language).
 * @param   none
 * @return  none
 * @note	should call tw_draw_menu for change of effect
 */
void
next_iconset (void)
{
	CUR_ICONSET_NUM++;
	if(CUR_ICONSET_NUM > CHANGEABLE_SET_MAX)
		CUR_ICONSET_NUM= 1;
}

/**
 * @func    set_iconset_num
 * @brief   set icon set no.(changeable, multi-language).
 * @param   num		set no.
 * @return  none
 * @note	should call tw_draw_menu for change of effect
 */
void
set_iconset_num (unsigned char num)
{
	CUR_ICONSET_NUM= num;
	if(CUR_ICONSET_NUM > CHANGEABLE_SET_MAX)
		CUR_ICONSET_NUM= 1;
}

/**
 * @func    next_iconset
 * @brief   getting current icon set of no.(changeable, multi-language).
 * @param   none
 * @return  icon set of no.
 */
unsigned char
get_iconset_num (void)
{
	return CUR_ICONSET_NUM;
}

/**
 * @func    find_ch_icon
 * @brief   icon ID of mapping function with ICONSET (changeable).
 * @param   id		dynamic of icon ID
 *			pmenu	icon ID belongs to the this menu
 * @return  0 if not changeable menu or other errors
 */
unsigned short
find_ch_icon(unsigned short id, struct tw_menu* pmenu)
{
	unsigned char i= 0;
	unsigned short icon_id= 0;
	unsigned long tmp_addr= 0;
	struct res_tile_t tile_s;

	if(id == 0xFFFF)
		return 0;

	/* get menu of tiles structure with tiles ID */
	if(get_res_tile(pmenu->tilesID, &tile_s))
    	return 0;

	/* get table of first icon ID with base address */
	memcpy(&icon_id, (char*)(tile_s.base_addr), sizeof(unsigned short));
	endian_inv(icon_id);

	if(icon_id==0) {	// this change table if icon ID is '0'

		/* check current of ICONSET */
		if((CUR_ICONSET_NUM>=tile_s.length)||(CUR_ICONSET_NUM<=1)) {
			CUR_ICONSET_NUM= 1;
			return id;	// default ICON SET if '1' or unknow
		}

		/* get changeable of defalut table of tiles structure */
		memcpy(&icon_id, (char*)(tile_s.base_addr+2), sizeof(unsigned short));
		endian_inv(icon_id);

		tmp_addr = tile_s.base_addr;
		get_res_tile(icon_id, &tile_s);

		/* search icon ID of order in default table */
		for(i=0; i<tile_s.length; i++) {
			memcpy(&icon_id, (char*)(tile_s.base_addr+(i*2)), sizeof(unsigned short));
			endian_inv(icon_id);
			if(id == icon_id)
				break;		// find
		}

		if(i >= tile_s.length) {
			ERROR("can't find chageable of icon id!\n");
			return 0;
		}

		/* get current ICON SET of tiles structure */
		memcpy(&icon_id, (char*)(tmp_addr+(2*CUR_ICONSET_NUM)), sizeof(unsigned short));
		endian_inv(icon_id);
		get_res_tile(icon_id, &tile_s);

		/* get mapping of icon ID */
		memcpy(&icon_id, (char*)(tile_s.base_addr+(i*2)), sizeof(unsigned short));
		endian_inv(icon_id);
		return icon_id;

	} else {
	 	ERROR("this menu not has chageable fucntion!\n");
		return 0;
	}
}

/**
 * @func    tw_load_oregs
 * @brief   loading a OSD2 registers table with specified ID.
 * @param   id	resource of ID
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_load_oregs (unsigned short id)
{
	struct res_oreg_t oreg_s;
	
#ifdef TW_WG_DEBUG_MODE
	show_resource_header(id);
#endif

	if(get_res_oreg(id, &oreg_s))
		return 1;

	osd2_wr_regs_tbl(oreg_s.base_addr, oreg_s.length);

    return 0;
}

#define DMA_MAX_LENGTH		0x500//0x2000 Some case cause menu dma error.
/**
 * @func    _tw_load_menu_at
 * @brief   loading a OSD2 MENU with specified ID and address.
 * @param   id		resource of ID
 * 			addr	OSD2 RAM of address
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
_tw_load_menu_at (unsigned short id, unsigned short addr)
{
    unsigned char i=0, count=0;
    struct res_menu_t menu_s;

    if(get_res_menu(id, &menu_s))
    	return 1;
    if(addr >= OSD2_RAM_MAX_SIZE) {
    	ERROR("address(0x%04X) out range(0x2000)!\n", addr);
    	return 1;
    }

    count = (menu_s.length/DMA_MAX_LENGTH);

#ifdef TW_WG_DEBUG_MODE
    dbg(0, ">>>> MENU LENGTH %u !\n", menu_s.length);
#endif
    for(i=0; i< count; i++) {
        spi_dma2oram((menu_s.base_addr + (i*DMA_MAX_LENGTH)),
			(addr + (i*DMA_MAX_LENGTH/2)), DMA_MAX_LENGTH);
#ifdef TW_WG_DEBUG_MODE     
		dbg(0, "spi_dma2oram(%08lX, %08X, %08X)\n",
			(menu_s.base_addr + (i*DMA_MAX_LENGTH)),
			(addr + (i*DMA_MAX_LENGTH/2)), DMA_MAX_LENGTH);
#endif
    }
    if((menu_s.length%DMA_MAX_LENGTH)) {
        spi_dma2oram((menu_s.base_addr + (i*DMA_MAX_LENGTH)),
			(addr + (i*DMA_MAX_LENGTH/2)), (menu_s.length%DMA_MAX_LENGTH));
#ifdef TW_WG_DEBUG_MODE     
		dbg(0, "spi_dma2oram(%08lX, %08X, %08X)\n",
			(menu_s.base_addr+(i*DMA_MAX_LENGTH)),
			(addr + (i*DMA_MAX_LENGTH/2)), (menu_s.length%DMA_MAX_LENGTH));
		osd2_oram_dump((addr + (i*DMA_MAX_LENGTH/2)), (menu_s.length%DMA_MAX_LENGTH)/2);
#endif  
    }

    return 0;
}

/**
 * @func    tw_load_menu
 * @brief   loading a OSD2 MENU with specified struct MENU.
 * @param   *pmenu	struct MENU of point
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_load_menu (struct tw_menu* pmenu)
{
    if(pmenu == NULL) {
    	ERROR("struct tw_menu can't is NULL!");
    	return 0;
    }

	return _tw_load_menu_at(pmenu->menuID, pmenu->addr);
}

/**
 * @func    tw_load_icon
 * @brief   loading a OSD2 ICON with specified ID and address.
 * @param   id		resource of ID
 * 			*oaddr	OSD2 RAM of address
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_load_icon (unsigned short id, unsigned short *oaddr)
{
	unsigned char i= 0;
	unsigned short count= 0;
	struct res_icon_t icon_s;

	if(id == 0xFFFF)
		return 1;

#if 1 //def TW_WG_DEBUG_MODE
	show_resource_header(id);
#endif

    if(get_res_icon(id, &icon_s))
    	return 1;
    if(*oaddr >= OSD2_RAM_MAX_SIZE) {
		ERROR("address(0x%04X) out range(0x2000)!\n", *oaddr);
		return 1;
	}

	count = ((icon_s.font_h*icon_s.font_w)/((8/icon_s.bp)));
	if(count%2)
		count++;
	count*= icon_s.count;
	count = (count/DMA_MAX_LENGTH);

    for(i=0; i< count; i++) {
		spi_dma2oram((icon_s.base_addr+(i*DMA_MAX_LENGTH)), 
			(*oaddr)+ (i*DMA_MAX_LENGTH/2),
			DMA_MAX_LENGTH);
#if 1//def TW_WG_DEBUG_MODE
		dbg(0, "spi_dma2oram(%08lX, %08X, %08X)\n", 
			(icon_s.base_addr+(i*DMA_MAX_LENGTH)),
			(*oaddr) + (i*DMA_MAX_LENGTH/2),
			DMA_MAX_LENGTH);
#endif
	}
	count = ((icon_s.font_h*icon_s.font_w)/((8/icon_s.bp)));
	if(count%2)
		count++;
	count*= icon_s.count;

	if((count%DMA_MAX_LENGTH)) {
		(*oaddr)+= (i*DMA_MAX_LENGTH/2);
		spi_dma2oram((icon_s.base_addr+(i*DMA_MAX_LENGTH)), 
			(*oaddr), (count%DMA_MAX_LENGTH));
		
#if 1//def TW_WG_DEBUG_MODE
		dbg(0, "spi_dma2oram(%08lX, %08X, %08X)\n", 
			(icon_s.base_addr+(i*DMA_MAX_LENGTH)),
			(*oaddr), (count%DMA_MAX_LENGTH));
#endif
		(*oaddr)+= ((count%DMA_MAX_LENGTH)/2);
	} else {
		(*oaddr)+= (i*DMA_MAX_LENGTH/2);
	}
	
	return 0;
}

/**
 * @func    _tw_load_tiles_at
 * @brief   loading a OSD2 tiles(fonts) with specified ID and address.
 * @param   id		resource of ID
 * 			*oaddr	OSD2 RAM of address
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
_tw_load_tiles_at (unsigned short id, unsigned short oaddr)
{
    unsigned char i= 0;
    unsigned short icon_id= 0;
    unsigned short tmp_addr= oaddr;
    struct res_tile_t tile_s;

    if(id == 0xFFFF)
        return 1;

#ifdef TW_WG_DEBUG_MODE
    show_resource_header(id);
#endif  

    if(get_res_tile(id, &tile_s))
    	return 1;
    if(oaddr >= OSD2_RAM_MAX_SIZE) {
		ERROR("address(0x%04X) out range(0x2000)!\n", oaddr);
		return 1;
	}

    /* get tile table(icon table) of address */
	memcpy(&icon_id, (char*)(tile_s.base_addr), sizeof(unsigned short));// add by dos
	endian_inv(icon_id);

	if(icon_id == 0) {
		/* changeable type of table */
		if((CUR_ICONSET_NUM >= tile_s.length) || (CUR_ICONSET_NUM <= 0))
			CUR_ICONSET_NUM = 1;
		/* get icon table of address */
		memcpy(&icon_id, (char*)(tile_s.base_addr+(2*CUR_ICONSET_NUM)), sizeof(unsigned short));// add by dos
		endian_inv(icon_id);

		if(get_res_tile(icon_id, &tile_s))
			return 1;
	}
	 
	for(i=0; i<tile_s.length; i++) {
        memcpy(&icon_id, (char*)(tile_s.base_addr+(i*2)), sizeof(unsigned short));
		endian_inv(icon_id);
        tw_load_icon(icon_id, &tmp_addr);
    }

	return 0;
}
  
/**
 * @func    tw_load_tiles
 * @brief   loading a OSD2 tiles(fonts) with specified ID.
 * @param   id		resource of ID
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_load_tiles (unsigned short id)
{
    return _tw_load_tiles_at(id, 0);
}

/**
 * @func    tw_load_luts
 * @brief   loading a OSD2 tiles(fonts) with specified ID.
 * @param   id		resource of ID
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_load_luts (unsigned short id)
{
	struct res_lut_t lut_s;	
	
#ifdef TW_WG_DEBUG_MODE
	show_resource_header(id);
#endif

	if(get_res_lut(id, &lut_s))
		return 1;

	if(lut_s.main_len)
		osd2_wr_lut_tbl(lut_s.lut_addr, (lut_s.main_len*4));
	if(lut_s.re_2bp_len)
		osd2_wr_2bp_lut_tbl((lut_s.lut_addr+(lut_s.main_len*4)), (lut_s.re_2bp_len*1));
	if(lut_s.re_bg_len)
		osd2_wr_bg_lut_tbl((lut_s.lut_addr+(lut_s.main_len*4)+(lut_s.re_2bp_len*1)), (lut_s.re_bg_len*1));

	return 0;
}

/**
 * @func    tw_img_draw
 * @brief   drawing a SPIOSD IMAGE with specified struct IMAGE.
 * @param   *pimg		struct tw_img of point
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_img_draw (struct tw_img* pimg)
{
	struct res_img_t img_s;

	if(pimg == NULL) {
		ERROR("tw image of point is null!\n");
		return 1;
	}
	
#ifdef TW_WG_DEBUG_MODE
	show_resource_header(pimg->imgID);
#endif
	 
	if(get_res_img(pimg->imgID, &img_s))
		return 1;
	 
	_sosd_pLUT_addr(img_s.lut_addr);
    _sosd_img_addr(img_s.index_addr);
    _sosd_img_ls(img_s.line_jump);
    _sosd_img_window(pimg->width, pimg->height);
	_sosd_img_loca(pimg->x, pimg->y);

    CUR_BK_ID = pimg->imgID;
 
    return 0;
} 

/**
 * @func    tw_spr_draw
 * @brief   loading a SPIOSD SPRITE with specified struct SPRITE.
 * @param   *pspr		struct tw_spr of point
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_spr_draw (struct tw_spr* pspr)
{
	//char rc= 0;
	struct res_spr_t spr_s;
	unsigned long tmpAddr= 0;

	if(pspr == NULL) {
		ERROR("tw sprite of point is null!\n");
		return 1;
	}

#ifdef TW_WG_DEBUG_MODE
	show_resource_header(pspr->sprID);
#endif
		
	if(get_res_spr(pspr->sprID, &spr_s))
		return 1;

	_sosd_sLUT_addr(spr_s.lut_addr);

	if(pspr->cnt<spr_s.count) {
		tmpAddr = pspr->width*pspr->height;
		tmpAddr *= pspr->cnt;
		tmpAddr += spr_s.index_addr;
	} else if(pspr->cnt>spr_s.count){
		tmpAddr = (spr_s.index_addr);
		ERROR("Sprite count is outrange!\n");
	} else {
		tmpAddr = (spr_s.index_addr);
	}

	_sosd_spr_addr(tmpAddr);
	_sosd_spr_ls(spr_s.line_jump);
	_sosd_spr_window(pspr->width, pspr->height);
	_sosd_spr_loca(pspr->x, pspr->y); 
	
	CUR_SPR_ID = pspr->sprID;

	return 0;
}

/**
 * @func    tw_get_spr_idx
 * @brief   getting a SPIOSD SPRITE of index address with specified ID.
 * @param   id		resource of ID
 * 			addr	sprite of index address
 * @return  0 if successfully, or 1 if fail
 */
void
tw_get_spr_idx (unsigned short id, unsigned long *addr)
{
	struct res_spr_t spr_s;
		
	if(get_res_spr(id, &spr_s))
		*addr = 0;
	else
		*addr = spr_s.index_addr;
}

/**
 * @func    tw_spr_action
 * @brief   action a SPIOSD SPRITE with specified struct SPRITE.
 * @param   struct tw_spr* pspr		struct tw_spr of point
 * @return  none
 */
void
tw_spr_action (struct tw_spr* pspr)
{	
	unsigned short id = CUR_SPR_ID;
	
	tw_spr_draw(pspr);

	if(pspr->sprID == id)
		sosd_spr_enable(DONT_LOAD);
	else
		sosd_spr_enable(LOAD_LUT);
}

/**
 * @func    tw_td_draw
 * @brief   drawing a SPIOSD TWBC with specified ID.
 * @param   *ptd	sturct tw_td of point
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_td_draw (struct tw_td* ptd)
{
	struct res_td_t td_s;
	
	if(ptd == NULL) {
		ERROR("tw td of point is null!\n");
		return 1;
	}

#ifdef TW_WG_DEBUG_MODE
	show_resource_header(ptd->tdID);
#endif
	
	if(get_res_td(ptd->tdID, &td_s))
		return 1;
	
	_sosd_tdc_addr(td_s.color_addr);
	_sosd_img_addr(td_s.index_addr);
	_sosd_td_ls(td_s.line_jump);
	_sosd_tdc_h_size(td_s.line_jump);
	_sosd_td_window(ptd->width, ptd->height);
	_sosd_td_loca(ptd->x, ptd->y);
	_sosd_td_spilt_mode_en();

	CUR_BK_ID = ptd->tdID;

	return 0;
}

/**
 * @func    tw_emu_trig
 * @brief   trigger SPIOSD of emulation
 * @param   none
 * @return  none
 */
void
tw_emu_trig (void)
{
	//IC_WriteByte (TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_ENABLE);	// for SPIOSD shadow issue
	cq_write_byte_issue(CQ_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_ENABLE, CQ_TRIGGER_SW);
	sosd_emu_trig();
}

/**
 * @func    tw_load_emu
 * @brief   load SPIOSD of emulation and trigger
 * @param   id	emulation of ID
 * @return  none
 */
void
tw_load_emu (unsigned short id)
{
	struct res_emu_t emu_s;

#ifdef TW_WG_DEBUG_MODE
	show_resource_header(id);
#endif

	get_res_emu(id, &emu_s);

	tw_emu_exit();

	sosd_set_emu_base_addrss(emu_s.base_addr);
	tw_emu_trig();
}

/**
 * @func    tw_wait_emu_done
 * @brief   waiting emulation action done
 * @param   none
 * @return  none
 */
void
tw_wait_emu_done (void)
{
	sosd_wait_emu_done();
	cq_write_byte_issue(CQ_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_DISABLE, CQ_TRIGGER_SW);
	//IC_WriteByte (TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_DISABLE);	// for SPIOSD shadow issue
}

/**
 * @func    tw_emu_exit
 * @brief   exit SPIOSD of emulation action
 * @param   none
 * @return  none
 */
void
tw_emu_exit (void)
{
	sosd_emu_exit();
	//IC_WriteByte (TWIC_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_DISABLE);	// for SPIOSD shadow issue
	cq_write_byte_issue(CQ_P0, SHADOW_CTRL_CONF_REG, SHADOW_CTRL_DISABLE, CQ_TRIGGER_SW);
}

/**
 * @func    tw_img_force_alpha
 * @brief   control SPIOSD image of force alpha
 * @param   en		0: disable 1: enable
 * @return  none
 */
void
tw_img_force_alpha (unsigned char en)
{
	if(en) {
		sosd_img_force_ctrl(64);
		sosd_img_force_enable();
	} else
		sosd_img_force_disable();
}

/**
 * @func    tw_fixed_menu2
 * @brief   fix a OSD2 of menu2 bug.
 * @param   none
 * @return  none
 */
static void
tw_fixed_menu2 (void)
{
	osd2_set_ram_addr(0x1FFE);
	osd2_set_ram_data(0x0801);
	osd2_set_ram_data(0x0000);   // 0 char
}

/**
 * @func    tw_menu_draw
 * @brief   drawing a SPIOSD MENU with specified struct MENU.
 * @param   *pmenu	sturct tw_menu of point
 * @return  0 if successfully, or 1 if fail
 */
unsigned char
tw_menu_draw(struct tw_menu* pmenu)
{
	if(pmenu == NULL) {
		ERROR("tw menu of point is null!\n");
		goto EXIT;
	}

	if(get_res_menu(pmenu->menuID, &CUR_MENU_RES))
		goto EXIT;

	if(pmenu->tofID != 0xffff)//by dos default 0xff
	{
		if(get_res_tof(pmenu->tofID, &CUR_TOF_RES))
			goto EXIT;
	}

	tw_load_oregs(pmenu->regsID);
	tw_load_luts(pmenu->lutsID);
	tw_load_tiles(pmenu->tilesID);
	
	tw_load_menu(pmenu);
	tw_fixed_menu2();
	tw_menu_reset_index();
	CUR_MENU_P = pmenu;

	_osd2_set_menu_addr(pmenu->addr);
	_osd2_set_menu_width(pmenu->width);
	_osd2_set_menu_height(pmenu->height);

	return 0;

EXIT:
	CUR_MENU_P = (struct tw_menu*)&null_menu;
	return 1;
}

/**
 * @func    check_color_config
 * @brief   change to color attribute.
 * @param   index	font of index
 * 			offset	font of offset
 * 			color	color of index
 * @return  color of attribute
 */
unsigned char
check_color_config (unsigned short index, unsigned char offset, unsigned char color)
{
	unsigned short FONT_START= 0;
	unsigned char ccolor= 0;

	FONT_START= osd2_cfg_read(OSD_CHAR2BP_FONT_INDEX);
	if(FONT_START==0xFF)
		FONT_START= osd2_cfg_read(OSD_CHAR4BP_FONT_INDEX);
	FONT_START <<= 1;

	index+= offset;

	if(index<FONT_START) {
		/* 1BP/ROM Font */
		ccolor = color;//(color&0x0F)|((color&0xF0)<<1);
	} else {
		/* 2BP/4BP/8BP Font */
		ccolor = (color&0x0F);
	}

	return ccolor;
}

/**
 * @func    check_osd2_region
 * @brief   check location of region.
 * @param   x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  0 if in region, 1 if out of range
 */
static int
check_osd2_region (unsigned char x, unsigned char y, unsigned char width, unsigned char height,
				   unsigned char menu_width, unsigned char menu_row)
{
	int rc = 0;

	if(y >= menu_row) {
		ERROR("row(%u) is out of range(%u)!\n\r", y, menu_row);
		rc = 1;
	}
	if((y+height) > menu_row) {
		WARN("row length(%u) is out of range(%u)!\n\r", (y+height), menu_row);
	}
	if(x >= menu_width) {
		ERROR("char(%u) is out of range(%u)!\n\r", x, menu_width);
		rc = 1;
	}
	if((x+width) > menu_width) {
		WARN("char length(%u) is out of range(%u)!\n\r", (x+width), menu_width);
	}

	return rc;
}

/**
 * @func    _icon_draw_at
 * @brief   draw(put) character attribute in MENU with parameters.
 * @param   index		font of index
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
void
_icon_draw_at (unsigned short index, unsigned char color, unsigned char x, unsigned char y,
			   unsigned char width, unsigned char height, unsigned short menu_addr,
			   unsigned char menu_width, unsigned char menu_row, unsigned short offset,
			   unsigned char blinking)
{
#ifndef TW_USE_CQ
	int rc = 0;
	int w, h = 0;
	int i = 0, temp = 0;

    rc = check_osd2_region(x, y, width, height, menu_width, menu_row);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
#endif

#ifdef EX_I2C_SAFE_MODE
	IC_WriteByte(TWIC_P0, 0xF0, SLAVE_SAFE_ADDR);
#endif

	color <<= 1;    // lut index[8:5]
	if(blinking)
		color |= 0x20;

#ifdef TW_USE_CQ
    _osd2_set_menu_addr(menu_addr);
    _osd2_set_menu_width(menu_width);
    _osd2_set_menu_height(menu_row);
	_osd2_cq_burst(y, x, width, height, color, (index+offset), 1);
#else
	for(h=0; h< height; h++) {
		if((h+height) >= menu_row)
			break;	// out off range
		osd2_set_location_addr(x, y+h,  menu_addr, menu_width);
		for(w=0; w< width; w++) {
			if((w+width) < menu_width)  {
				temp = index+i+offset;
				temp |= (color<<8);
				osd2_set_ram_data(temp);
			}
			i++;
		}
	}
#endif
#ifdef EX_I2C_SAFE_MODE 
	IC_WriteByte(TWIC_P0, 0xF0, SLAVE_WORK_ADDR);
#endif
}

/**
 * @func    _tw_icon_draw_at
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			offset, menu of parameters.
 * @param   *picon		icon structure of point
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
void
_tw_icon_draw_at (struct tw_icon* picon, unsigned short menu_addr, unsigned char menu_width,
				  unsigned char menu_row, unsigned short offset, unsigned char blinking)
{
	unsigned char color= 0;

	color = check_color_config(picon->index, 0, picon->color);

	_icon_draw_at(picon->index, color, picon->x, picon->y, picon->width, picon->height,
		menu_addr, menu_width, menu_row, offset, blinking);
}

/**
 * @func    _icon_draw
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			index, location of parameters.
 * @param   index		font of index
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			blinking	blinking flag
 * @return  none
 */
void
_icon_draw (unsigned short index, unsigned char color, unsigned char x, unsigned char y,
		    unsigned char width, unsigned char height, unsigned char blinking)
{
	_icon_draw_at(index, color, x, y, width, height, CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height, 0, blinking);
}

/**
 * @func    _n_tw_icon_draw
 * @brief   no update, draw(put) character attribute in MENU with ICON structure.
 * @param   *picon		icon structure of point
 * @return  none
 */
void
_n_tw_icon_draw (struct tw_icon* picon)
{
	unsigned char color= 0;

	color = check_color_config(picon->index, 0, picon->color);

	_icon_draw(picon->index, color, picon->x, picon->y, picon->width, picon->height, 0);
}

/**
 * @func    tw_icon_draw
 * @brief   draw(put) character attribute in MENU with ICON structure.
 * @param   *picon		icon structure of point
 * @return  none89056
 */
void
tw_icon_draw (struct tw_icon* picon)
{
	_n_tw_icon_draw(picon);
	osd2_update();
}

/**
 * @func    _n_tw_icon_loc
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
void
_n_tw_icon_loc (struct tw_icon* picon, unsigned char x, unsigned char y)
{
	unsigned char color= 0;

	color = check_color_config(picon->index, 0, picon->color);

	_icon_draw_at(picon->index, color, x, y, picon->width, picon->height,
		CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height, 0, 0);
}

/**
 * @func    tw_icon_loc
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
void
tw_icon_loc (struct tw_icon* picon, unsigned char x, unsigned char y)
{
	_n_tw_icon_loc(picon, x, y);
	osd2_update();
}

/**
 * @func    _n_tw_icon_color_change
 * @brief   no update, draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
void
_n_tw_icon_color_change (struct tw_icon* picon, unsigned char color, unsigned char blanking)
{
	_icon_draw_at(picon->index, color, picon->x, picon->y, picon->width, picon->height,
		CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height, 0, blanking);
}

/**
 * @func    tw_icon_color_change
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
void
tw_icon_color_change (struct tw_icon* picon, unsigned char color, unsigned char blanking)
{
	_n_tw_icon_color_change(picon, color, blanking);
	osd2_update();
}

/**
 * @func    _n_tw_icon_loc_color
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font*
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
void
_n_tw_icon_loc_color (struct tw_icon* picon, unsigned char x, unsigned char y, unsigned char color, unsigned char blanking)
{
	_icon_draw_at(picon->index, color, x, y, picon->width, picon->height,
		CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height, 0, blanking);
}

/**
 * @func    tw_icon_loc_color
 * @brief   draw(put) character attribute in MENU with ICON structure and
 * 			color index and blinking.
 * @param   *picon		icon structure of point
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font*
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
void
tw_icon_loc_color (struct tw_icon* picon, unsigned char x, unsigned char y, unsigned char color, unsigned char blanking)
{
	_n_tw_icon_loc_color(picon, x, y, color, blanking);
	osd2_update();
}

/**
 * @func    _n_tw_icon_1st_color
 * @brief   no update, change to 1st color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
_n_tw_icon_1st_color (struct tw_icon* picon)
{
	_n_tw_icon_draw(picon);
}

/**
 * @func    tw_icon_1st_color
 * @brief   change to 1st color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
tw_icon_1st_color (struct tw_icon* picon)
{
	tw_icon_draw(picon);
}

/**
 * @func    _n_tw_icon_2nd_color
 * @brief   no update, change to 2nd color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
_n_tw_icon_2nd_color (struct tw_icon* picon)
{
	_n_tw_icon_color_change(picon, ((picon->color)>>4), 0);
}

/**
 * @func    tw_icon_2nd_color
 * @brief   change to 2nd color of ICON.
 * @param   *picon		ICON structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
tw_icon_2nd_color (struct tw_icon* picon)
{
	tw_icon_color_change(picon, ((picon->color)>>4), 0);
}

/**
 * @func    _icons_draw_at
 * @brief   draw(put) character attribute in MENU with parameters (table).
 * @param   *table      index table
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
void
_icons_draw_at(unsigned char *table, unsigned char color, unsigned char x, unsigned char y,
               unsigned char width, unsigned char height, unsigned short menu_addr,
               unsigned char menu_width, unsigned char menu_row, unsigned short offset,
               unsigned char blinking)
{
#ifndef TW_USE_CQ
	int rc = 0;
	int w, h = 0;
	int i = 0, temp = 0;

    rc = check_osd2_region(x, y, width, height, menu_width, menu_row);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
#endif

#ifdef EX_I2C_SAFE_MODE
    IC_WriteByte(TWIC_P0, 0xF0, SLAVE_SAFE_ADDR);
#endif

    color <<= 1;    // lut index[8:5]
    if(blinking)
        color |= 0x20;

#ifdef TW_USE_CQ
    _osd2_set_menu_addr(menu_addr);
    _osd2_set_menu_width(menu_width);
    _osd2_set_menu_height(menu_row);
    _osd2_cq_burst3(y, x, width, height, color, table, offset);
#else
    for(h=0; h< height; h++) {
		if((h+height) >= menu_row)
			break;	// out off range
        osd2_set_location_addr(x, y+h,  menu_addr, menu_width);
        for(w=0; w< width; w++) {
			if((w+width) < menu_width) {
                temp = table[i];
                if(temp==0xFF)  /* for Blank ROM Font */
                    temp= 0;
                else {
                    temp = table[i]+offset;
                    temp |= (color<<8);
                }
                osd2_set_ram_data(temp);
            }
            i++;
        }
    }
#endif
#ifdef EX_I2C_SAFE_MODE
    IC_WriteByte(TWIC_P0, 0xF0, SLAVE_WORK_ADDR);
#endif
}

/**
 * @func    _tw_icons_draw_at
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			offset, menu of parameters (table).
 * @param   *picon		ICONS structure of point
 * 			menu_addr	MENU of base address
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * 			offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
void
_tw_icons_draw_at (struct tw_icons* picon, unsigned short menu_addr, unsigned char menu_width,
                   unsigned char menu_row, unsigned short offset, unsigned char blinking)
{
    unsigned char color= 0;

    color = check_color_config(picon->table[0], picon->offset, picon->color);

    _icons_draw_at(picon->table, color, picon->x, picon->y, picon->width, picon->height,
        menu_addr, menu_width, menu_row, picon->offset+offset, blinking);
}

/**
 * @func    _icons_draw
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			index, location of parameters (table).
 * @param   *table      index table
 * 			color		color of index
 * 			x			x coord. of the MENU, unit is font
 * 			y			y coord. of the MENU, unit is font
 * 			width		icon of width, unit is font
 * 			height		icon of height, unit is font
 *          offset		font of offset
 * 			blinking	blinking flag
 * @return  none
 */
void
_icons_draw (unsigned char *table, unsigned char color, unsigned char x, unsigned char y,
             unsigned char width, unsigned char height, unsigned short offset, unsigned char blinking)
{
    unsigned char ccolor= 0;

    ccolor = check_color_config(table[0], offset, color);

    _icons_draw_at(table, ccolor, x, y, width, height, CUR_MENU_P->addr, CUR_MENU_P->width,
        CUR_MENU_P->height, offset, blinking);
}

/**
 * @func    _n_tw_icons_draw
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure.
 * @param   *picon	ICONS structure of point
 * @return  none
 */
void
_n_tw_icons_draw (struct tw_icons* picon)
{
    _icons_draw(picon->table, picon->color, picon->x, picon->y, picon->width,
    	picon->height, picon->offset, 0);
}

/**
 * @func    tw_icons_draw
 * @brief   draw(put) character attribute in MENU with ICONS structure.
 * @param   *picon	ICONS structure of point
 * @return  none
 */
void
tw_icons_draw (struct tw_icons* picon)
{
	_n_tw_icons_draw(picon);
	osd2_update();
}

/**
 * @func    _n_tw_icons_loc
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
void
_n_tw_icons_loc (struct tw_icons* picon, unsigned char x, unsigned char y)
{
	unsigned char color= 0;

	color = check_color_config(picon->table[0], picon->offset, picon->color);

	_icons_draw_at(picon->table, color, x, y, picon->width, picon->height,
        CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height, picon->offset, 0);
}

/**
 * @func    tw_icons_loc
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			location parameters.
 * @param   *picon	icons structure of point
 * 			x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * @return  none
 */
void
tw_icons_loc (struct tw_icons* picon, unsigned char x, unsigned char y)
{
	_n_tw_icons_loc(picon, x, y);
	//osd2_update();
}

/**
 * @func    _n_tw_icons_color_change
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure and
 * 			color index and blinking.
 * @param   *picon		icons structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
void
_n_tw_icons_color_change (struct tw_icons* picon, unsigned char color, unsigned char blanking)
{
    _icons_draw_at(picon->table, color, picon->x, picon->y, picon->width, picon->height,
        CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height, picon->offset, blanking);
}

/**
 * @func    tw_icons_color_change
 * @brief   draw(put) character attribute in MENU with ICONS structure and
 * 			color index and blinking.
 * @param   *picon		icons structure of point
 * 			color		color of index
 * 			blinking	blinking flag
 * @return  none
 */
void
tw_icons_color_change (struct tw_icons* picon, unsigned char color, unsigned char blanking)
{
	_n_tw_icons_color_change(picon, color, blanking);
	osd2_update();
}

/**
 * @func    _n_tw_icons_1st_color
 * @brief   no update, change to 1st color of ICONS.
 * @param   *picon	ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
_n_tw_icons_1st_color (struct tw_icons* picon)
{
	_n_tw_icons_draw(picon);
}

/**
 * @func    tw_icons_1st_color
 * @brief   change to 1st color of ICONS.
 * @param   *picon	ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
tw_icons_1st_color (struct tw_icons* picon)
{
	tw_icons_draw(picon);
}

/**
 * @func    _n_tw_icons_2nd_color
 * @brief   no update, change to 2nd color of ICONS.
 * @param   *picon		ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
_n_tw_icons_2nd_color (struct tw_icons* picon)
{
	_n_tw_icons_color_change(picon, ((picon->color)>>4), 0);
}

/**
 * @func    tw_icons_1st_color
 * @brief   change to 2nd color of ICONS.
 * @param   *picon		ICONS structure of point
 * @return  none
 * @note	for 2BP/4BP Font
 */
void
tw_icons_2nd_color (struct tw_icons* picon)
{
	tw_icons_color_change(picon, ((picon->color)>>4), 0);
}

static unsigned short
check_gicon_id (unsigned short id)
{
	unsigned char type = 0;
	unsigned short icon_id = 0;
	struct res_tile_t tile_s;

	type = rc_type(id);
	if(type == TYPE_TILE) {
        /* get current ICON SET of group ICON */
        dbg(2, "Group ICON + Changeable!\n\r");
        get_res_tile(id, &tile_s);
        if((CUR_ICONSET_NUM > tile_s.length)||(CUR_ICONSET_NUM <= 0))
			CUR_ICONSET_NUM= 1;
        memcpy(&icon_id, (char*)(tile_s.base_addr+(2*(CUR_ICONSET_NUM-1))), sizeof(unsigned short));
        endian_inv(icon_id);
    } else if(type == TYPE_ICON) {
        icon_id = id;
    } else {
        ERROR("group icon of ID type is error!\n\r");
        return NULL;
    }

    return icon_id;
}

/**
 * @func    _n_tw_gicon_load
 * @brief   no update, load character in MENU with ICON structure.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
void
_n_tw_gicon_load(struct tw_gicon* pgicon)
{
	unsigned short icon_id = 0;
	unsigned short tmp_addr = 0;

    icon_id = check_gicon_id(pgicon->gicon_id);
    if(!icon_id)
        return;

    tmp_addr = pgicon->oram_addr;
    /* dynamic load ICON to OSDRAM */
    tw_load_icon(icon_id, &tmp_addr);
}

/**
 * @func    _n_tw_gicon_draw
 * @brief   no update, draw(put) character attribute in MENU with ICON structure.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
void
_n_tw_gicon_draw(struct tw_gicon* pgicon)
{
	unsigned short icon_id = 0;
	unsigned char color = 0;
	unsigned short tmp_addr = 0;

    icon_id = check_gicon_id(pgicon->gicon_id);
    if(!icon_id)
        return;

    tmp_addr = pgicon->oram_addr;
    /* dynamic load ICON to OSDRAM */
    tw_load_icon(icon_id, &tmp_addr);

	color = check_color_config(pgicon->index, 0, pgicon->color);
	_icon_draw(pgicon->index, color, pgicon->x, pgicon->y, pgicon->width, pgicon->height, 0);
}

/**
 * @func    _n_tw_gicon_loc
 * @brief   no update, draw(put) character attribute in MENU 
 *          with ICON structure and location parameters.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
void
_n_tw_gicon_loc(struct tw_gicon* pgicon, 
                unsigned char x, unsigned char y, unsigned char blank)
{
	unsigned short icon_id = 0;
	unsigned char color = 0;
	unsigned short tmp_addr = 0;

    icon_id = check_gicon_id(pgicon->gicon_id);
    if(!icon_id)
        return;

    tmp_addr = pgicon->oram_addr;
    /* dynamic load ICON to OSDRAM */
    tw_load_icon(icon_id, &tmp_addr);

	color = check_color_config(pgicon->index, 0, pgicon->color);
	_icon_draw(pgicon->index, color, x, y, pgicon->width, pgicon->height, blank);
}

/**
 * @func    tw_gicon_draw
 * @brief   draw(put) character attribute in MENU with ICON structure.
 * @param   *gpicon	Group ICON structure of point
 * @return  none
 */
void
tw_gicon_draw(struct tw_gicon* pgicon)
{
	_n_tw_gicon_draw(pgicon);
	osd2_update();
}

/**
 * @func    _n_tw_gicons_draw
 * @brief   no update, draw(put) character attribute in MENU with ICONS structure.
 * @param   *gpicon	Group ICONS structure of point
 * @return  none
 */
void
_n_tw_gicons_draw (struct tw_gicons* pgicon)
{
	unsigned short icon_id = 0;
	unsigned short tmp_addr = 0;

    icon_id = check_gicon_id(pgicon->gicons_id);
    if(!icon_id)
        return;

    tmp_addr = pgicon->oram_addr;
    /* dynamic load ICON to OSDRAM */
    tw_load_icon(icon_id, &tmp_addr);

	_icons_draw(pgicon->table, pgicon->color,pgicon->x, pgicon->y,
			pgicon->width, pgicon->height, pgicon->offset, 0);
}

/**
 * @func    tw_gicons_draw
 * @brief   draw(put) character attribute in MENU with ICONS structure.
 * @param   *gpicon	Group ICONS structure of point
 * @return  none
 */
void
tw_gicons_draw (struct tw_gicons* pgicon)
{
	_n_tw_gicons_draw(pgicon);
	osd2_update();
}

/**
 * @func    tw_osd2_menu_write
 * @brief   draw(put) a character attribute in MENU.
 * @param  	x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * @return  none
 */
void
tw_osd2_menu_write (unsigned char x, unsigned char y, unsigned short index, unsigned char color)
{
	int rc = 0;

	rc = check_osd2_region(x, y, 1, 1, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_menu_write(x, y, ((color<<8)|index), CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_osd2_menu_write_line
 * @brief   draw(put) a ROW of characters attribute in MENU.
 * @param  	y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * @return  none
 */
void
tw_osd2_menu_write_line (unsigned char y, unsigned short index, unsigned char color)
{
	int rc = 0;

	rc = check_osd2_region(0, y, CUR_MENU_P->width, 1, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_menu_write_line(y, ((color<<8)|index), CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_osd2_menu_write_block
 * @brief   draw(put) a ROW of characters attribute in MENU.
 * @param  	x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * 			width	icon of width, unit is font
 * 			height	icon of height, unit is font
 * @return  none
 */
void
tw_osd2_menu_write_block (unsigned char x, unsigned char y, unsigned char width,
	unsigned char height, unsigned short index, unsigned char color)
{
	int rc = 0;

	rc = check_osd2_region(x, y, width, height, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_menu_write_block(x, y, width, height, ((color<<8)|index), CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_osd2_clear_char
 * @brief   clear(put blank font) a character attribute in MENU.
 * @param  	x	x coord. of the MENU, unit is font
 * 			y	y coord. of the MENU, unit is font
 * @return  none
 */
void
tw_osd2_clear_char (unsigned char x, unsigned char y)
{
	int rc = 0;

	rc = check_osd2_region(x, y, 1, 1, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_clear_char(x, y, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_osd2_clear_line
 * @brief   clear(put blank font) a ROW of characters attribute in MENU.
 * @param  	y	y coord. of the MENU, unit is font
 * @return  none
 */
void
tw_osd2_clear_line (unsigned char y)
{
	int rc = 0;

	rc = check_osd2_region(0, y, CUR_MENU_P->width, 1, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_clear_line(y, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_osd2_menu_write_block
 * @brief   draw(put) a ROW of characters attribute in MENU.
 * @param  	x		x coord. of the MENU, unit is font
 * 			y		y coord. of the MENU, unit is font
 * 			index	font of index
 * 			color	color of index
 * 			width	icon of width, unit is font
 * 			height	icon of height, unit is font
 * @return  none
 */
void
tw_osd2_clear_block (unsigned char x, unsigned char y, unsigned char width, unsigned char height)
{
	int rc = 0;

	rc = check_osd2_region(x, y, width, height, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_clear_block(x, y, width, height, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_clear_icon
 * @brief   clear a block in MENU with ICON.
 * @param  	*picon	ICON structure of point
 * @return  none
 */
void
tw_clear_icon (struct tw_icon* picon)
{
	int rc = 0;

	rc = check_osd2_region(picon->x, picon->y, picon->width, picon->height, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_clear_block(picon->x, picon->y, picon->width, picon->height, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_clear_icons
 * @brief   clear a block in MENU with ICON.
 * @param  	*picon	ICONS structure of point
 * @return  none
 */
void
tw_clear_icons (struct tw_icons* picon)
{
	int rc = 0;

	rc = check_osd2_region(picon->x, picon->y, picon->width, picon->height, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_clear_block(picon->x, picon->y, picon->width, picon->height, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_clear_txt
 * @brief   clear a block in MENU with ICON.
 * @param  	*ptxt	TXT structure of point
 * @return  none
 */
void
tw_clear_txt (struct tw_txt* ptxt)
{
	int rc = 0;

	rc = check_osd2_region(ptxt->x, ptxt->y, ptxt->width, ptxt->height, CUR_MENU_P->width, CUR_MENU_P->height);
	if(rc) {
		ERROR("out of range!\n\r");
		return;
	}
	osd2_clear_block(ptxt->x, ptxt->y, ptxt->width, ptxt->height, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    _tw_clear_icon_at
 * @brief   clear a block in MENU with ICON.
 * @param  	*picon	ICON structure of point
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  none
 */
void
_tw_clear_icon_at (struct tw_icon* picon, unsigned short menu_addr, unsigned char menu_width)
{
    osd2_clear_block(picon->x, picon->y, picon->width, picon->height, menu_addr, menu_width);
}

/**
 * @func    _tw_clear_icons_at
 * @brief   clear a block in MENU with ICONS.
 * @param  	*picon	ICONS structure of point
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  none
 */
void
_tw_clear_icons_at (struct tw_icons* picon, unsigned short menu_addr, unsigned char menu_width)
{
    osd2_clear_block(picon->x, picon->y, picon->width, picon->height, menu_addr, menu_width);
}

/**
 * @func    _tw_clear_txt_at
 * @brief   clear a block in MENU with ICON.
 * @param  	*ptxt	TXT structure of point
 * 			menu_width	MENU of width (ROW of characters)
 * 			menu_row	MENU of height (ROW)
 * @return  none
 */
void
_tw_clear_txt_at (struct tw_txt* ptxt, unsigned short menu_addr, unsigned char menu_width)
{
    osd2_clear_block(ptxt->x, ptxt->y, ptxt->width, ptxt->height, menu_addr, menu_width);
}

/**
 * @func    tw_osd2_clear_menu
 * @brief   clear(put blank index) OSD2 MENU.
 * @param  	none
 * @return  none
 */
void
tw_osd2_clear_menu (void)
{
	osd2_clear_block(0, 0, CUR_MENU_P->width, CUR_MENU_P->height, CUR_MENU_P->addr, CUR_MENU_P->width);
}

/**
 * @func    tw_osd2_menu_start_h
 * @brief	set a specified ROW of H start position of the MENU.
 * @param   x		x coord. of the display, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			row		specified ROW of the MENU
 * @return  none
 */
void
tw_osd2_menu_start_h (unsigned short x)
{
	osd2_menu_start_h(x, CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height);
}

/**
 * @func    tw_osd2_menu_location
 * @brief	set OSD2 MENU of location.
 * @param   x		x coord. of the MENU, unit is pixel
 * 			y		y coord. of the MENU, unit is pixel
 * 			addr	MENU of base address
 * 			width	MENU of width (ROW of characters)
 * 			height	MENU of height (ROW)
 * @return  none
 */
void
tw_osd2_menu_location (unsigned short x, unsigned short y)
{
	osd2_menu_location(x, y, CUR_MENU_P->addr, CUR_MENU_P->width, CUR_MENU_P->height);
}




/**
 * @func    _tw_is_half_alphabet
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_is_half_alphabet()
{
	return spi_read_byte(CUR_TOF_RES.base_addr + 0x54L);
}

/* T123 OSD Font Drawing Info Table         24x24 32x31 32x36 48x48(16x24) 48x48(12x24)  rev */
unsigned short const TOF_FONT_SPLIT_W[]    = { 2,    2,    2,    3,           4,           1};
unsigned short const TOF_FONT_SPLIT_H[]    = { 1,    1,    2,    2,           2,           2};
unsigned short const TOF_FONT_SPLIT_HSA[]  = { 1,    1,    1,    2,           2,           1};

#define SINGLE_PIX 0
#define SUB_PIX    1 

#define GB_ERROR_CODE 0xFFFF

/**
 * @func    _tw_half_alphabet_num
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_half_alphabet_num()
{
	return spi_read_byte(CUR_TOF_RES.base_addr + 0x54L);
}

/**
 * @func    _tw_get_font_size_id
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_get_font_size_id()
{
	return spi_read_byte(CUR_TOF_RES.base_addr + 0x55L);	
}

/**
 * @func    _tw_get_file_type
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_get_file_type() {
	return spi_read_byte(CUR_TOF_RES.base_addr + 0x01L);
}

/**
 * @func    _Unicode_len
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned short _Unicode_len(const unsigned short *ucs) {
	int i;

	for(i=0; ucs[i]!= 0 && i<0xFFFF; i++) {	
	}

	return i;
}

/**
 * @func    _GB_code_len
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned short _GB_code_len(const unsigned short *ucs) {
	unsigned char *gbs =(unsigned char *) ucs;
	unsigned short len = 0;
	int i;

	for(i=0, len=0; gbs[i]!=0 && len<GB_ERROR_CODE; len++) {
		if(gbs[i] >= 0xA1) { 
			if(gbs[i+1] < 0xA0) {
				len = GB_ERROR_CODE - 1;
			}
			i+=2;
		}
		else {	
			i++;
		}		
	}

	if(len == GB_ERROR_CODE) { 
		ERROR("Fail To Get GB String Length!\n");
	}
		 
	return len;
}

/**
 * @func    _get_offset_GBS
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned short _get_offset_GBS(unsigned short *ucs, unsigned short n) {	 
	unsigned char *gbs =(unsigned char *) ucs;
	int i, idx;

	for(i=0, idx = 0;  gbs[idx]!=0 && i<0xFFFF; i++) { 
		if(i == n) {
			break;	
		}
		if(gbs[idx] >= 0xA1) {  
			idx+=2;
		}
		else {
			idx++;
		}		
	}
	
	if(i == 0xFFFF) {  
		idx = GB_ERROR_CODE;
		ERROR("Fail To Get GB String Offset!\n");
	}	  

	return idx;
}

/**
 * @func    _get_code_from_GBS
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned short _get_code_from_GBS(const unsigned short *ucs, unsigned short n) {	 
	unsigned char *gbs =(unsigned char *) ucs;
	unsigned short cod = GB_ERROR_CODE;
	int i, idx;

	for(i=0, idx=0; gbs[idx]!=0 && i<0xFFFF; i++) { 
		if(i == n) {
			if(gbs[idx] >= 0xA1) {  
				cod = gbs[idx+1L];
				cod += (gbs[idx]<<8L);
			}
			else {				
				cod = gbs[idx];
			}	
			break;	
		}
		if(gbs[idx] >= 0xA1) {  
			idx+=2;
		}
		else {
			idx++;
		}		
	}
	
	if(cod == GB_ERROR_CODE) {
		ERROR("Fail To Get GB Code!\n");
	}	  

	return cod;
}

/**
 * @func    _tw_get_font_quality
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_get_font_quality()
{	
	return spi_read_byte(CUR_TOF_RES.base_addr + 0x50L);
}

/**
 * @func    _tw_get_draw_len
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_get_draw_len(unsigned short *ucs, unsigned char width, unsigned char size_id)
{
	unsigned char offset, i;
	unsigned char is_half_alphabet = (_tw_half_alphabet_num() > 0);

	unsigned char is_gb = ((_tw_get_file_type() == 'G') ? 1 : 0);	
	unsigned short len = ((is_gb) ? _GB_code_len(ucs) : _Unicode_len(ucs));
	unsigned short uc;

	

	offset = 0;
	for(i=0; i<len; i++) {
		uc = ((is_gb) ? _get_code_from_GBS(ucs, i) : ucs[i]);

		if(is_half_alphabet && uc < 256)
			offset+=TOF_FONT_SPLIT_HSA[size_id];
		else {	
			offset+=TOF_FONT_SPLIT_W[size_id];
		}

		if(offset > width)
			break;
	}

	return i;	
}

/**
 * @func    _romfont_draw_at
 * @brief   .
 * @param   none
 * @return  none
 */
#define FONT_ASCII_OFFSET   10
void _romfont_draw_at(struct tw_txt* ptxt, unsigned char x, unsigned char y, const unsigned char *ucs, unsigned short menu_addr, unsigned char menu_width, unsigned char color, unsigned char blink)
{
    unsigned char i;
    unsigned short temp= 0;
                
    if(blink)
        color |= 0x20;
                                            
    osd2_set_location_addr(x, y, menu_addr, menu_width);
                        
    for(i=0; ucs[i]!=0; i++) {
        if((x+i)>=CUR_MENU_P->width) 
            break;
        if(i>=ptxt->width)
            break;
        temp = (color<<8);
        if(ucs[i]==' ') //BLANK
            OSD_SET_RAM_DATA(temp);
        else if(ucs[i]=='^') {
            temp |= (ucs[i]+ptxt->index+2-FONT_ASCII_OFFSET);
            OSD_SET_RAM_DATA(temp);
        } else {
            temp |= (ucs[i]+ptxt->index-FONT_ASCII_OFFSET);
            OSD_SET_RAM_DATA(temp);
        }
    }
}

/**
 * @func    _romfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
void _romfont_draw(struct tw_txt* ptxt, unsigned char x, unsigned char y, const unsigned char *ucs, unsigned char color, unsigned char blink)
{                           
    _romfont_draw_at(ptxt, x, y, ucs, CUR_MENU_P->addr, CUR_MENU_P->width, color, blink);    
}

/**
 * @func    tw_romfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_romfont_draw(struct tw_txt* ptxt, const unsigned char *ucs)
{                           
    _romfont_draw_at(ptxt, ptxt->x, ptxt->y, ucs, CUR_MENU_P->addr, CUR_MENU_P->width, ((ptxt->fg_color)<<1) + ((ptxt->bg_color)<<6), 0);    
}
/**
 * @func    _twfont_draw_at
 * @brief   .
 * @param   none
 * @return  none
 */
void _twfont_draw_at(struct tw_txt* ptxt, unsigned char x, unsigned char y, unsigned short *ucs, unsigned short menu_addr, unsigned char menu_width, unsigned char color, unsigned char blink)
{
	unsigned char size_id, font_quality;	
							   
	font_quality = _tw_get_font_quality();
	size_id      = _tw_get_font_size_id();	
	tw_menu_reset_index_n(ptxt->index);	
	tw_menu_draw_nstring(x, y, ptxt->width, ucs, menu_addr, menu_width, (blink?(color|0x10):color), size_id, font_quality!=SINGLE_PIX);	
}

/**
 * @func    _twfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
void _twfont_draw(struct tw_txt* ptxt, unsigned char x, unsigned char y, unsigned short *ucs, unsigned char color, unsigned char blink)
{   
    _twfont_draw_at(ptxt, x, y, ucs, CUR_MENU_P->addr, CUR_MENU_P->width, color, blink);      
}

/**
 * @func    tw_twfont_draw
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_twfont_draw(struct tw_txt* ptxt, unsigned short *ucs)
{ 
	_twfont_draw_at(ptxt, ptxt->x, ptxt->y, ucs, CUR_MENU_P->addr, CUR_MENU_P->width, ((ptxt->fg_color)<<0) + ((ptxt->bg_color)<<5), 0);
}

/**
 * @func    tw_text_draw
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_text_draw(struct tw_txt* ptxt, unsigned short *ucs)
{   
    if(ptxt->icon_id==0) {
        /* ROM FONT */
        tw_romfont_draw(ptxt, (unsigned char*)ucs);
    } else {
        /* TWFONT */
        tw_twfont_draw(ptxt, ucs);  
    }
	
}

/**
 * @func    tw_text_color_change
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_text_color_change(struct tw_txt* ptxt, unsigned short *ucs, unsigned char color)
{
    if(ptxt->icon_id==0) {
    /* ROM FONT */
        _romfont_draw(ptxt, ptxt->x, ptxt->y, (unsigned char*)ucs, color, 0);    
    } else {
        /* TWFONT */
        _twfont_draw(ptxt, ptxt->x, ptxt->y, ucs, color, 0);      
    }
}

/**
 * @func    _tw_draw_char_at
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char _tw_draw_char_at(unsigned char x, unsigned char y, unsigned char w, unsigned h, 
				          unsigned short idx, unsigned char color, unsigned short menu_addr, unsigned char menu_width)
{
	unsigned char i, j;
	
	for(j=0; j<w; j++) {  
		for(i=0; i<h; i++) {
			osd2_menu_write(x+j, y+i, ((color<<8)|(idx+i+j*h)), menu_addr, menu_width);
		}
	}

	return w*h;
}

void _tw_burst_draw_line(unsigned char x, unsigned char y, unsigned char w, unsigned h, 
				          unsigned short idx, unsigned char color, unsigned short menu_addr, unsigned char menu_width)
{
	unsigned char i, j;
	unsigned short dat;

#if _CQ_DRAW_FONT_MENU
    if(osd_en_status()){
        for(i= 0; i<h; i++){
            osd2_set_location_addr(x, y+i, menu_addr, menu_width);
            _osd_cq_tw_font(color, idx, w, h, i);
        }
    }else{
#endif
        for(i=0; i<h; i++) {		
    		osd2_set_location_addr(x, y+i, menu_addr, menu_width);
    		for(j=0; j<w; j++) { 
    			dat = (color<<9) + idx + j*h + i; 
    			osd2_set_ram_data(dat);	
    		}
    	}
#if _CQ_DRAW_FONT_MENU
    }
    osd2_update();
#endif
}

/**
 * @func    tw_menu_draw_nstring
 * @brief   .
 * @param   none
 * @return  none
 */
unsigned char tw_menu_draw_nstring(unsigned char x, unsigned char y, unsigned char max_w, const unsigned short *ucs, unsigned short menu_addr, unsigned char menu_width, 
                                       unsigned char color, unsigned char size_id, unsigned char is2BP)
{
	unsigned short i;
	unsigned short font_size;  
	unsigned char is_hsa = (_tw_half_alphabet_num() > 0);
	unsigned short real_oaddr;
	unsigned char bp = is2BP ? 2 : 1;

	unsigned char is_gb = ((_tw_get_file_type()=='G') ? 1 : 0);	  
	unsigned short len = ((is_gb) ? _GB_code_len(ucs) : _Unicode_len(ucs));
	unsigned short uc;
		
	unsigned char w_count = 0;
	unsigned char w_sum = 0;
	unsigned short temp_idx = menu_string_start_index;

	font_size = (CUR_MENU_P->font_w*CUR_MENU_P->font_h)/16;
					
   	for(i=0; (i<len)&&(w_sum<=max_w); i++) { 
		uc = ((is_gb) ? _get_code_from_GBS(ucs, i) : ucs[i]);
		real_oaddr = (is2BP) ? (CUR_2BP_IDX*font_size + (menu_string_start_index-CUR_2BP_IDX)*font_size*2) : font_size*menu_string_start_index;  
	    if(is_hsa) {
			w_count = osd2_put_twf_char_to_oram(CUR_TOF_RES.flash_addr, CUR_TOF_RES.base_addr, real_oaddr, uc, TOF_FONT_SPLIT_H[size_id]*font_size*2*bp, is_hsa);  
			menu_string_start_index += w_count* TOF_FONT_SPLIT_H[size_id]; 
			w_sum += w_count;	
		}
		else {																				   
			osd2_put_twf_char_to_oram(CUR_TOF_RES.flash_addr, CUR_TOF_RES.base_addr, real_oaddr, uc, font_size*TOF_FONT_SPLIT_W[size_id]*TOF_FONT_SPLIT_H[size_id]*2*bp, is_hsa); 
			menu_string_start_index += TOF_FONT_SPLIT_W[size_id]*TOF_FONT_SPLIT_H[size_id]; 
			w_sum += TOF_FONT_SPLIT_W[size_id];
		}
	}
	
	if(w_sum > max_w) {
		w_sum = max_w;
	}	
	 			
	_tw_burst_draw_line(x, y, w_sum, TOF_FONT_SPLIT_H[size_id], temp_idx, color, menu_addr, menu_width);
	
	return i;	
}

/**
 * @func    tw_menu_row_moveV
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_menu_row_moveV(unsigned char row, unsigned short y)
{
	unsigned short dat = 0x4000 + y;
	unsigned short address = CUR_MENU_P->addr + ((CUR_MENU_P->width + 4) * row); 
												
	IC_WriteByte(0x00, 0xAA, (unsigned char)(address & 0xff));
	IC_WriteByte(0x00, 0xAA, (unsigned char)(address>>8));
				  
	osd2_set_ram_data(dat);
}

/**
 * @func    tw_menu_reset_index
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_menu_reset_index()
{
	menu_string_start_index = 0;
}

/**
 * @func    tw_menu_reset_index_n
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_menu_reset_index_n(unsigned char idx)
{
	menu_string_start_index = idx;	
}

/**
 * @func    tw_menu_fill
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_menu_fill(unsigned char x, unsigned char y, unsigned char dat ,unsigned char color, unsigned char length)
{
	unsigned char i;

	for(i=0; i< length; i++) {
		osd2_menu_write(x+i, y, ((color<<8)|dat), CUR_MENU_P->addr, CUR_MENU_P->width);
	}		 
}

struct tw_scl_txt _init_scrol(struct tw_txt* ptxt, unsigned short *ucs, unsigned char direct, unsigned char tail_blank, unsigned char is2bp) {
 	struct tw_scl_txt ret;
	unsigned char is_half_alphabet = (_tw_half_alphabet_num() > 0); 	
	unsigned short font_size = CUR_MENU_P->font_w*CUR_MENU_P->font_h/16;  
	unsigned char i;
	unsigned char size_id = _tw_get_font_size_id();
	unsigned char w_count;
	unsigned char bp = (is2bp) ? 2 : 1;
    unsigned short real_oaddr;
					 
	tw_menu_reset_index_n(ptxt->index);
	for(i=0; ucs[i]!=0 && i < 0xff; i++) {
        real_oaddr = (is2bp) ? (CUR_2BP_IDX*font_size + (menu_string_start_index-CUR_2BP_IDX)*font_size*2) : font_size*menu_string_start_index;  
	    if(is_half_alphabet && ucs[i] < 256) {
			w_count = osd2_put_twf_char_to_oram(CUR_TOF_RES.flash_addr, CUR_TOF_RES.base_addr, real_oaddr, ucs[i],
									     font_size*TOF_FONT_SPLIT_H[size_id]*2*bp, is_half_alphabet);   
			menu_string_start_index += w_count*TOF_FONT_SPLIT_H[size_id];
		}
		else {																				   
			osd2_put_twf_char_to_oram(CUR_TOF_RES.flash_addr, CUR_TOF_RES.base_addr, real_oaddr, ucs[i], 
									     font_size*TOF_FONT_SPLIT_W[size_id]*TOF_FONT_SPLIT_H[size_id]*2*bp, is_half_alphabet); 
			menu_string_start_index += TOF_FONT_SPLIT_W[size_id]*TOF_FONT_SPLIT_H[size_id];
		}
	}
    real_oaddr = (is2bp) ? (CUR_2BP_IDX*font_size + (menu_string_start_index-CUR_2BP_IDX)*font_size*2) : font_size*menu_string_start_index;   	 
	osd2_put_twf_char_to_oram(CUR_TOF_RES.flash_addr, CUR_TOF_RES.base_addr, real_oaddr, 0x20, font_size*2*bp, is_half_alphabet); 

    
  
	ret.total_len = (menu_string_start_index - ptxt->index) / TOF_FONT_SPLIT_H[size_id];	
  
	ret.direct = direct;  
	ret.tail_blank = tail_blank; 
	ret.cur_index = ptxt->index; 
	ret.size_id = size_id; 
	ret.ptxt = ptxt;	
 
	tw_scrolling(&ret);
	
	return ret;	
}






/**
 * @func    tw_init_scrol
 * @brief   .
 * @param   none
 * @return  none
 */
struct tw_scl_txt tw_init_scrol(struct tw_txt* ptxt, unsigned short *ucs, unsigned char direct, unsigned char tail_blank)
{
	return _init_scrol(ptxt, ucs, direct, tail_blank, 1);								
}



/**
 * @func    tw_scrolling
 * @brief   .
 * @param   none
 * @return  none
 */
void tw_scrolling(struct tw_scl_txt* scl_txt)
{						  
	unsigned char color = (scl_txt->ptxt->fg_color<<1L) + (scl_txt->ptxt->bg_color<<6L);
	unsigned short i, j, dW;  
	unsigned short dat;
	unsigned char split_h = TOF_FONT_SPLIT_H[scl_txt->size_id];
 
	unsigned short max_index = scl_txt->ptxt->index + scl_txt->total_len * split_h;	  	
	unsigned short color_h = (color<<8L);
	unsigned char blank_index_count = scl_txt->tail_blank*split_h;
	//unsigned short disp_index_count = scl_txt->ptxt->width*split_h;	 


    

    
	for(i=0; i<scl_txt->ptxt->width; i++) {
		for(j=0; j<split_h; j++) {			
			if(scl_txt->cur_index+i*split_h > max_index) {
				dW = (scl_txt->cur_index+i*split_h) - max_index;
				if(dW < blank_index_count) {
					dat = color_h + 0;	
					osd2_set_location_addr(scl_txt->ptxt->x+i, scl_txt->ptxt->y+j, CUR_MENU_P->addr, CUR_MENU_P->width);
					osd2_set_ram_data(dat);
               
					
				}
				else {
					dW = dW - blank_index_count;	  
					dat = color_h + scl_txt->ptxt->index+dW+j;
					osd2_set_location_addr(scl_txt->ptxt->x+i, scl_txt->ptxt->y+j, CUR_MENU_P->addr, CUR_MENU_P->width);
					osd2_set_ram_data(dat);	
                    
				}
			}
			else {	 
				dat = color_h + scl_txt->cur_index + i*split_h+j;	
				osd2_set_location_addr(scl_txt->ptxt->x+i, scl_txt->ptxt->y+j, CUR_MENU_P->addr, CUR_MENU_P->width);
				osd2_set_ram_data(dat);
                
			}
		}
	}

	if(scl_txt->direct) {
		scl_txt->cur_index += split_h;	
		scl_txt->cur_index = scl_txt->ptxt->index + (scl_txt->cur_index - scl_txt->ptxt->index)%((scl_txt->total_len + scl_txt->tail_blank)*split_h);
	}
	else {
		scl_txt->cur_index = (scl_txt->cur_index < split_h) ? 0 : scl_txt->cur_index - split_h;
		if(scl_txt->cur_index < scl_txt->ptxt->index) {
			scl_txt->cur_index = scl_txt->ptxt->index + (scl_txt->total_len + scl_txt->tail_blank - 1)*split_h;	
		}	
	}	
}

/* tw widget system of canvas point */
static struct tw_canvas tw_cvs;

#define PIEXL2BUF_POS(X, Y, W, BP)		((X + (Y * W)) / (8 / BP))
#define PIEXL_SHIFT_BIT(X, Y, W, BP)	((((8/BP) - 1) - ((X + (Y * W)) % (8 / BP))) * BP)
#define WORD1_LSB	0
#define WORD1_MSB	1
#define WORD2_LSB	2
#define WORD2_MSB	3
#define WORD3_LSB	4
#define WORD3_MSB	5
static void
_canvas_12w_update (void)
{
	int i = 0, j = 0, k = 0, x = 0, y = 0, pos = 0;
	int w_num = 0, h_num = 0;
	unsigned char fmsb = 0, flsb = 0;
	unsigned char com[3 * 2] = { 0 };
	char *dbuf	= tw_cvs.dbuf;

	w_num = (tw_cvs.width/tw_cvs.font_w);
	h_num = (tw_cvs.height/tw_cvs.font_h);
	/* set OSDRAM of point address */
	osd2_set_ram_addr(tw_cvs.addr);
	for(j = 0; j < h_num; j++) {
		for(i = 0; i < w_num; i++) {
			for(k = 0; k < tw_cvs.font_h; k++) {
				/* get font every line start of word index */
				x = (i * tw_cvs.font_w);		// x position (pixel)
				y = (k + (j * tw_cvs.font_h));	// y position (pixel)
				pos = PIEXL2BUF_POS(x, y, tw_cvs.width, tw_cvs.bpp);
				if(tw_cvs.bpp == 1) {
					switch(i % 2) {
					case 0:
						fmsb = dbuf[pos];
						flsb = (dbuf[pos + 1] & 0xF0);
						break;
					case 1:
						fmsb = (dbuf[pos] << 4) | (dbuf[pos + 1] >> 4);
						flsb = (dbuf[pos + 1] << 4);
						break;
					}
					/* 1bp */
					switch(k % 4) {
					case 0:
						com[WORD1_MSB] = fmsb;
						com[WORD1_LSB] = (flsb & 0xF0);
						break;
					case 1:
						com[WORD1_LSB] |= (fmsb >> 4);
						com[WORD2_MSB] = (fmsb << 4) | (flsb >> 4);
						/* word access, LSB first, then MSB byte */
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, com[WORD1_LSB]);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD1_MSB]);
						usleep(1);
						if((k + 1) == tw_cvs.font_h) {
							IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, 0x00);
							IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD2_MSB]);
							usleep(1);
						}
						break;
					case 2:
						com[WORD2_LSB] = fmsb;
						com[WORD3_MSB] = flsb;
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, com[WORD2_LSB]);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD2_MSB]);
						usleep(1);
						break;
					case 3:
						com[WORD3_MSB] |= (fmsb >> 4);
						com[WORD3_LSB] = (fmsb << 4) | (flsb >>4);
						/* word access, LSB first, then MSB byte */
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, com[WORD3_LSB]);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD3_MSB]);
						usleep(1);
						break;
					}
				} else {
					/* 2bp */
					switch(k % 2) {
					case 0:
						com[WORD1_MSB] = dbuf[pos];
						com[WORD1_LSB] = dbuf[pos + 1];
						com[WORD2_MSB] = dbuf[pos + 2];
						/* word access, LSB first, then MSB byte */
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, com[WORD1_LSB]);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD1_MSB]);
						usleep(1);
						break;
					case 1:
						com[WORD2_LSB] = dbuf[pos];
						com[WORD3_MSB] = dbuf[pos + 1];
						com[WORD3_LSB] = dbuf[pos + 2];
						/* word access, LSB first, then MSB byte */
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, com[WORD2_LSB]);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD2_MSB]);
						usleep(1);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, com[WORD3_LSB]);
						IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, com[WORD3_MSB]);
						usleep(1);
						break;
					}
				}
			}
		}
	}
}

#if 0
extern void
osd2_cq_oram_cut (int font_w, int font_h, int bp, int width, int height,
	int addr, unsigned char *dbuf, int size);
#endif
//#define REG_SCALAR_BASE 0xb0400000
static void
_canvas_16w_update (void)
{
#if 0
	osd2_cq_oram_cut(tw_cvs.font_w, tw_cvs.font_h, tw_cvs.bpp,
		tw_cvs.width, tw_cvs.height, tw_cvs.addr, tw_cvs.dbuf, tw_cvs.size);
#else
	int i = 0, j = 0, k = 0, idx = 0;
	int w_num = 0, h_num = 0;
	char *dbuf	= tw_cvs.dbuf;
//	unsigned long Address = REG_SCALAR_BASE + (OSD_RAM_DH_REG << 4);

	w_num = (tw_cvs.width/tw_cvs.font_w);
	h_num = (tw_cvs.height/tw_cvs.font_h);

	/* set OSDRAM of point address */
	osd2_set_ram_addr(tw_cvs.addr);
	for(j = 0; j < h_num; j++) {
		for(i = 0; i < w_num; i++) {
			for(k = 0; k < tw_cvs.font_h; k++) {
				/* get font every line start of word index */
				idx = i + (k * w_num);	// font no.
				idx += ((j * tw_cvs.font_h) * w_num);	// + font total word
				idx *= (2 * tw_cvs.bpp);	// word to byte
				/* word access, LSB first, then MSB byte */
				IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, dbuf[idx + 1]);
				IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, dbuf[idx]);
				if(tw_cvs.bpp == 2) {
					/* word access, LSB first, then MSB byte */
					IC_WRITEBYTE(TWIC_P0, OSD_RAM_DL_REG, dbuf[idx + 3]);
					IC_WRITEBYTE(TWIC_P0, OSD_RAM_DH_REG, dbuf[idx + 2]);
				}
//				(*(volatile unsigned int *)(Address)) = dbuf[(idx * 2) + 1];
//				(*(volatile unsigned int *)(Address)) = dbuf[(idx * 2)];
			}
		}
	}
#endif
}

/**
 * @func    _get_point
 * @brief   get point from canvas
 * @param   x		x position
 * 			y		y position
 * 			dbuf	data buffer of point
 * 			width	buffer of width
 * 			height	buffer of height
 * 			bpp		bits per pixel
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
int
_get_point (int x, int y, char *dbuf, int w, int h, int bpp)
{
	int pos = 0;
	int s_bit = 0;
	int idx = 0;
	unsigned char mask = 0;

	if(x >= w)	x = w - 1;
	if(y >= h)	y = h - 1;
	if(x < 0)	x = 0;
	if(y < 0)	y = 0;

	pos = PIEXL2BUF_POS(x, y, w, bpp);
	s_bit = PIEXL_SHIFT_BIT(x, y, w, bpp);

	mask = (bpp == 2? 0x03 : 0x01);
	mask <<= s_bit;
	idx = dbuf[pos] & mask;
	idx >>= s_bit;

	return idx;
}
/**
 * @func    _canvas_get_point
 * @brief   get point from canvas
 * @param   x		x position
 * 			y		y position
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
int
_canvas_get_point (int x, int y)
{
	return _get_point(x, y, tw_cvs.dbuf, tw_cvs.width, tw_cvs.height, tw_cvs.bpp);
}
/**
 * @func    _set_point
 * @brief   set point at canvas
 * @param   x		x position
 * 			y		y position
 * 			idx		index (color)
 * 			dbuf	data buffer of point
 * 			width	buffer of width
 * 			height	buffer of height
 * 			bpp		bits per pixel
 * 			chkey	chrome key
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
void
_set_point (int x, int y, int idx, char *dbuf, int w, int h, int bpp, int chkey)
{
	int pos = 0;
	int s_bit = 0;
	unsigned char mask = 0;

	if(idx == chkey)
		return;

	if(x >= w)	x = w - 1;
	if(y >= h)	y = h - 1;
	if(x < 0)	x = 0;
	if(y < 0)	y = 0;

	pos = PIEXL2BUF_POS(x, y, w, bpp);
	s_bit = PIEXL_SHIFT_BIT(x, y, w, bpp);

	mask = (bpp == 2? 0x03 : 0x01);
	mask <<= s_bit;
	dbuf[pos] &= (~mask);

	idx &= (bpp == 2? 0x03 : 0x01);
	dbuf[pos] |= (idx << s_bit);
}
/**
 * @func    _canvas_set_point
 * @brief   set point at canvas
 * @param   x		x position
 * 			y		y position
 * 			idx		index (color)
 * @return  none
 * @note	not check buffer point, so maybe code dump if NULL point
 */
void
_canvas_set_point (int x, int y, int idx)
{
	_set_point(x, y, idx, tw_cvs.dbuf, tw_cvs.width, tw_cvs.height, tw_cvs.bpp, tw_cvs.chkey);
}
/**
 * @func    tw_canvas_set_point
 * @brief   set point at canvas
 * @param   x		x position
 * 			y		y position
 * 			idx		index (color)
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_set_point (int x, int y, int idx)
{
	int rc = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}
	_canvas_set_point(x, y, idx);

	return rc;
}
/**
 * @func    _canvas_line
 * @brief   draw a line at canvas
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_line (int x0, int y0, int x1, int y1, int idx)
{
    int x, y;
    int dx;
    int dy;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}
    if (x0 > 0x7FFF) x0 = 0;
    if (y0 > 0x7FFF) y0 = 0;
    if (x1 > 0x7FFF) x1 = 0;
    if (y1 > 0x7FFF) y1 = 0;
	if(x0 >= tw_cvs.width)	x0 = tw_cvs.width - 1;
	if(y0 >= tw_cvs.height)	y0 = tw_cvs.height - 1;
	if(x1 >= tw_cvs.width)	x1 = tw_cvs.width - 1;
	if(y1 >= tw_cvs.height)	y1 = tw_cvs.height - 1;

    if (y0 == y1) {
        if (x0 <= x1) x = x0;
        else {
            x = x1;
            x1 = x0;
        }
        while (x <= x1) {
        	_canvas_set_point (x, y0, idx);
            x++;
        }
        return 0;
    }
    else if (y0 > y1)
        dy = y0 - y1;
    else
        dy = y1 - y0;
    if (x0 == x1) {
        if (y0 <= y1) y = y0;
        else {
            y = y1;
            y1 = y0;
        }
        while (y <= y1) {
        	_canvas_set_point (x0, y, idx);
            y++;
        }
        return 0;
    } else if (x0 > x1) {
        dx = x0 - x1;
        x = x1;
        x1 = x0;
        y = y1;
        y1 = y0;
    } else {
        dx = x1 - x0;
        x = x0;
        y = y0;
    }

    if (dx == dy) {
        while (x <= x1) {
            x++;
            if (y > y1) y--;
            else y++;
            _canvas_set_point (x, y, idx);
        }
    } else {
    	_canvas_set_point (x, y, idx);
		if (y < y1) {
			if (dx > dy) {
				int p = dy * 2 - dx;
				int twoDy = 2 * dy;
				int twoDyMinusDx = 2 * (dy - dx);
				while (x < x1) {
					x++;
					if (p < 0) p += twoDy;
					else {
						y++;
						p += twoDyMinusDx;
					}
					_canvas_set_point (x, y, idx);
				}
			} else {
				int p = dx * 2 - dy;
				int twoDx = 2 * dx;
				int twoDxMinusDy = 2 * (dx - dy);
				while (y < y1) {
					y++;
					if (p < 0) p += twoDx;
					else {
						x++;
						p += twoDxMinusDy;
					}
					_canvas_set_point (x, y, idx);
				}
			}
		} else {
			if (dx > dy) {
				int p = dy * 2 - dx;
				int twoDy = 2 * dy;
				int twoDyMinusDx = 2 * (dy - dx);
				while (x < x1) {
					x++;
					if (p < 0) p += twoDy;
					else {
						y--;
						p += twoDyMinusDx;
					}
					_canvas_set_point (x, y, idx);
				}
			} else {
				int p = dx * 2 - dy;
				int twoDx = 2 * dx;
				int twoDxMinusDy = 2 * (dx - dy);
				while (y1 < y) {
					y--;
					if (p < 0) p += twoDx;
					else {
						x++;
						p += twoDxMinusDy;
					}
					_canvas_set_point (x, y, idx);
				}
			}
		}
    }

    return 0;
}
/**
 * @func    tw_canvas_line
 * @brief   draw a line at canvas and update
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_line (int x0, int y0, int x1, int y1, int idx)
{
	int rc = 0;

	rc = _canvas_line(x0, y0, x1, y1, idx);
	if(rc < 0) {
		ERROR("heap allocation is fail for canvas\n");
		return rc;
	}
	tw_canvas_update();

	return rc;
}
/**
 * @func    _canvas_circle
 * @brief   draw a circle at canvas
 * @param   c_x		x0 position
 * 			c_y		y0 position
 * 			r		radius
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_circle (int c_x, int c_y, int r, int idx, int fill)
{
    int x, y;
    int delta, tmp;
    x = 0;
    y = r;
    delta = 3 - (r << 1);

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas.\n");
		return -1;
	}
	if(c_x < 0 || c_y < 0 || r <= 0) {
        ERROR("Invalid parameters.\n");
        return -1;
	}

    while (y >= x) {
        if (fill) {
        	_canvas_line (c_x + x, c_y + y, c_x - x, c_y + y, idx);
        	_canvas_line (c_x + x, c_y - y, c_x - x, c_y - y, idx);
        	_canvas_line (c_x + y, c_y + x, c_x - y, c_y + x, idx);
        	_canvas_line (c_x + y, c_y - x, c_x - y, c_y - x, idx);
        } else {
        	_canvas_set_point (c_x + x, c_y + y, idx);
        	_canvas_set_point (c_x - x, c_y + y, idx);
        	_canvas_set_point (c_x + x, c_y - y, idx);
        	_canvas_set_point (c_x - x, c_y - y, idx);
        	_canvas_set_point (c_x + y, c_y + x, idx);
        	_canvas_set_point (c_x - y, c_y + x, idx);
        	_canvas_set_point (c_x + y, c_y - x, idx);
        	_canvas_set_point (c_x - y, c_y - x, idx);
        }
        x++;
        if (delta >= 0) {
            y--;
            tmp = (x << 2);
            tmp -= (y << 2);
            delta += (tmp + 10);
        } else
            delta += ((x << 2) + 6);
    }

    return 0;
}
/**
 * @func    tw_canvas_circle
 * @brief   draw a circle at canvas and update
 * @param   c_x		x0 position
 * 			c_y		y0 position
 * 			r		radius
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_circle (int c_x, int c_y, int r, int idx, int fill)
{
	int rc = 0;

	rc = _canvas_circle(c_x, c_y, r, idx, fill);
	if(rc < 0) {
		ERROR("heap allocation is fail for canvas\n");
		return rc;
	}
	tw_canvas_update();

	return rc;
}
/**
 * @func    _canvas_rectangle
 * @brief   draw a rectangle at canvas
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_rectangle (int x0, int y0, int x1, int y1, int idx, int fill)
{
	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas.\n");
		return -1;
	}
	if (fill) {
        int i;
        if (x0 > x1) {
            i = x1;
            x1 = x0;
        } else
            i = x0;
        for (; i <= x1; i++)
        	_canvas_line (i, y0, i, y1, idx);
        return 0;
    }
	_canvas_line (x0, y0, x0, y1, idx);
	_canvas_line (x0, y1, x1, y1, idx);
	_canvas_line (x1, y1, x1, y0, idx);
	_canvas_line (x1, y0, x0, y0, idx);

    return 0;
}
/**
 * @func    tw_canvas_rectangle
 * @brief   draw a rectangle at canvas and update
 * @param   x0		x0 position
 * 			y0		y0 position
 * 			x1		x1 position
 * 			y1		y1 position
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_rectangle (int x0, int y0, int x1, int y1, int idx, int fill)
{
	int rc = 0;

	rc = _canvas_rectangle(x0, y0, x1, y1, idx, fill);
	if(rc < 0) {
		ERROR("heap allocation is fail for canvas\n");
		return rc;
	}
	tw_canvas_update();

	return rc;
}
/**
 * @func    _canvas_square
 * @brief   draw a square at canvas
 * @param   x		x position
 * 			y		y position
 * 			within	width
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_square (int x, int y, int within, int idx, int fill)
{
    return _canvas_rectangle (x, y, x + within, y + within, idx, fill);
}
/**
 * @func    tw_canvas_square
 * @brief   draw a square at canvas and update
 * @param   x		x position
 * 			y		y position
 * 			within	width
 * 			idx		index (color)
 * 			fill	fill option
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_square (int x, int y, int within, int idx, int fill)
{
    return tw_canvas_rectangle (x, y, x + within, y + within, idx, fill);
}
/**
 * @func    _canvas_bmp
 * @brief   load a bmp at canvas
 * @param   x		x position
 * 			y		y position
 * 			w		BMP of width
 * 			h		BMP of height
 * 			bpp		BMP of bits per pixel*
 * 			src		BMP of source point
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_bmp (int x, int y, int w, int h, int bpp, unsigned char *src)
{
    int rc = 0;
    int i = 0, j = 0, w_num = 0, h_num = 0;
    int idx = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas.\n");
		return -1;
	}
	if(x < 0 || y < 0 || w <= 0 || h <= 0 || bpp <= 0 || src == NULL) {
        ERROR("Invalid parameters.\n");
        return -1;
	}
	w_num = (((x + w) > tw_cvs.width)? (tw_cvs.width - x) : w);
	h_num = (((y + h) > tw_cvs.height)? (tw_cvs.height - y) : h);

	for(j = 0; j < h_num; j++) {
		for(i = 0; i < w_num; i++) {
			idx = x + i + ((y + j) * tw_cvs.width);
			tw_cvs.dbuf[idx] = src[(i + (j * w))];
		}
	}

    return rc;
}
/**
 * @func    tw_canvas_bmp
 * @brief   load a bmp at canvas and update
 * @param   x		x position
 * 			y		y position
 * 			w		BMP of width
 * 			h		BMP of height
 * 			bpp		BMP of bits per pixel
 * 			src		BMP of source point
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_bmp (int x, int y, int w, int h, int bpp, unsigned char *src)
{
	int rc = 0;

	rc = _canvas_bmp (x, y, w, h, bpp, src);
	if(rc < 0) {
		ERROR("heap allocation is fail for canvas\n");
		return rc;
	}
	tw_canvas_update();

	return rc;
}

/**
 * @func    tw_canvas_update
 * @brief   canvas update
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_update (void)
{
	int rc = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}

	if(tw_cvs.font_w == 16) {
		_canvas_16w_update();
	} else if(tw_cvs.font_w == 12) {
		_canvas_12w_update();
	} else {
		ERROR("OSD2 Font width(%d) not is 12 or 16!\n", tw_cvs.font_w);
	}

	return rc;
}

/**
 * @func    _canvas_clear
 * @brief   canvas clear
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_clear (void)
{
	int rc = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}
	memset(tw_cvs.dbuf, 0, tw_cvs.size);

	return rc;
}
/**
 * @func    tw_canvas_clear
 * @brief   canvas clear and update
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_clear (void)
{
	int rc = 0;

	rc = _canvas_clear();
	if(rc < 0) {
		ERROR("heap allocation is fail for canvas\n");
		return rc;
	}
	tw_canvas_update();

	return rc;
}

/**
 * @func    _canvas_fill
 * @brief   canvas fill
 * @param   idx		fill index
 * @return  0 for successful, minus if any error occurred
 */
int
_canvas_fill (int idx)
{
	int rc = 0;
	int i = 0;
	unsigned char dat = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}

	idx &= (tw_cvs.bpp == 2? 0x03 : 0x01);

	for(i = 0; i < 8; i += tw_cvs.bpp) {
		dat |= (idx << i);
	}
	memset(tw_cvs.dbuf, dat, tw_cvs.size);

	return rc;
}
/**
 * @func    tw_canvas_fill
 * @brief   canvas fill and update
 * @param   idx		fill index (color)
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_fill (int idx)
{
	int rc = 0;

	rc = _canvas_fill(idx);
	if(rc < 0) {
		ERROR("heap allocation is fail for canvas\n");
		return rc;
	}
	tw_canvas_update();

	return rc;
}

/**
 * @func    tw_canvas_config
 * @brief   configure a canvas (system) base on a canvas (app)
 * @param   canvas		canvas structure
 * @return  none
 */
void
tw_canvas_config (struct tw_canvas *canvas)
{
	tw_cvs = *canvas;

#ifdef TW_WG_DEBUG_MODE
	printf("font_w %d\n", tw_cvs.font_w);
	printf("font_h %d\n", tw_cvs.font_h);
	printf("bpp %d\n", tw_cvs.bpp);
	printf("fg_color %d\n", tw_cvs.fg_color);
	printf("bg_color %d\n", tw_cvs.bg_color);
	printf("chkey %d\n", tw_cvs.chkey);
	printf("x %d\n", tw_cvs.x);
	printf("y %d\n", tw_cvs.y);
	printf("index %d\n", tw_cvs.index);
	printf("addr 0x%04X\n", tw_cvs.addr);
	printf("width %d\n", tw_cvs.width);
	printf("height %d\n", tw_cvs.height);
	printf("size %d\n", tw_cvs.size);
	printf("dbuf %p\n", tw_cvs.dbuf);
#endif
}

/**
 * @func    tw_canvas_init
 * @brief   initialize canvas for OSD2 (memory allocation)
 * @param   canvas		canvas structure of point
 * @return  0 for successful, minus if any error occurred
 * @note	must release when unused
 */
int
tw_canvas_init (struct tw_canvas *canvas)
{
	int rc = 0;

	canvas->dbuf = heap_alloc(canvas->size);
	if(canvas->dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		rc = -1;
	}

	tw_canvas_config(canvas);
	_canvas_clear();

	return rc;
}

/**
 * @func    tw_canvas_release
 * @brief   release canvas (release sequence memory)
 * @param   canvas		canvas structure of point
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_release (struct tw_canvas *canvas)
{
	int rc = 0;

	if(tw_cvs.dbuf != canvas->dbuf) {
		INFO("canvas of dbuf is not syn.");
		rc = heap_release(tw_cvs.dbuf);
//		if(rc < 0) {
//			ERROR("heap release is fail for canvas\n");
//		}
	}
	rc = heap_release(canvas->dbuf);
//	if(rc < 0) {
//		ERROR("heap release is fail for canvas\n");
//	}

	canvas->dbuf = NULL;
	memset(&tw_cvs, 0, sizeof(tw_cvs));

//	return rc;
	return 0;
}

/**
 * @func    tw_canvas_enable
 * @brief   enable (display) a canvas on OSD2 MENU
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_enable (void)
{
	int rc = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}

	_osd2_cq_burst(tw_cvs.y, tw_cvs.x, (tw_cvs.width/tw_cvs.font_w), (tw_cvs.height/tw_cvs.font_h),
		((tw_cvs.fg_color) << 1) + ((tw_cvs.bg_color) << 6), tw_cvs.index, 1);
	osd2_update();

	return rc;
}

/**
 * @func    tw_canvas_disable
 * @brief   disable a canvas on OSD2 MENU
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_disable (void)
{
	int rc = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}

	// clear MENU
	_osd2_cq_burst(tw_cvs.y, tw_cvs.x, (tw_cvs.width/tw_cvs.font_w),
		(tw_cvs.height/tw_cvs.font_h), 0, 0, 0);
	osd2_update();

	return rc;
}

/**
 * @func    tw_canvas_loc
 * @brief   setting canvas of location on OSD2 MENU
 * @param   x	x position
 * 			y	y position
 * @return  0 for successful, minus if any error occurred
 */
int
tw_canvas_loc (int x, int y)
{
	int rc = 0;

	if(tw_cvs.dbuf == NULL) {
		ERROR("heap allocation is fail for canvas\n");
		return -1;
	}

	if(x < 0)	x = 0;
	if(y < 0)	y = 0;

	tw_canvas_disable();
	tw_cvs.x = x;
	tw_cvs.y = y;
	tw_canvas_enable();

	return rc;
}

/**
 * @func    tw_canvas_dbuf
 * @brief   setting canvas of data buffer
 * @param   dbuf	data buffer of point
 * @return  none
 */
void
tw_canvas_dbuf (void *dbuf)
{
	tw_cvs.dbuf = (unsigned char *)dbuf;
}

/*
 * tw widget system of global variables
 */
//void* tw_sys_ctx = NULL;				// store system context of point
//void* tw_au_ctx = NULL;					// store audio context of point
static tw_task_status_t tw_task_sta = {	// task of status
	0, 0, 0
};
//time_t tw_detect_tm = 0;
//int tw_detect_tm_sec = 0;
//int tw_detect_tm_cnt = -1;
unsigned char CUR_SRV_ID = 0xFF;		// store current of service ID

/**
 * @func    get_cur_page_id
 * @brief   get current of page ID.
 * @param   none
 * @return  page ID if has active of page
 */
int get_cur_page_id (void)
{
	return CUR_PAGE_ID;
}

/**
 * @func    put_enter_page_event
 * @brief   put event for enter page with specified page ID.
 * @param   unsigned char id	page ID
 * @return  none
 */
void put_enter_page_event (unsigned char id)
{
	TW_EVENT events;
	events.type = TW_EVENT_TYPE_ENTER_SYSTEM;
	events.page.id = id;

	if(tw_task_sta.status == TW_TASK_STA_WIDGET)
		put_event(&events);
#if 0	// maybe someday need this
	else if(tw_task_sta.status == TW_TASK_STA_SERVICE)
		srv_put_event(&events);
#endif
	else
		ERROR("unknow error!\n");

	tw_task_sta.status = TW_TASK_STA_WIDGET;
	tw_task_sta.page_id = id;
}

/**
 * @func    put_quit_page_event
 * @brief   put event for quit page with specified page ID.
 * @param   unsigned char id	page ID
 * @return  none
 */
void put_quit_page_event (unsigned char id)
{
	TW_EVENT events;
	events.type = TW_EVENT_TYPE_QUIT_SYSTEM;
	events.page.id = id;
	put_event(&events);
}

/**
 * @func    put_enter_slide_page_event
 * @brief   put event for enter page by slide.
 * @param   unsigned char id	page ID
 * @return  none
 */
void put_enter_slide_page_event (unsigned char id)
{
    TW_EVENT events;
    events.type = TW_EVENT_TYPE_ENTER_SYSTEM;
    events.page.id = id;
    events.page.mode = PAGE_EVENT_SLIDE;
    put_event(&events);
}

/**
 * @func    put_quit_slide_page_event
 * @brief   put event for quit page by slide.
 * @param   unsigned char id	page ID
 * @return  none
 */
void put_quit_slide_page_event (unsigned char id)
{
    TW_EVENT events;
    events.type = TW_EVENT_TYPE_QUIT_SYSTEM;
    events.page.id = id;
    events.page.mode = PAGE_EVENT_SLIDE;
    put_event(&events);
}

/**
 * @func    put_enter_srv_event
 * @brief   put event for enter service with specified service ID.
 * @param   unsigned char id	page ID
 * @return  none
 */
void put_enter_srv_event (unsigned char id)
{
	TW_EVENT events;
	events.type = TW_EVENT_TYPE_ENTER_SYSTEM;
	events.page.id = id;

	if(tw_task_sta.status == TW_TASK_STA_WIDGET)
		put_event(&events);
#if 0	// maybe someday need this
	else if(tw_task_sta.status == TW_TASK_STA_SERVICE)
		srv_put_event(&events);
#endif
	else
		ERROR("unknow error!\n");

	tw_task_sta.status = TW_TASK_STA_SERVICE;
	tw_task_sta.srv_id = id;
}
  
static unsigned int TW_SYS_TIMEOUT = 60000;
#define TW_AFTER_SEC		3
static unsigned long long tm_out = 0;
static int tw_tm_fg = 0;

struct tw_timer user_time[TW_TIMER_TOTAL_NUM];

#define is_time_outs(val)    ((long long)(read_cpu_count() - val) > 0)

void tw_sys_timer_clear (void)
{
	tw_tm_fg = 1;
	tm_out = read_cpu_count() + msec2hz(TW_SYS_TIMEOUT);
}

void tw_sys_timer_start (void)
{
	tw_tm_fg = 1;
	tw_sys_timer_clear();
	INFO("time count start for enter sleep mode....\n");
}

void tw_sys_timer_stop (void)
{
	tw_tm_fg = 0;
}

/**
 * @func    tw_set_timer
 * @brief   set the msec and count of individual timer
 * @param   id		ID of timer
 * 			msec	millisecond
 * @return  0 for successful, minus if any error occurred
 */
int tw_set_timer (int id, int msec)
{
	if(id >= TW_TIMER_TOTAL_NUM) {
		ERROR("set timer id is %d, but the %d maximum number of timer.\n",
			id, TW_TIMER_TOTAL_NUM);
		return -1;
	} else if (id < 0) {
		int sub_id = 0;
		sub_id = -id;
		if(sub_id >= TW_TIMER_TOTAL_NUM || sub_id < 0) {
			ERROR("set timer id is %d, but the %d maximum number of timer.\n",
				sub_id, TW_TIMER_TOTAL_NUM);
			return -1;
		}
		id = sub_id;
		user_time[id].auto_set = 1;
	} else {
		user_time[id].auto_set = 0;
	}

	user_time[id].en = 1;
	user_time[id].count = read_cpu_count() + msec2hz(msec);
	user_time[id].msec = msec;

	return 0;
}

/**
 * @func    tw_clear_timer
 * @brief   clear the state of individual timer
 * @param   id		ID of timer
 * @return  0 for successful, minus if any error occurred
 */
int tw_clear_timer (int id)
{
	if(id > TW_TIMER_TOTAL_NUM || id < 0) {
		ERROR("set timer id is %d, but the %d maximum number of timer.\n",
			id, TW_TIMER_TOTAL_NUM);
		return -1;
	}

	user_time[id].en = 0;
	user_time[id].count = 0;
	user_time[id].msec = 0;
	user_time[id].auto_set = 0;

	return 0;
}

/**
 * @func    tw_check_timer
 * @brief   check the count of individual timer
 * @param   id		ID of timer
 * @return  1 if timeout, 0 if noting
 */
int tw_check_timer (int id)
{
	if(user_time[id].en > 0) {
		if(is_time_outs(user_time[id].count)) {
			if(user_time[id].auto_set) {
				tw_set_timer(id, user_time[id].msec);
				user_time[id].auto_set = 1;
			} else {
				tw_clear_timer(id);
			}
			return 1;
		}
	}
	return 0;
}

static int chk_ringbell = 0;
static int pre_bell_sta = 0;
void tw_set_chk_rbell (int en)
{
	if(en < 0 || en > 1)
		en = 0;

	chk_ringbell = en;
	pre_bell_sta = 0;
}

static tw_chk_gpio_event_t chk_gpio_event = NULL;
void
tw_set_gpio_chk (tw_chk_gpio_event_t chk_func)
{
	chk_gpio_event = chk_func;
}

static tw_chk_gpio_event_t chk_uart_comm_event = NULL;
void
tw_set_uart_comm_chk (tw_chk_gpio_event_t chk_func)
{
	chk_uart_comm_event = chk_func;
}

static unsigned int watch_time = 5000;	// default 5000ms

void
tw_set_watch_time (unsigned int wt)
{
	watch_time = wt;
}

unsigned int
tw_get_watch_time (void)
{
	return watch_time;
}

void
tw_set_shutdown_time (unsigned int st)
{
	TW_SYS_TIMEOUT = st;
}

unsigned int
tw_get_shutdown_time (void)
{
	return TW_SYS_TIMEOUT;
}

/**
 * @func    tw_check_event
 * @brief   check all event and put to event queue, call this function in main loop,
 * 			and anybody can get event. add check function in here
 * 			if you want receiver of event (definition in TW_EVENT).
 * @param   none
 * @return  none
 */



extern unsigned char mSarkey1Count;
extern unsigned char mSarkey2Coun;
extern unsigned char mSarkey1_val;
extern unsigned char mSarkey2_val;
extern unsigned char mkey1stroke_ch;
extern unsigned char mkey2stroke_ch;

void
tw_check_event (void)
{
    TW_EVENT s_event;
    touchd  tv = {0, 0, 0};
    int read_value = 0x00;
    int tw_evt_fg = 0;
    int i = 0;
	
    /* check Touch event from Touch of queue */
#if 0
    tv = touch_read();
	if((tv.x!=0)||(tv.y!=0)){		
		switch(tv.status) {
			/* get data from Touch of queue and
			 * put event queue for anybody can get
			 */
		case TOUCH_PEN_DOWN:
			s_event.type = TW_EVENT_TYPE_TOUCH_DOWN;
			s_event.touch_panel.x = (int)tv.x;
			s_event.touch_panel.y = (int)tv.y;
			put_event(&s_event);
			tw_evt_fg = 1;
			break;
		case TOUCH_PEN_MOVE:
			s_event.type = TW_EVENT_TYPE_TOUCH_MOTION;
			s_event.touch_panel.x = (int)tv.x;
			s_event.touch_panel.y = (int)tv.y;
			put_event(&s_event);
			tw_evt_fg = 1;
			break;
		case TOUCH_PEN_REL:
			s_event.type = TW_EVENT_TYPE_TOUCH_RELEASE;
			s_event.touch_panel.x = (int)tv.x;
			s_event.touch_panel.y = (int)tv.y;
			put_event(&s_event);
			tw_evt_fg = 1;
			break;
		}
	}else{
		tv=check_repeat(td_rep_thres);
		if((tv.x!=0)||(tv.y!=0)){
			s_event.type = TW_EVENT_TYPE_TOUCH_REPEAT;
			s_event.touch_panel.x = (int)tv.x;
			s_event.touch_panel.y = (int)tv.y;
			put_event(&s_event);
			tw_evt_fg = 1;
		}
	} 
#endif
    /* check SAR event from SAR of queue */
    read_value = sar1_read();	
    if(read_value!=0)	{
		/* get data from SAR of queue and
		 * put event queue for anybody can get
		 */	
		s_event.type = TW_EVENT_TYPE_KEY_DOWN;
		s_event.keystroke.ch = read_value; 
		s_event.keystroke.no = 1;	// [7] 1: SAR1, 2: SAR2      
		put_event(&s_event);
		tw_evt_fg = 1;		
		mSarkey1_val =0x80;
		mkey1stroke_ch =read_value;	
		printf("get key = %d\r\n", mkey1stroke_ch); //bob
    }

	read_value = sar2_read(); 
    if(read_value!=0) {
    	/* get data from SAR of queue and
		 * put event queue for anybody can get
		 */
    	s_event.type = TW_EVENT_TYPE_KEY_DOWN;
		s_event.keystroke.ch = read_value;
		s_event.keystroke.no = 2;	// [7] 1: SAR1, 2: SAR2
		put_event(&s_event);
		tw_evt_fg = 1;			
		mSarkey2_val =0x80;	
		mkey2stroke_ch =read_value;
	}

	if(mSarkey1_val >>7 ==1 || mSarkey2_val >>7 ==1)	{		
		unsigned char sta =sar_status();
		if(sta ==0x01)		    s_event.type = TW_EVENT_TYPE_KEY_UP;		
		else if(sta ==0x02)		s_event.type = TW_EVENT_TYPE_KEY_REPEAT;
		
		if(sta ==0x01 || sta ==0x02) {
			if(mSarkey1_val >>7 ==1){
				s_event.keystroke.ch =mkey1stroke_ch; 
				s_event.keystroke.no = 1;
			}
			if(mSarkey2_val >>7 ==1) {
				s_event.keystroke.ch =mkey2stroke_ch; 
				s_event.keystroke.no = 2;
			}
			put_event(&s_event);  
		}
	}
#if 0
    /* check IR event from IR of queue */
    read_value = IRread();
    if(read_value!=0) {
    	/* get data from IR of queue and
		 * put event queue for anybody can get
		 */
    	s_event.type = TW_EVENT_TYPE_IR_DOWN;
		s_event.ircode.ch = read_value;
		put_event(&s_event);
		tw_evt_fg = 1;
    }

    /* touch for keep alive */
    watchdog_touch();
 #endif
 #if 0
    /* check CVBS state */   
    if(cvbs_detect_change()) {
    	s_event.type = TW_EVENT_TYPE_SIGNAL;
		s_event.signal.std = cvbs_st.std_mode;
		put_event(&s_event);
		tw_evt_fg = 1;
		dbg(2,"0000\n");
    }

    for(i = 0; i < TW_TIMER_TOTAL_NUM; i++) {
    	if(tw_check_timer(i)) {
        	s_event.type = TW_EVENT_TYPE_TIMEOUT;
    		s_event.timer.id = i;
    		put_event(&s_event);
    	}
    }

    /* check GPIO of event */
    if(chk_gpio_event)
    	if(chk_gpio_event())
    		tw_evt_fg = 1;

	/* check uart communication data*/
	if(chk_uart_comm_event)
    	chk_uart_comm_event();
//		if(chk_uart_comm_event())
//			tw_evt_fg = 1;

    /* check rang bell of event */
    if(chk_ringbell) {
    	int cur_bell_sta = ringbell_status();
    	if(pre_bell_sta != cur_bell_sta) {
			switch (ringbell_status()) {
			case ringbell_idle:
				s_event.type = TW_EVENT_TYPE_BELL;
				s_event.rbell.status = ringbell_idle;
				put_event(&s_event);
				tw_evt_fg = 1;
				break;
			case ringbell_play:
				s_event.type = TW_EVENT_TYPE_BELL;
				s_event.rbell.status = ringbell_play;
				put_event(&s_event);
				tw_evt_fg = 1;
				break;
			case ringbell_stop:
				s_event.type = TW_EVENT_TYPE_BELL;
				s_event.rbell.status = ringbell_stop;
				put_event(&s_event);
				tw_evt_fg = 1;
				break;
			}
    	}
    	pre_bell_sta = cur_bell_sta;
    }

    /* enter sleep mode or shutdown if timeout */
    if(tw_tm_fg) {
    	if(tw_evt_fg) {
    		tw_sys_timer_clear();
    	} else if(is_time_outs(tm_out)) {
			INFO("Within a 30 seconds no any event....\n");
			INFO("So after %d second(s) system will be shutdown\n", TW_AFTER_SEC);
			sleep(TW_AFTER_SEC);
			sys_shutdown();
		}
    }
#endif
}

TW_EVENT*
tw_get_sys_evt (void)
{
	tw_check_event();
	return get_event();
}

#if 0	// maybe someday need this
/**
 * @func    tw_srv_process
 * @brief   service of process function.
 * @param   srv_handler			App of service handler
 * 			TW_EVENT* event		sys event of point
 * @return  0 if noting, or 1 if some thing
 */
unsigned char
tw_srv_process (tw_srv_handler_t srv_handler, TW_EVENT* event)
{
    if(event->type == TW_EVENT_TYPE_ENTER_SYSTEM) {
        CUR_PAGE_ID = event->page.id;
        if((CUR_PAGE_ID == 0xFF)||(CUR_PAGE_ID == 0)) {
        	/* page ID is unknow */
        	CUR_PAGE_ID = 0;	// clear current page ID
            dbg(0, "no active page!\n");

            return TW_RETURN_NO_ACTIVE;
        }

        return TW_EVENT_TYPE_ENTER_SYSTEM;

    } else if(event->type == TW_EVENT_TYPE_SERVICE_CREATE) {
        /* request service */
    	CUR_SRV_ID = event->service.srv_id;
        if((CUR_SRV_ID == 0xFF)||(CUR_SRV_ID == 0)) {
        	/* service ID is unknow */
        	CUR_SRV_ID = 0;	// clear current service ID
        	dbg(0, "no active service!\n");

        	return TW_ERROR_SRV_CREATE_FAILE;
        }

    } else if(event->type == TW_EVENT_TYPE_QUIT_TASK) {
        /* quit tw widget system */
        return TW_EVENT_TYPE_QUIT_TASK;

    }

    return srv_handler(CUR_SRV_ID, tw_sys_ctx);
}

/**
 * @func    tw_service_handler
 * @brief   service of handler entry function.
 * @param   srv_handler		App of service handler
 * @return  0 if noting, or 1 if some error
 */
int
tw_service_handler (tw_srv_handler_t srv_handler)
{
    TW_EVENT* events;
    int rc= 0, w_quit= 0;

    if(!srv_handler) {
        ERROR("requset service, but no srv_handler function!\n");
        rc = TW_ERROR_NO_SRV_HANDLE;
        goto EXIT;
    }

    while(!w_quit) {
		events = srv_get_event();
        rc = tw_srv_process(srv_handler, events);

        if(rc != TW_RETURN_NONE) {

        	if(rc == TW_EVENT_TYPE_ENTER_SYSTEM)
            	INFO("enter page id: %u\n", CUR_PAGE_ID);
            else if (rc == TW_EVENT_TYPE_QUIT_TASK)
            	INFO("quit task from page id: %u\n", CUR_PAGE_ID);
            else if (rc == TW_RETURN_NO_ACTIVE)
                dbg(0, "no active page (id:%u)\n", CUR_PAGE_ID);
            else
            	dbg(0, "unknow error... \n");

            w_quit= 1;
        }
    }

EXIT:
    return rc;
}

/**
 * @func    tw_service_init
 * @brief   initialize for service of task with specified service ID.
 * @param   unsigned char service_id	service ID
 * @return  none
 */
void
tw_service_init (unsigned char service_id)
{
	TW_EVENT service_event;

	/* put request service of event with specified service ID */
	service_event.type = TW_EVENT_TYPE_SERVICE_CREATE;
	service_event.service.srv_id = service_id;
	put_event(&service_event);

	/* update task of status */
	tw_task_sta.status= TW_TASK_STA_SERVICE;
	tw_task_sta.srv_id= service_id;
}
#endif

/**
 * @func    tw_page_process
 * @brief   widget page of process function.
 * @param   page_handler		App of page handler
 * 			TW_EVENT* event		sys event of point
 * @return  0 if noting, or 1 if some thing
 */
unsigned char
tw_page_process (tw_page_handler_t page_handler, TW_EVENT* event)
{
    if(event->type == TW_EVENT_TYPE_ENTER_SYSTEM) {
        CUR_PAGE_ID = event->page.id;
        if((CUR_PAGE_ID == 0xFF)||(CUR_PAGE_ID == 0)) {
        	/* page ID is unknown */
        	CUR_PAGE_ID = 0;	// clear current page ID
            dbg(0, "no active page!\n");

            return TW_RETURN_NO_ACTIVE;
        }
    }
#if 0	// maybe someday need this
    else if(event->type == TW_EVENT_TYPE_SERVICE_CREATE) {
        /* request service */
    	CUR_SRV_ID = event->service.srv_id;
        if((CUR_SRV_ID == 0xFF)||(CUR_SRV_ID == 0)) {
        	/* service ID is unknow */
        	CUR_SRV_ID = 0;	// clear current service ID
        	dbg(0, "no active service!\n");

        	return TW_ERROR_SRV_CREATE_FAILE;
        }

        return TW_EVENT_TYPE_SERVICE_CREATE;

    } else if(event->type == TW_EVENT_TYPE_SERVICE_DESTROY) {
        /* service destroy, back to page */
    	if((CUR_PAGE_ID == 0xFF)||(CUR_PAGE_ID == 0)) {
    		/* page ID is unknow */
    		//CUR_PAGE_ID = 0;	// clear current page ID
			dbg(0, "service destroy, back to page but no active page!\n");

			return TW_RETURN_NO_ACTIVE;
		}
    }
#endif
    else if(event->type == TW_EVENT_TYPE_QUIT_TASK) {
        /* quit tw widget system */
        return TW_EVENT_TYPE_QUIT_TASK;

    }

    return page_handler(CUR_PAGE_ID, event);
}

/**
 * @func    tw_task_handler
 * @brief   widget page of handler entry function.
 * @param   page_handler	App of page handler
 * @return  0 if noting, or 1 if some error
 */
int
tw_widget_handler (tw_page_handler_t page_handler)
{
    TW_EVENT* events;
    int rc= 0, w_quit= 0;

    if(!page_handler) {
        ERROR("enter tw widget sys, but no tw_page_handler function!\n");
        rc = TW_ERROR_NO_PAGE_HANDLE;
        goto EXIT;
    }

    while(!w_quit) {
    	tw_check_event();
		events = get_event();
        rc = tw_page_process(page_handler, events);

        if (rc == TW_RETURN_NO_PROCESS)
        	rc = TW_RETURN_NO_PROCESS;	// noting
        else if(rc != TW_RETURN_NONE) {
            if(rc == TW_EVENT_TYPE_SERVICE_CREATE)
            	INFO("request service id: %u\n", CUR_SRV_ID);
            else if (rc == TW_EVENT_TYPE_QUIT_TASK)
            	INFO("quit task from page id: %u\n", CUR_PAGE_ID);
            else if (rc == TW_RETURN_NO_ACTIVE)
                dbg(0, "no active page (id:%u)\n", CUR_PAGE_ID);
            else
            	dbg(0, "unknow error... \n");
            w_quit= 1;
            rc = -1;
        }
    }

    //dbg(0, "quit widget system task, return code: %u\n", rc);

EXIT:
    return rc;
}

/**
 * @func    tw_widget_init
 * @brief   initialize for widget page of task with specified page ID.
 * @param   int page_id		page ID
 * @return  none
 */
void
tw_widget_init (int page_id)
{
	TW_EVENT page_event;

	/* put enter page of event with specified page ID */
	page_event.type = TW_EVENT_TYPE_ENTER_SYSTEM;
	page_event.page.id = page_id;
	put_event(&page_event);

	/* update task of status */
	tw_task_sta.status = TW_TASK_STA_WIDGET;
	tw_task_sta.page_id = page_id;

	/* clear user timer */
	memset(&user_time, 0, sizeof(struct tw_timer)*TW_TIMER_TOTAL_NUM);
}

/**
 * @func    tw_task_handler
 * @brief   task of handler entry function, include page handler and service handler.
 * @param   int page_id		page ID
 * 			page_handler	App of page handler
 * @return  0 if noting, or 1 if some error
 */
int
tw_task_handler (int page_id, tw_page_handler_t page_handler)
{
    int rc= 0, t_quit= 0;

    if(page_id < 1)
    {
		ERROR("No widget page!\n\r");// No service!\n\r");
		rc = -1;
		goto EXIT;
	}

    tw_widget_init(page_id);

//    tw_detect_tm = time();
//    tw_detect_tm_cnt = 1;

    while(!t_quit) {
    	/* task of main loop */
		dbg(0,"tw_task_sta.status = %d\r\n", tw_task_sta.status);
        switch(tw_task_sta.status) {
		case TW_TASK_STA_WIDGET:
			/* widget page task */
			rc = tw_widget_handler(page_handler);
			
			if(rc < 0) {
				t_quit= 1;
			}
			break;
#if 0	// maybe someday need this
		case TW_TASK_STA_SERVICE:
			/* service task */
			rc = tw_service_handler(service_handler);

			if(rc < 0) {
				t_quit= 1;
			}
			break;
		case TW_TASK_STA_DESTROY:
			/* task destroy */
			DBG_PRINT("tw task destroy, sys will exit main drivers!\n\r");
			t_quit= 1;
			rc = 0; 	// nothing
			break;
#endif
		default:
			ERROR("tw task is unknow status, sys will exit main drivers!\n\r");
			t_quit= 1;
			rc = -1;	// unknow error
			break;
        }

    }

EXIT:
    return rc;
}
