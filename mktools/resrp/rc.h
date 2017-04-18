#ifndef RC_H_INCLUDED
#define RC_H_INCLUDED

#define LAYOUT_INFO_ADDR		0x100
#define LAYOUT_INFO_SIZE		28
#define LAYOUT_USERDATA_SIZE	0x1000
#define LAYOUT_MAGIC  			0x1a1a1010
#define LAYOUT_MAGIC_ADDR		LAYOUT_INFO_ADDR
#define LAYOUT_TEXTSTART		LAYOUT_INFO_ADDR + 4
#define LAYOUT_TEXTSIZE			LAYOUT_INFO_ADDR + 8
#define LAYOUT_USERSTART		LAYOUT_INFO_ADDR + 12
#define LAYOUT_USERSIZE			LAYOUT_INFO_ADDR + 16
#define LAYOUT_RESSTART			LAYOUT_INFO_ADDR + 20
#define LAYOUT_RESSIZE			LAYOUT_INFO_ADDR + 24

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
#define TYPE_TWBC           9
#define TYPE_TOFONT         10

/*****************************************************************************/
#define DWORD int

#pragma pack(push) 	/* push current alignment to stack */
#pragma pack(1) 	/* set alignment to 1 byte boundary */
struct res_item_t {
	unsigned short    id;
	unsigned char     type;
	unsigned char     subtype;	/* dummy */
	unsigned long     offset;
};
struct res_t {
    unsigned long     resource_magic;  	/* = 0x12347733 */
    unsigned short    nelements;
    struct res_item_t desc[1];			/* sizeof (desc[0]) == 8 */
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};

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
};
#pragma pack(pop)

union item_u{
	char data[16];
	struct res_img_t img;
	struct res_spr_t spr;
	struct res_emu_t emu;
	struct res_menu_t menu;
	struct res_tile_t tile;
	struct res_lut_t lut;
	struct res_oreg_t oreg;
	struct res_icon_t icon;
	struct res_td_t twbc;
	struct res_tof_t tof;
};

#endif // RC_H_INCLUDED
