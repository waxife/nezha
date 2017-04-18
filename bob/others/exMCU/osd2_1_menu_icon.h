#define OSD2_1_MENU_FONT_W  16L
#define OSD2_1_MENU_FONT_H  24L

#define OSD2_1_MENU_1BP_SIZE ((OSD2_1_MENU_FONT_W*OSD2_1_MENU_FONT_H)/8L) //bytes
#define OSD2_1_MENU_2BP_SIZE ((OSD2_1_MENU_FONT_W*OSD2_1_MENU_FONT_H)/4L) //bytes
#define OSD2_1_MENU_4BP_SIZE ((OSD2_1_MENU_FONT_W*OSD2_1_MENU_FONT_H)/2L) //bytes
#define OSD2_1_MENU_8BP_SIZE ((OSD2_1_MENU_FONT_W*OSD2_1_MENU_FONT_H)/1L) //bytes

#define OSD2_1_MENU_1BP_TOTAL (1+1L)
#define OSD2_1_MENU_2BP_TOTAL (0)
#define OSD2_1_MENU_4BP_TOTAL (0)

#define OSD2_1_MENU_TOTAL_LENGTH ((OSD2_1_MENU_1BP_SIZE*OSD2_1_MENU_1BP_TOTAL)+(OSD2_1_MENU_2BP_SIZE*OSD2_1_MENU_2BP_TOTAL)+(OSD2_1_MENU_4BP_TOTAL*OSD2_1_MENU_4BP_SIZE))
extern unsigned short const osd2_1_menu_menuTable[];

#define _ICON_SPACE_W16_H24_1BP_WIDTH  1
#define _ICON_SPACE_W16_H24_1BP_HEIGHT 1
extern unsigned char const _icon_space_w16_h24_1bp[];

#define _ROM_FONT_1_WIDTH  1
#define _ROM_FONT_1_HEIGHT 1
extern unsigned char const _rom_font_1[];

#define _ROM_FONT_2_WIDTH  1
#define _ROM_FONT_2_HEIGHT 1
extern unsigned char const _rom_font_2[];

#define _ROM_FONT_3_WIDTH  1
#define _ROM_FONT_3_HEIGHT 1
extern unsigned char const _rom_font_3[];

#define _ROM_FONT_4_WIDTH  1
#define _ROM_FONT_4_HEIGHT 1
extern unsigned char const _rom_font_4[];

#define _ROM_FONT_5_WIDTH  1
#define _ROM_FONT_5_HEIGHT 1
extern unsigned char const _rom_font_5[];

#define _ROM_FONT_6_WIDTH  1
#define _ROM_FONT_6_HEIGHT 1
extern unsigned char const _rom_font_6[];

#define _ROM_FONT_7_WIDTH  1
#define _ROM_FONT_7_HEIGHT 1
extern unsigned char const _rom_font_7[];

#define _ROM_FONT_8_WIDTH  1
#define _ROM_FONT_8_HEIGHT 1
extern unsigned char const _rom_font_8[];

#define _ROM_FONT_9_WIDTH  1
#define _ROM_FONT_9_HEIGHT 1
extern unsigned char const _rom_font_9[];

#define _ROM_FONT_10_WIDTH  1
#define _ROM_FONT_10_HEIGHT 1
extern unsigned char const _rom_font_10[];

#define _ICON_DUMMY_W16_H24_1BP_WIDTH  1
#define _ICON_DUMMY_W16_H24_1BP_HEIGHT 1
extern unsigned char const _icon_dummy_w16_h24_1bp[];

