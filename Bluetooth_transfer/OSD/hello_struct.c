#include "tw_widget_sys.h"
#include "res.h"
#include "hello.h"

struct tw_td const layout_1_tower = {
    ID_LAYOUT_1_TOWER, 0, 0, 800, 480
};

struct tw_spr const hello_butterfly = {
    ID_SP_BUTTERFLY, 655, 191, 136, 110, 9
};

struct tw_menu const	hello_hello_OSD_Menu = {
    ID_HELLO_OSD_MENU,
    ID_HELLO_OSD_MENU_TILE,
    ID_HELLO_OSD_MENU_LUT,
    ID_HELLO_OSD_MENU_OREG,
    0Xffff,
    0,
    0,
    50,
    20,
    7109,
    16,
    24,
};

unsigned char const hello_rom_font_4_ids[] = {
	66, 69, 55, 58, 255, 63, 57, 69, 68, 39, };

struct tw_icons const hello_rom_font_4 = {
	0,
	1,
	2,
	3,
	10,
	1,
	hello_rom_font_4_ids,
	2,
};

unsigned char const hello_rom_font_5_ids[] = {
	66, 69, 55, 58, 255, 63, 57, 69, 68, 40, };

struct tw_icons const hello_rom_font_5 = {
	0,
	2,
	2,
	9,
	10,
	1,
	hello_rom_font_5_ids,
	2,
};

struct tw_icon const hello_big_num_0 = {
	ID_ICON_BIG_NUM_0,
	0,
	2,
	12,
	2,
	2,
	122,
};

struct tw_icon const hello_big_num_1 = {
	ID_ICON_BIG_NUM_1,
	0,
	2,
	12,
	2,
	2,
	126,
};

struct tw_icon const hello_big_num_2 = {
	ID_ICON_BIG_NUM_2,
	0,
	2,
	12,
	2,
	2,
	130,
};

struct tw_icon const hello_big_num_3 = {
	ID_ICON_BIG_NUM_3,
	0,
	2,
	12,
	2,
	2,
	134,
};

struct tw_icon const hello_big_num_4 = {
	ID_ICON_BIG_NUM_4,
	0,
	2,
	12,
	2,
	2,
	138,
};

struct tw_icon const hello_big_num_5 = {
	ID_ICON_BIG_NUM_5,
	0,
	2,
	12,
	2,
	2,
	142,
};

struct tw_icon const hello_big_num_6 = {
	ID_ICON_BIG_NUM_6,
	0,
	2,
	12,
	2,
	2,
	146,
};

struct tw_icon const hello_big_num_7 = {
	ID_ICON_BIG_NUM_7,
	0,
	2,
	12,
	2,
	2,
	150,
};

struct tw_icon const hello_big_num_8 = {
	ID_ICON_BIG_NUM_8,
	0,
	2,
	12,
	2,
	2,
	154,
};

struct tw_icon const hello_big_num_9 = {
	ID_ICON_BIG_NUM_9,
	0,
	2,
	12,
	2,
	2,
	158,
};

struct tw_icon const hello_big_num_10 = {
	ID_ICON_BIG_NUM_10,
	0,
	2,
	12,
	1,
	2,
	162,
};

struct tw_icon const hello_big_num_11 = {
	ID_ICON_BIG_NUM_11,
	0,
	2,
	12,
	1,
	2,
	164,
};

