/**
 *  @file   spiosd.h
 *  @brief  head file for SPIOSD control function
 *  $Id: spiosd.h,v 1.1.1.1 2013/12/18 03:43:51 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/11/22  yc	New file.
 *
 */

#ifndef _SPIOSD_H
#define _SPIOSD_H

#define		LOAD_LUT	1
#define		DONT_LOAD	0

/**
 * @func    sosd_hvoffset_init
 * @brief   setting SPIOSD image H/V start of offset.
 * @param   h_offset	horizontal of start offset
 * 			v_offset	vertical of start offset
 * @return  none
 */
extern void
sosd_hvoffset_init (unsigned short h_offset, unsigned short v_offset);
/**
 * @func    sosd_sp_hvoffset_init
 * @brief   setting SPIOSD sprite H/V start of offset.
 * @param   h_offset	horizontal of start offset
 * 			v_offset	vertical of start offset
 * @return  none
 */
extern void
sosd_sp_hvoffset_init (unsigned short h_offset, unsigned short v_offset);
/**
 * @func    _sosd_emu_addr
 * @brief   setting SPIOSD emulation of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
_sosd_emu_addr (unsigned long base_address);
/**
 * @func    _sosd_img_addr
 * @brief   setting SPIOSD image of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
_sosd_img_addr (unsigned long base_address);
/**
 * @func    _sosd_spr_addr
 * @brief   setting SPIOSD sprite of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
_sosd_spr_addr (unsigned long base_address);
/**
 * @func    _sosd_pLUT_addr
 * @brief   setting SPIOSD pLUT of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
_sosd_pLUT_addr (unsigned long base_address);
/**
 * @func    _sosd_sLUT_addr
 * @brief   setting SPIOSD sLUT of address.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
_sosd_sLUT_addr (unsigned long base_address);
/**
 * @func    _sosd_hspos
 * @brief   setting SPIOSD image of h start position.
 * @param   h_pos	horizontal of position
 * @return  none
 */
extern void
_sosd_hspos (unsigned short h_pos);
/**
 * @func    sosd_vspos
 * @brief   setting SPIOSD image of v start position.
 * @param   v_pos	vertical of position
 * @return  none
 */
extern void
_sosd_vspos (unsigned short v_pos);
/**
 * @func    _sosd_spr_hspos
 * @brief   setting SPIOSD sprite of h start position.
 * @param   h_pos	horizontal of position
 * @return  none
 */
extern void
_sosd_sp_hspos (unsigned short h_pos);
/**
 * @func    _sosd_spr_vspos
 * @brief   setting SPIOSD sprite of v start position.
 * @param   v_pos	vertical of position
 * @return  none
 */
extern void
_sosd_sp_vspos (unsigned short v_pos);
/**
 * @func    _sosd_img_loca
 * @brief   setting SPIOSD image of location.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
extern void
_sosd_img_loca (unsigned short x, unsigned short y);
/**
 * @func    _sosd_spr_loca
 * @brief   setting SPIOSD sprite of location.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
extern void
_sosd_spr_loca (unsigned short x, unsigned short y);
/**
 * @func    _sosd_td_loca
 * @brief   setting SPIOSD TWBC of location.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
extern void
_sosd_td_loca (unsigned short x, unsigned short y);
/**
 * @func    _sosd_img_window
 * @brief   setting SPIOSD image of window size.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
extern void
_sosd_img_window (unsigned short width, unsigned short height);
/**
 * @func    _sosd_spr_window
 * @brief   setting SPIOSD sprite of window size.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
extern void
_sosd_spr_window (unsigned char width, unsigned short height);
/**
 * @func    _sosd_td_window
 * @brief   setting SPIOSD TWBC of window size.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
extern void
_sosd_td_window (unsigned short width, unsigned short height);
/**
 * @func    _sosd_img_ls
 * @brief   setting SPIOSD image of line store.
 * @param   line_store	store size
 * @return  none
 */
extern void
_sosd_img_ls (unsigned short line_store);
/**
 * @func    _sosd_spr_ls
 * @brief   setting SPIOSD sprite of line store.
 * @param   line_store	store size
 * @return  none
 */
extern void
_sosd_spr_ls (unsigned short line_store);
/**
 * @func    _sosd_td_ls
 * @brief   setting SPIOSD TWBC of line store.
 * @param   line_store	store size
 * @return  none
 */
extern void
_sosd_td_ls (unsigned short line_store);
/**
 * @func    sosd_set_emu_base_addrss
 * @brief   setting sOSD emulation of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
sosd_set_emu_base_addrss (unsigned long base_address);
/**
 * @func    sosd_set_img_base_address
 * @brief   setting sOSD image of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
sosd_set_img_base_address (unsigned long base_address);
/**
 * @func    sosd_set_spr_base_address
 * @brief   setting sOSD sprite of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
sosd_set_spr_base_address (unsigned long base_address);
/**
 * @func    sosd_set_pLUT_base_address
 * @brief   setting sOSD pLUT of address and sOSD update.
 * @param   unsigned long base_address	NOR Flash of address
 * @return  none
 */
extern void
sosd_set_pLUT_base_address (unsigned long base_address);
/**
 * @func    sosd_set_sLUT_base_address
 * @brief   setting sOSD SLUT of address and sOSD update.
 * @param   base_address	NOR Flash of address
 * @return  none
 */
extern void
sosd_set_sLUT_base_address (unsigned long base_address);
/**
 * @func    sosd_set_hspos
 * @brief   setting sOSD image of h position and sOSD update.
 * @param   h_pos	horizontal of start position
 * @return  none
 */
extern void
sosd_set_hspos (unsigned short h_pos);
/**
 * @func    sosd_set_vspos
 * @brief   setting sOSD image of v position and sOSD update.
 * @param   v_pos	vertical of start position
 * @return  none
 */
extern void
sosd_set_vspos (unsigned short v_pos);
/**
 * @func    sosd_set_spr_hspos
 * @brief   setting sOSD sprite of h position and sOSD update.
 * @param   h_pos	horizontal of start position
 * @return  none
 */
extern void
sosd_set_sp_hspos (unsigned short h_pos);
/**
 * @func    sosd_set_spr_vspos
 * @brief   setting sOSD sprite of v position and sOSD update.
 * @param   v_pos	vertical of start position
 * @return  none
 */
extern void
sosd_set_sp_vspos (unsigned short v_pos);
/**
 * @func    sosd_set_img_location
 * @brief   setting sOSD image of location and sOSD update.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
extern void
sosd_set_img_location (unsigned short x, unsigned short y);
/**
 * @func    sosd_set_spr_location
 * @brief   setting sOSD sprite of location and sOSD update.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
extern void
sosd_set_spr_location (unsigned short x, unsigned short y);
/**
 * @func    sosd_set_td_location
 * @brief   setting sOSD TWBC of location and sOSD update.
 * @param   x	x coord.
 * 			y	y coord.
 * @return  none
 */
extern void
sosd_set_td_location (unsigned short x, unsigned short y);
/**
 * @func    sosd_set_img_active_window
 * @brief   setting sOSD image of window and sOSD update.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
extern void
sosd_set_img_active_window (unsigned short width, unsigned short height);
/**
 * @func    sosd_set_spr_active_window
 * @brief   setting sOSD sprite of window and sOSD update.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
extern void
sosd_set_spr_active_window (unsigned char width, unsigned short height);
/**
 * @func    sosd_set_td_active_window
 * @brief   setting sOSD TWBC of window and sOSD update.
 * @param   width	width size
 * 			height	height size
 * @return  none
 */
extern void
sosd_set_td_active_window (unsigned short width, unsigned short height);
/**
 * @func    sosd_set_img_line_store
 * @brief   setting SPIOSD image of line store and sOSD update.
 * @param   line_store	store size
 * @return  none
 */
extern void
sosd_set_img_line_store (unsigned short line_store);
/**
 * @func    sosd_set_spr_line_store
 * @brief   setting SPIOSD sprite of line store and sOSD update.
 * @param   line_store	store size
 * @return  none
 */
extern void
sosd_set_spr_line_store (unsigned short line_store);
/**
 * @func    sosd_set_td_line_store
 * @brief   setting SPIOSD TWBC of line store and sOSD update.
 * @param   line_store	store size
 * @return  none
 */
extern void
sosd_set_td_line_store (unsigned short line_store);
/**
 * @func    sosd_img_force_enable
 * @brief   set SOSD image of force alpha level and SOSD update
 * @param   level	alpha level: 0 ~ 0x40
 * @return  none
 */
extern void
sosd_img_force_ctrl (unsigned char level);
/**
 * @func    sosd_img_force_enable
 * @brief   enable SOSD image of force alpha and SOSD update
 * @param   none
 * @return  none
 */
extern void
sosd_img_force_enable (void);
/**
 * @func    sosd_img_force_disable
 * @brief   disable SOSD image of force alpha and SOSD update
 * @param   none
 * @return  none
 */
extern void
sosd_img_force_disable (void);
/**
 * @func    _sosd_tdc_addr
 * @brief   set TWBC of Color address
 * @param   hsize	TWBC of Color address
 * @return  none
 */
extern void
_sosd_tdc_addr (unsigned long address);
/**
 * @func    _sosd_tdc_h_size
 * @brief   set TWBC of Color HSize
 * @param   hsize	TWBC of Color HSize
 * @return  none
 */
extern void
_sosd_tdc_h_size (unsigned short hsize);
/**
 * @func    _sosd_td_spilt_mode_en
 * @brief   set enable TWBC of split mode
 * @param   none
 * @return  none
 */
extern void
_sosd_td_spilt_mode_en (void);
/**
 * @func    _sosd_td_spilt_mode_dis
 * @brief   set disable TWBC of split mode
 * @param   none
 * @return  none
 */
extern void
_sosd_td_spilt_mode_dis (void);
/**
 * @func    sosd_td_color_addr
 * @brief   set TWBC of Color address and SOSD update
 * @param   hsize	TWBC of Color address
 * @return  none
 */
extern void
sosd_td_color_addr (unsigned long address);
/**
 * @func    sosd_td_color_h_size
 * @brief   set TWBC of Color HSize and SOSD update
 * @param   hsize	TWBC of Color HSize
 * @return  none
 */
extern void
sosd_td_color_h_size (unsigned short hsize);
/**
 * @func    sosd_td_spi_gap
 * @brief   unknown
 * @param   gap		gap
 * @return  none
 */
extern void
sosd_td_spi_gap (unsigned char gap);
/**
 * @func    sosd_spr_force_ctrl
 * @brief   setting forced sprite alpha-blending value and sOSD update.
 * @param   level	0~16
 * @return  none
 */
extern void
sosd_spr_force_ctrl (unsigned char level);
/**
 * @func    sosd_emu_trig
 * @brief   trig SPIOSD of emulation.
 * @param   none
 * @return  none
 */
extern void
sosd_emu_trig (void);
/**
 * @func    sosd_emu_exit
 * @brief   exit from SPIOSD emulation.
 * @param   none
 * @return  none
 */
extern void
sosd_emu_exit (void);
/**
 * @func    sosd_wait_emu_done
 * @brief   waiting SPIOSD emulation to done.
 * @param   none
 * @return  0 if done, or 1 if timeout
 */
unsigned char
sosd_wait_emu_done (void);
/**
 * @func    _sosd_swtc_offset
 * @brief   setting horizontal or vertical count for image transition.
 * @param   offset	horizontal or vertical count
 * @return  none
 */
extern void
_sosd_swtc_offset (unsigned short offset);
/**
 * @func    _sosd_swtc_setting
 * @brief   setting horizontal or vertical count of registers for image transition.
 * @param   value	registers value
 * @return  none
 */
extern void
_sosd_swtc_setting (unsigned short value);
/**
 * @func    _sosd_swtc_h_mode
 * @brief   setting SPIOSD image transition of horizontal mode.
 * @param   none
 * @return  none
 */
extern void
_sosd_swtc_h_mode (void);
/**
 * @func    _sosd_swtc_v_mode
 * @brief   setting SPIOSD image transition of vertical mode.
 * @param   none
 * @return  none
 */
extern void
_sosd_swtc_v_mode (void);
/**
 * @func    _sosd_swtc_enable
 * @brief   enable SPIOSD image transition mode.
 * @param   none
 * @return  none
 */
extern void
_sosd_swtc_enable (void);
/**
 * @func    _sosd_swtc_disable
 * @brief   disable SPIOSD image transition mode.
 * @param   none
 * @return  none
 */
extern void
_sosd_swtc_disable (void);
/**
 * @func    sosd_sw_tc_offset
 * @brief   setting horizontal or vertical count for image transition
 * 			and sOSD update.
 * @param   offset	horizontal or vertical count
 * @return  none
 */
extern void
sosd_sw_tc_offset (unsigned short offset);
/**
 * @func    sosd_sw_tc_setting
 * @brief   setting horizontal or vertical count of registers for image transition
 *			and sOSD update.
 * @param   value	registers value
 * @return  none
 */
extern void
sosd_sw_tc_setting (unsigned short value);
/**
 * @func    sosd_sw_tc_h_mode
 * @brief   setting SPIOSD image transition of horizontal mode and sOSD update
 * @param   none
 * @return  none
 */
extern void
sosd_sw_tc_h_mode (void);
/**
 * @func    sosd_sw_tc_v_mode
 * @brief   setting SPIOSD image transition of vertical mode and sOSD update
 * @param   none
 * @return  none
 */
extern void
sosd_sw_tc_v_mode (void);
/**
 * @func    sosd_sw_tc_enable
 * @brief   enable SPIOSD image transition mode and sOSD update
 * @param   none
 * @return  none
 */
extern void
sosd_sw_tc_enable (void);
/**
 * @func    sosd_sw_tc_disable
 * @brief   disable SPIOSD image transition mode and sOSD update
 * @param   none
 * @return  none
 */
extern void
sosd_sw_tc_disable (void);
/**
 * @func    _sosd_load_plut
 * @brief   load SPIOSD pLUT of color table.
 * @param   none
 * @return  none
 */
extern void
_sosd_load_plut (void);
/**
 * @func    _sosd_load_slut
 * @brief   load SPIOSD sLUT of color table.
 * @param   none
 * @return  none
 */
extern void
_sosd_load_slut (void);
/**
 * @func    _sosd_load_pslut
 * @brief   load SPIOSD pLUT and sLUT of color table.
 * @param   none
 * @return  none
 */
extern void
_sosd_load_pslut (void);
/**
 * @func    sosd_pLUTsLUT_load
 * @brief   load SPIOSD pLUT and sLUT of color table and sOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_pLUTsLUT_load (void);
/**
 * @func    sosd_td_enable
 * @brief   enable TWBC and SOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_td_enable (void);
/**
 * @func    sosd_td_disable
 * @brief   disable TWBC and SOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_td_disable (void);
/**
 * @func    sosd_td_spilt_mode_disable
 * @brief   enable TWBC of split mode and SOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_td_spilt_mode_enable (void);
/**
 * @func    sosd_td_spilt_mode_disable
 * @brief   disable TWBC of split mode and SOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_td_spilt_mode_disable (void);
/**
 * @func    sosd_spr_force_enable
 * @brief   enable to force overwriting sprite alpha-blending percentage
 * 			and sOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_spr_force_enable (void);
/**
 * @func    sosd_spr_enable
 * @brief   enable SPIOSD sprite and load LUT.
 * @param   load	0: unload LUT, 1: load LUT
 * @return  none
 */
extern void
sosd_spr_enable (unsigned char load);
/**
 * @func    sosd_spr_enable
 * @brief   disable SPIOSD sprite.
 * @param   none
 * @return  none
 */
extern void
sosd_spr_disable (void);
/**
 * @func    sosd_enable
 * @brief   enable SPIOSD and load LUT.
 * @param   unsigned char load	0: unload LUT, 1: load LUT
 * @return  none
 */
extern void
sosd_enable (unsigned char load);
/**
 * @func    sosd_disable
 * @brief   disable SPIOSD.
 * @param   none
 * @return  none
 */
extern void
sosd_disable (void);
/**
 * @func    sosd_update
 * @brief   SPIOSD update.
 * @param   none
 * @return  none
 */
extern void
sosd_update (void);
/**
 * @func    spiosd_quad_mode_enable
 * @brief	enable SPIOSD Quad mode
 * @param   none
 * @return  none
 */
extern void
spiosd_quad_mode_enable (void);
/**
 * @func    spiosd_quad_mode_disable
 * @brief	disable SPIOSD Quad mode
 * @param   none
 * @return  none
 */
extern void
spiosd_quad_mode_disable (void);
/**
 * @func    sosd_dma_init
 * @brief	initial SPIOSD of DMA configure
 * @param   none
 * @return  none
 */
extern void
sosd_dma_init (void);
/**
 * @func    spiosd_init
 * @brief	initial SPIOSD and set h/v of offset
 * @param   h_offset	image of horizontal offset
 * 			v_offset	image of vertical offset
 * 			s_h_offset	sprite of horizontal offset
 * 			s_v_offset	sprite of vertical offset
 * @return  none
 */
extern void
spiosd_init (unsigned short h_offset, unsigned short v_offset,
			 unsigned short s_h_offset, unsigned short s_v_offset);
/**
 * @func    _get_img_idx_addr
 * @brief   getting current SPIOSD image of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
extern void
_get_img_idx_addr (unsigned long *addr);
/**
 * @func    _get_spr_idx_addr
 * @brief   getting current SPIOSD sprite of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
extern void
_get_spr_idx_addr(unsigned long *addr);
/**
 * @func    _get_img_ls
 * @brief   getting current SPIOSD image of line store size.
 * @param   *ls		line store size will store to this point
 * @return  none
 */
extern void
_get_img_ls (unsigned short *ls);
/**
 * @func    _get_spr_ls
 * @brief   getting current SPIOSD sprite of line store size.
 * @param   *ls		line store size will store to this point
 * @return  none
 */
extern void
_get_spr_ls(unsigned short *ls);
/**
 * @func    _get_plut_addr
 * @brief   getting current SPIOSD pLUT of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
extern void
_get_plut_addr(unsigned long *addr);
/**
 * @func    _get_slut_addr
 * @brief   getting current SPIOSD sLUT of base address.
 * @param   *addr	address will store to this point
 * @return  none
 */
extern void
_get_slut_addr(unsigned long *addr);
/**
 * @func    _get_swtc_offset
 * @brief   getting current SPIOSD image transition of offset.
 * @param   *offset		offset will store to this point
 * @return  none
 */
extern void
_get_swtc_offset(unsigned short *offset);
/**
 * @func    _get_img_h_position
 * @brief   getting current SPIOSD image h position of offset.
 * @param   *h_position		offset will store to this point
 * @return  none
 */
extern void
_get_img_h_position(unsigned short *h_position);
/**
 * @func    _get_img_v_position
 * @brief   getting current SPIOSD image v position of offset.
 * @param   *v_position		offset will store to this point
 * @return  none
 */
extern void
_get_img_v_position(unsigned short *v_position);
/**
 * @func    _get_spr_h_position
 * @brief   getting current SPIOSD sprite h position of offset.
 * @param   *h_position		offset will store to this point
 * @return  none
 */
extern void
_get_spr_h_position(unsigned short *h_position);
/**
 * @func    _get_spr_v_position
 * @brief   getting current SPIOSD sprite v position of offset.
 * @param   *v_position		offset will store to this point
 * @return  none
 */
extern void
_get_spr_v_position(unsigned short *v_position);
/**
 * @func    _wr_sosd_hspos
 * @brief   write SPIOSD image of h start position and no offset.
 * @param   h_pos	horizontal of position
 * @return  none
 */
extern void
_wr_sosd_hspos (unsigned short h_pos);
/**
 * @func    _wr_sosd_vspos
 * @brief   write SPIOSD image of v start position and no offset.
 * @param   v_pos	vertical of position
 * @return  none
 */
extern void
_wr_sosd_vspos (unsigned short v_pos);
/**
 * @func    _wr_sosd_spr_hspos
 * @brief   write SPIOSD sprite of h start position and no offset.
 * @param   h_pos	horizontal of position
 * @return  none
 */
extern void
_wr_sosd_spr_hspos (unsigned short h_pos);
/**
 * @func    _wr_sosd_spr_vspos
 * @brief   write SPIOSD sprite of v start position and no offset.
 * @param   v_pos	vertical of position
 * @return  none
 */
extern void
_wr_sosd_spr_vspos (unsigned short v_pos);

#endif	/* _SPIOSD_H */
