#ifndef __RES_H__
#define __RES_H__

enum PAGE_ID_DEF {
	ID_PAGE_ST_LOGO = 1,
	TOTAL_PAGE_NUM
};

#define ID_LAYOUT_1_IMG 1
//[TD Struct]
// color_addr : 0x000000
// index_addr : 0x018110
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_ICON_SPACE_W16_H24_1BP 5020
//[ICON Struct]
// base_addr  : 0x02F810
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_ROM_FONT_1 2
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_2 3
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_3 4
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_4 5
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_5 6
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_6 7
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_7 8
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_8 9
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_9 10
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_10 11
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_11 12
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_12 13
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_13 14
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_131 15
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_DUMMY_W16_H24_1BP 5021
//[ICON Struct]
// base_addr  : 0x02F840
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_OSD2_0_ORG 5005
//[OREG Struct]
// base_addr  : 0x02F870
// length     : 54
#define ID_OSD2_0_MENU 5003
//[MENU Struct]
// base_addr  : 0x02F8A6
// oram_addr  : 0x1BC5
// length     : 2160
// width      : 50
// height     : 20
#define ID_OSD2_0_LUT 5004
//[LUT Struct]
// lut_addr   : 0x030116
// main_len   : 16
// sec_len    : 0
// re_2bp_len : 0
// re_bg_len  : 4
#define ID_OSD2_0_TILES 5006
//[Tile Struct]
// base_addr  : 0x03015A
// length     : 16
//#define ID_OSD2_0_TILES 5006
//[Tile Struct]
// base_addr  : 0x03015A
// length     : 16
#endif
