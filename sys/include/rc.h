/**
 *  @file   rc.h
 *  @brief  this header file put system interface initial function prototypeing
 *  $Id: rc.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  New file.
 *
 */

#ifndef __RC_H
#define __RC_H

#define NEW_TOOL_VERSION_RC

#define RESOURCE_MAGIC      0x12347733
#define RESOURCE_END_MAGIC  0x33771234
#define TYPE_IMG            1
#define TYPE_SPRITE         2
#define TYPE_EMU            3
#define TYPE_MENU           4
#define TYPE_TILE           5
#define TYPE_LUT            6
#define TYPE_OREG           7
#define TYPE_ICON           8
#define TYPE_TD           	9
#define TYPE_TOFONT         10

#define TYPE_RES_T          80
#define TYPE_RES_ITEM_T     81

#pragma pack(push, 1) 	/* push current alignment to stack and
						   set alignment to 1 byte boundary */

struct res_item_t {
        unsigned short    id;
        unsigned char     type;
		unsigned char     subtype;	/* dummy */
        unsigned long     offset;
} __attribute__((packed));
struct res_t {
    unsigned long     resource_magic;  	/* = 0x12347733 */
    unsigned short    nelements;
    struct res_item_t desc[1];			/* sizeof (desc[0]) == 8 */
} __attribute__((packed));

/*
 * resource structure size with 16 bytes alignment
 */
// res_img_t layout
// sizeof(res_img_t) = 16 byte
// 2 Byte: TW
// 4 byte: LUT base address
// 4 byte: index base address
// 2 byte: line jump
// 2 byte: width
// 2 byte: height
struct res_img_t {
        unsigned short	  head;
		unsigned long	  lut_addr;
		unsigned long	  index_addr;
        unsigned short	  line_jump;
		unsigned short    width;
        unsigned short    height;
} __attribute__((packed));

// res_spr_t layout
// sizeof(res_spr_t) = 16 byte
// 2 Byte: TW
// 4 byte: LUT base address
// 4 byte: index base address
// 2 byte: line jump
// 1 byte: width
// 2 byte: height
// 1 byte: count
struct res_spr_t {
        unsigned short	  head;
		unsigned long	  lut_addr;
		unsigned long	  index_addr;
        unsigned short	  line_jump;
		unsigned char     width;
        unsigned short    height;
		unsigned char     count;
} __attribute__((packed));

// res_emu_t layout
// sizeof(res_emu_t) = 9 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 2 byte: length
// 1 byte: repeat
// 7 byte: dummy
struct res_emu_t {
        unsigned short	  head;
		unsigned long	  base_addr;
		unsigned short    length;
		unsigned char	  repeat;
} __attribute__((packed));

// res_menu_t layout
// sizeof(res_menu_t) = 8 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 2 byte: length
// 8 byte: dummy
struct res_menu_t {
        unsigned short	  head;
		unsigned long	  base_addr;
		unsigned short	  oram_addr;
        unsigned short    length;
		unsigned char	  width;
		unsigned char	  height;
} __attribute__((packed));

// res_tile_t layout
// sizeof(res_tile_t) = 8 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 2 byte: length
// 8 byte: dummy
struct res_tile_t {
        unsigned short	  head;
		unsigned long	  base_addr;
        unsigned short    length;
} __attribute__((packed));

// res_tile_t layout
// sizeof(res_tile_t) = 8 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 2 byte: length
// 8 byte: dummy
struct res_tile_tb {
        unsigned short	  head;
		unsigned long	  base_addr;
		unsigned short	  oram_addr;
        unsigned short    length;
		unsigned char	  font_w;
		unsigned char	  font_h;
} __attribute__((packed));

// res_emu_t layout
// sizeof(res_emu_t) = 11 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: LUT base address
// 2 byte: main LUT length
// 1 byte: second LUT length
// 1 byte: 2BP remap LUT length
// 1 byte: BG remap LUT length

struct res_lut_t {
        unsigned short	  head;
		unsigned long	  lut_addr;
		unsigned short	  main_len;
        unsigned char	  sec_len;
		unsigned char	  re_2bp_len;
		unsigned char	  re_bg_len;
} __attribute__((packed));

// res_oreg_t layout
// sizeof(res_oreg_t) = 8 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 2 byte: length
// 8 byte: dummy
struct res_oreg_t {
        unsigned short	  head;
		unsigned long	  base_addr;
        unsigned char     length;
} __attribute__((packed));

// res_oreg_t layout
// sizeof(res_oreg_t) = 12 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 1 byte: font_w
// 1 byte: font_h
// 1 byte: width
// 1 byte: height
// 1 byte: bp
// 1 byte: count
// 5 byte: dummy
struct res_icon_t {
        unsigned short	  head;
		unsigned long	  base_addr;
        unsigned char     font_w;
        unsigned char     font_h;
		unsigned char	  width;
		unsigned char	  height;
		unsigned char	  bp;
		unsigned char	  count;
} __attribute__((packed));

// res_td_t layout
// sizeof(res_td_t) = 16 byte
// 2 Byte: TW
// 4 byte: TD color base address
// 4 byte: index base address
// 2 byte: line jump
// 2 byte: width
// 2 byte: height
struct res_td_t {
        unsigned short	  head;
		unsigned long	  color_addr;
		unsigned long	  index_addr;
        unsigned short	  line_jump;
		unsigned short    width;
        unsigned short    height;
} __attribute__((packed));

// res_tof_t layout
// sizeof(res_tof_t) = 10 byte (16 byte alignment)
// 2 Byte: TW
// 4 byte: base address
// 2 byte: width
// 2 byte: height
// 6 byte: dummy
struct res_tof_t {
	unsigned short	head;
	unsigned long	base_addr;
	unsigned short	font_width;
	unsigned short	font_height;
	unsigned long   flash_addr;
} __attribute__((packed));

#pragma pack(pop)

#define RES_ITEM_T_SIZE		sizeof(struct res_item_t)
#define RES_T_SIZE			sizeof(struct res_t)

#define RES_IMG_T_SIZE		sizeof(struct res_img_t)
#define RES_SPR_T_SIZE		sizeof(struct res_spr_t)
#define RES_EMU_T_SIZE		sizeof(struct res_emu_t)
#define RES_MENU_T_SIZE		sizeof(struct res_menu_t)
#define RES_TILE_T_SIZE		sizeof(struct res_tile_t)
#define RES_LUT_T_SIZE		sizeof(struct res_lut_t)
#define RES_OREG_T_SIZE		sizeof(struct res_oreg_t)
#define RES_ICON_T_SIZE		sizeof(struct res_icon_t)
#define RES_TD_T_SIZE		sizeof(struct res_td_t)	 
#define RES_TOF_T_SIZE		sizeof(struct res_tof_t)


extern int rc_init(unsigned short *n);
extern unsigned long rc_data(unsigned short id);
extern unsigned short rc_type(unsigned short id);
extern unsigned long rc_len(unsigned short id);

extern int resource_init (void);
extern unsigned char get_res_img (unsigned short id, struct res_img_t *img_s);
extern unsigned char get_res_spr (unsigned short id, struct res_spr_t *spr_s);
extern unsigned char get_res_emu (unsigned short id, struct res_emu_t *emu_s);
extern unsigned char get_res_menu (unsigned short id, struct res_menu_t *menu_s);
extern unsigned char get_res_tile (unsigned short id, struct res_tile_t *tile_s);
extern unsigned char get_res_lut (unsigned short id, struct res_lut_t *lut_s);
extern unsigned char get_res_oreg (unsigned short id, struct res_oreg_t *oreg_s);
extern unsigned char get_res_icon (unsigned short id, struct res_icon_t *icon_s);
extern unsigned char get_res_td (unsigned short id, struct res_td_t *td_s);
extern unsigned char get_res_tof (unsigned short id, struct res_tof_t *tof_s);
extern void show_resource_header(unsigned short res_id);

#endif	/* __RC_H */
