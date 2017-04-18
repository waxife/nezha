#ifndef __RES_H__
#define __RES_H__

enum PAGE_ID_DEF {
	ID_HELLO = 1,
	TOTAL_PAGE_NUM
};

#define ID_LAYOUT_1_TOWER 1
//[TD Struct]
// color_addr : 0x000000
// index_addr : 0x018110
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_SP_BUTTERFLY 2
//[Sprite Struct]
// lut_addr   : 0x02F810
// index_addr : 0x02FC10
// line_jump  : 136
// width      : 136
// height     : 990
// count      : 9
#define ID_EMU_SP_BUTTERFLY 5004
#define ID_ICON_SPACE_W16_H24_1BP 5017
//[ICON Struct]
// base_addr  : 0x050A48
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_ROM_FONT_4 6
//[ICON Struct]
// base_addr  : 0x050A78
// font_w     : 16
// font_h     : 24
// width      : 10
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_ROM_FONT_5 7
//[ICON Struct]
// base_addr  : 0x050A78
// font_w     : 16
// font_h     : 24
// width      : 10
// height     : 1
// bp         : 1
// count      : 0
#define ID_ICON_BIG_NUM_0 8
//[ICON Struct]
// base_addr  : 0x050A78
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_1 9
//[ICON Struct]
// base_addr  : 0x050BF8
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_2 10
//[ICON Struct]
// base_addr  : 0x050D78
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_3 11
//[ICON Struct]
// base_addr  : 0x050EF8
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_4 12
//[ICON Struct]
// base_addr  : 0x051078
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_5 13
//[ICON Struct]
// base_addr  : 0x0511F8
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_6 14
//[ICON Struct]
// base_addr  : 0x051378
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_7 15
//[ICON Struct]
// base_addr  : 0x0514F8
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_8 16
//[ICON Struct]
// base_addr  : 0x051678
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_9 17
//[ICON Struct]
// base_addr  : 0x0517F8
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_10 18
//[ICON Struct]
// base_addr  : 0x051978
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 2
// bp         : 2
// count      : 2
#define ID_ICON_BIG_NUM_11 19
//[ICON Struct]
// base_addr  : 0x051A38
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 2
// bp         : 2
// count      : 2
#define ID_ICON_DUMMY_W16_H24_1BP 5032
//[ICON Struct]
// base_addr  : 0x051AF8
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_HELLO_OSD_MENU_OREG 5014
//[OREG Struct]
// base_addr  : 0x051B28
// length     : 54
#define ID_HELLO_OSD_MENU 5013
//[MENU Struct]
// base_addr  : 0x051B5E
// oram_addr  : 0x1BC5
// length     : 2160
// width      : 50
// height     : 20
#define ID_HELLO_OSD_MENU_LUT 5015
//[LUT Struct]
// lut_addr   : 0x0523CE
// main_len   : 20
// sec_len    : 0
// re_2bp_len : 4
// re_bg_len  : 4
#define ID_HELLO_OSD_MENU_TILE 5016
//[Tile Struct]
// base_addr  : 0x052426
// length     : 16
//#define ID_HELLO_OSD_MENU_TILE 5016
//[Tile Struct]
// base_addr  : 0x052426
// length     : 16
#endif
