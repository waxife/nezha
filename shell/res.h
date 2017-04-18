#ifndef __RES_H__
#define __RES_H__

enum PAGE_ID_DEF {
	ID_PAGE_A_COVER_PNG = 1,
	ID_PAGE_B_COVER_PNG,
	TOTAL_PAGE_NUM
};

#define ID_LAYOUT_1_A_COVER_PNG 1
//[TD Struct]
// color_addr : 0x000000
// index_addr : 0x018110
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_SP_RECORD_PNG 20
//[Sprite Struct]
// lut_addr   : 0x02F810
// index_addr : 0x02FC10
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_SNAPSHOT_PNG 22
//[Sprite Struct]
// lut_addr   : 0x034D10
// index_addr : 0x035110
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_MESSAGE_PNG 17
//[Sprite Struct]
// lut_addr   : 0x03A210
// index_addr : 0x03A610
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_PLAY_PNG 19
//[Sprite Struct]
// lut_addr   : 0x03F710
// index_addr : 0x03FB10
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_PICVIEW_PNG 18
//[Sprite Struct]
// lut_addr   : 0x044C10
// index_addr : 0x045010
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_SETTINGS_PNG 21
//[Sprite Struct]
// lut_addr   : 0x04A110
// index_addr : 0x04A510
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_LAYOUT_2_B_COVER_PNG 2
//[TD Struct]
// color_addr : 0x04F610
// index_addr : 0x067720
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_SP_TIME_PNG 23
//[Sprite Struct]
// lut_addr   : 0x07EE20
// index_addr : 0x07F220
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_LANGB_PNG 28
//[Sprite Struct]
// lut_addr   : 0x084320
// index_addr : 0x084720
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_BACKUP_PNG 14
//[Sprite Struct]
// lut_addr   : 0x089820
// index_addr : 0x089C20
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_SP_FORMAT_PNG 15
//[Sprite Struct]
// lut_addr   : 0x08ED20
// index_addr : 0x08F120
// line_jump  : 144
// width      : 144
// height     : 144
// count      : 1
#define ID_TWF_OSD_FONT 120
//[TWFont Struct]
// base_addr     : 0x094220
// font_width    : 24
// font_height   : 24
#define ID_ICON_SPACE_W16_H24_1BP 5022
//[ICON Struct]
// base_addr  : 0x094E4C
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_ICON_1BP_FONT_3 5163
//[ICON Struct]
// base_addr  : 0x094E7C
// font_w     : 16
// font_h     : 24
// width      : 36
// height     : 2
// bp         : 1
// count      : 72
#define ID_ICON_LETTER_ENG_ARIAL_0_PNG 24
//[ICON Struct]
// base_addr  : 0x095BFC
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_LETTER_ENG_ARIAL_1_PNG 25
//[ICON Struct]
// base_addr  : 0x095C5C
// font_w     : 16
// font_h     : 24
// width      : 14
// height     : 1
// bp         : 2
// count      : 14
#define ID_ICON_LETTER_ENG_ARIAL_2_PNG 26
//[ICON Struct]
// base_addr  : 0x09619C
// font_w     : 16
// font_h     : 24
// width      : 18
// height     : 1
// bp         : 2
// count      : 18
#define ID_ICON_LETTER_ENG_ARIAL_3_PNG 27
//[ICON Struct]
// base_addr  : 0x09685C
// font_w     : 16
// font_h     : 24
// width      : 8
// height     : 1
// bp         : 2
// count      : 8
#define ID_ICON_RIGHT_PNG 93
//[ICON Struct]
// base_addr  : 0x096B5C
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_DOWN_PNG 90
//[ICON Struct]
// base_addr  : 0x096BBC
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_UP_PNG 94
//[ICON Struct]
// base_addr  : 0x096C1C
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_LEFT_PNG 92
//[ICON Struct]
// base_addr  : 0x096C7C
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_FILL_PNG 91
//[ICON Struct]
// base_addr  : 0x096CDC
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_LANG_PNG2_PNG1 123
//[ICON Struct]
// base_addr  : 0x096D3C
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_LANG_PNG3_PNG1 124
//[ICON Struct]
// base_addr  : 0x096F7C
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_LANG_PNG4_PNG1 125
//[ICON Struct]
// base_addr  : 0x0971BC
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_LANG_PNG5_PNG1 126
//[ICON Struct]
// base_addr  : 0x09745C
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_LANG_PNG12_PNG1 133
//[ICON Struct]
// base_addr  : 0x0976FC
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_LANG_PNG13_PNG1 134
//[ICON Struct]
// base_addr  : 0x09799C
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_LANG_PNG0_PNG1 121
//[ICON Struct]
// base_addr  : 0x097C3C
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_LANG_PNG1_PNG1 122
//[ICON Struct]
// base_addr  : 0x097DBC
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_LANG_PNG6_PNG1 127
//[ICON Struct]
// base_addr  : 0x097F3C
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_LANG_PNG7_PNG1 128
//[ICON Struct]
// base_addr  : 0x09811C
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_LANG_PNG8_PNG1 129
//[ICON Struct]
// base_addr  : 0x0982FC
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_LANG_PNG9_PNG1 130
//[ICON Struct]
// base_addr  : 0x09847C
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_LANG_PNG10_PNG 141
//[ICON Struct]
// base_addr  : 0x0985FC
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_LANG_PNG18_PNG 147
//[ICON Struct]
// base_addr  : 0x09877C
// font_w     : 16
// font_h     : 24
// width      : 8
// height     : 1
// bp         : 2
// count      : 8
#define ID_ICON_LANG_PNG14_PNG 143
//[ICON Struct]
// base_addr  : 0x098A7C
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_LANG_PNG16_PNG 145
//[ICON Struct]
// base_addr  : 0x098CBC
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_LANG_PNG11_PNG 142
//[ICON Struct]
// base_addr  : 0x098EFC
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_LANG_PNG19_PNG 148
//[ICON Struct]
// base_addr  : 0x09907C
// font_w     : 16
// font_h     : 24
// width      : 8
// height     : 1
// bp         : 2
// count      : 8
#define ID_ICON_LANG_PNG15_PNG 144
//[ICON Struct]
// base_addr  : 0x09937C
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_LANG_PNG17_PNG 146
//[ICON Struct]
// base_addr  : 0x0995BC
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_GREEN_RESOURCE_DATA_PNG4_PNG 100
//[ICON Struct]
// base_addr  : 0x0997FC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_GREEN_RESOURCE_DATA_PNG1_PNG 97
//[ICON Struct]
// base_addr  : 0x099AFC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_GREEN_RESOURCE_DATA_PNG5_PNG 101
//[ICON Struct]
// base_addr  : 0x099DFC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_ORANGE_RESOURCE_DATA_PNG1_PNG 103
//[ICON Struct]
// base_addr  : 0x09A0FC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_ORANGE_RESOURCE_DATA_PNG0_PNG 102
//[ICON Struct]
// base_addr  : 0x09A3FC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_ORANGE_RESOURCE_DATA_PNG2_PNG 104
//[ICON Struct]
// base_addr  : 0x09A6FC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_ORANGE_RESOURCE_DATA_PNG4_PNG 106
//[ICON Struct]
// base_addr  : 0x09A9FC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_GREEN_RESOURCE_DATA_PNG0_PNG 96
//[ICON Struct]
// base_addr  : 0x09ACFC
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_DUMMY_W16_H24_1BP 5027
//[ICON Struct]
// base_addr  : 0x09AFFC
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_DUMMY_W16_H24_2BP 5028
//[ICON Struct]
// base_addr  : 0x09B02C
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_OSD2_1_ORG 5019
//[OREG Struct]
// base_addr  : 0x09B08C
// length     : 54
#define ID_OSD2_1_MENU 5018
//[MENU Struct]
// base_addr  : 0x09B0C2
// oram_addr  : 0x3BC5
// length     : 2160
// width      : 50
// height     : 20
#define ID_OSD2_1_LUT 5020
//[LUT Struct]
// lut_addr   : 0x09B932
// main_len   : 64
// sec_len    : 0
// re_2bp_len : 16
// re_bg_len  : 4
#define ID_OSD2_1_TILES 5021
//[Tile Struct]
// base_addr  : 0x09BA46
// length     : 3
//#define ID_OSD2_1_TILES 5252
//[Tile Struct]
// base_addr  : 0x09BA4C
// length     : 25
//#define ID_OSD2_1_TILES 5253
//[Tile Struct]
// base_addr  : 0x09BA7E
// length     : 25
#define ID_OSD2_2_ORG 5030
//[OREG Struct]
// base_addr  : 0x09BAB0
// length     : 54
#define ID_OSD2_2_MENU 5029
//[MENU Struct]
// base_addr  : 0x09BAE6
// oram_addr  : 0x3BC5
// length     : 2160
// width      : 50
// height     : 20
#define ID_OSD2_2_LUT 5031
//[LUT Struct]
// lut_addr   : 0x09C356
// main_len   : 60
// sec_len    : 0
// re_2bp_len : 12
// re_bg_len  : 4
#define ID_OSD2_2_TILES 5032
//[Tile Struct]
// base_addr  : 0x09C456
// length     : 3
//#define ID_OSD2_2_TILES 5254
//[Tile Struct]
// base_addr  : 0x09C45C
// length     : 17
//#define ID_OSD2_2_TILES 5255
//[Tile Struct]
// base_addr  : 0x09C47E
// length     : 17
#endif
