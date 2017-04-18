#include "tw_ex_sys.h"
#include "res_full.h"
#include "page_0.h"

struct tw_menu const	page_0_osd2_1_menu = {
  0x000096+RES_OFFSET,
  &page_0_osd2_1_tile,
  &page_0_osd2_1_lut,
  &page_0_osd2_1_org,
  0,
  0,
  0,
  50,
  20,
  7109,
  16,
  24,
};

unsigned char const page_0_rom_font_1_ids[] = {
	38, };

struct tw_icons const page_0_rom_font_1 = {
  0x000030+RES_OFFSET,
  33,
  0,
  0,
  1,
  1,
  (unsigned char*)page_0_rom_font_1_ids,
  2,
};

unsigned char const page_0_rom_font_2_ids[] = {
	39, };

struct tw_icons const page_0_rom_font_2 = {
  0x000030+RES_OFFSET,
  33,
  0,
  1,
  1,
  1,
  (unsigned char*)page_0_rom_font_2_ids,
  2,
};

unsigned char const page_0_rom_font_3_ids[] = {
	40, };

struct tw_icons const page_0_rom_font_3 = {
  0x000030+RES_OFFSET,
  33,
  0,
  2,
  1,
  1,
  (unsigned char*)page_0_rom_font_3_ids,
  2,
};

unsigned char const page_0_rom_font_4_ids[] = {
	41, };

struct tw_icons const page_0_rom_font_4 = {
  0x000030+RES_OFFSET,
  33,
  0,
  3,
  1,
  1,
  (unsigned char*)page_0_rom_font_4_ids,
  2,
};

unsigned char const page_0_rom_font_5_ids[] = {
	42, };

struct tw_icons const page_0_rom_font_5 = {
  0x000030+RES_OFFSET,
  33,
  0,
  4,
  1,
  1,
  (unsigned char*)page_0_rom_font_5_ids,
  2,
};

unsigned char const page_0_rom_font_6_ids[] = {
	43, };

struct tw_icons const page_0_rom_font_6 = {
  0x000030+RES_OFFSET,
  33,
  0,
  0,
  1,
  1,
  (unsigned char*)page_0_rom_font_6_ids,
  2,
};

unsigned char const page_0_rom_font_7_ids[] = {
	44, };

struct tw_icons const page_0_rom_font_7 = {
  0x000030+RES_OFFSET,
  33,
  0,
  0,
  1,
  1,
  (unsigned char*)page_0_rom_font_7_ids,
  2,
};

unsigned char const page_0_rom_font_8_ids[] = {
	45, };

struct tw_icons const page_0_rom_font_8 = {
  0x000030+RES_OFFSET,
  33,
  0,
  0,
  1,
  1,
  (unsigned char*)page_0_rom_font_8_ids,
  2,
};

unsigned char const page_0_rom_font_9_ids[] = {
	46, };

struct tw_icons const page_0_rom_font_9 = {
  0x000030+RES_OFFSET,
  33,
  0,
  0,
  1,
  1,
  (unsigned char*)page_0_rom_font_9_ids,
  2,
};

unsigned char const page_0_rom_font_10_ids[] = {
	47, };

struct tw_icons const page_0_rom_font_10 = {
  0x000030+RES_OFFSET,
  33,
  0,
  0,
  1,
  1,
  (unsigned char*)page_0_rom_font_10_ids,
  2,
};

struct tw_luts const page_0_osd2_1_lut = {
  0x000906+RES_OFFSET,
  16,//main len
  0,//sec len
  0,//2bp len
  4//bg len
};

struct tw_regs const page_0_osd2_1_org = {
  0x000060+RES_OFFSET,
  54
};

unsigned long const osd2_1_tile_tbl[] = {
  0x000000+RES_OFFSET,  _OSD2_1_MENU_1BP_SIZE*1,
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  //0,                  _OSD2_1_MENU_1BP_SIZE*0,  //rom font
  0x000030+RES_OFFSET,  _OSD2_1_MENU_1BP_SIZE*1,
};

struct tw_tiles const page_0_osd2_1_tile = {
  osd2_1_tile_tbl,
  2//length
};

