#ifndef __RES_H__
#define __RES_H__

enum PAGE_ID_DEF {
	ID_PAGE_VIDEO_VIEW=1,

	TOTAL_PAGE_NUM
};

#define ID_BOOT_LOGO 2
//[TD Struct]
// color_addr : 0x000000
// index_addr : 0x018110
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_SP_BUTTERFLY 4
//[Sprite Struct]
// lut_addr   : 0x02F810
// index_addr : 0x02FC10
// line_jump  : 136
// width      : 136
// height     : 990
// count      : 9
#define ID_EMU_SP_BUTTERFLY 5002
#define ID_VIDEO_MODE_VIDEO_VIEW 1
//[TD Struct]
// color_addr : 0x050A48
// index_addr : 0x068B58
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_SP_TAG1 5
//[Sprite Struct]
// lut_addr   : 0x080258
// index_addr : 0x080658
// line_jump  : 144
// width      : 144
// height     : 480
// count      : 1
#define ID_SP_TAG1_PHONE 6
//[Sprite Struct]
// lut_addr   : 0x091458
// index_addr : 0x091858
// line_jump  : 144
// width      : 144
// height     : 480
// count      : 1
#define ID_SP_TAG3 8
//[Sprite Struct]
// lut_addr   : 0x0A2658
// index_addr : 0x0A2A58
// line_jump  : 144
// width      : 144
// height     : 480
// count      : 1
#define ID_SP_CHECK 15
//[Sprite Struct]
// lut_addr   : 0x0B3858
// index_addr : 0x0B3C58
// line_jump  : 144
// width      : 144
// height     : 480
// count      : 1
#define ID_SP_CLK 16
//[Sprite Struct]
// lut_addr   : 0x0C4A58
// index_addr : 0x0C4E58
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_IMG_CTRL 17
//[Sprite Struct]
// lut_addr   : 0x0D72D8
// index_addr : 0x0D76D8
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_MSG 18
//[Sprite Struct]
// lut_addr   : 0x0E9B58
// index_addr : 0x0E9F58
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_PHOTO 19
//[Sprite Struct]
// lut_addr   : 0x0FC3D8
// index_addr : 0x0FC7D8
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_SETTING 20
//[Sprite Struct]
// lut_addr   : 0x10EC58
// index_addr : 0x10F058
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_VIDEO 21
//[Sprite Struct]
// lut_addr   : 0x1214D8
// index_addr : 0x1218D8
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_TAG2 22
//[Sprite Struct]
// lut_addr   : 0x133D58
// index_addr : 0x134158
// line_jump  : 156
// width      : 156
// height     : 480
// count      : 1
#define ID_SP_BTN_EFFCT 170
//[Sprite Struct]
// lut_addr   : 0x1465D8
// index_addr : 0x1469D8
// line_jump  : 80
// width      : 80
// height     : 86
// count      : 1
#define ID_SP_MDCTRL1 171
//[Sprite Struct]
// lut_addr   : 0x1484B8
// index_addr : 0x1488B8
// line_jump  : 80
// width      : 80
// height     : 480
// count      : 1
#define ID_SP_MDCTRL2 172
//[Sprite Struct]
// lut_addr   : 0x151EB8
// index_addr : 0x1522B8
// line_jump  : 80
// width      : 80
// height     : 480
// count      : 1
#define ID_MENU_MODE_MENU_VIEW 3
//[TD Struct]
// color_addr : 0x15B8B8
// index_addr : 0x1739C8
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_IMG_CTRL_IMG_CTRL_VIEW 50
//[TD Struct]
// color_addr : 0x18B0C8
// index_addr : 0x1A31D8
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_CITY_1_CITY 76
//[TD Struct]
// color_addr : 0x1BA8D8
// index_addr : 0x1D29E8
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_CITY_2_CITY_IMG_CTRL 77
//[TD Struct]
// color_addr : 0x1EA0E8
// index_addr : 0x2021F8
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_CITY_3_CITY_LIVE 78
//[TD Struct]
// color_addr : 0x2198F8
// index_addr : 0x231A08
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_TOWER_1_TOWER 82
//[TD Struct]
// color_addr : 0x249108
// index_addr : 0x261218
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_TOWER_2_TOWER_IMG_CTRL 83
//[TD Struct]
// color_addr : 0x278918
// index_addr : 0x290A28
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_TOWER_3_TOWER_LIVE 84
//[TD Struct]
// color_addr : 0x2A8128
// index_addr : 0x2C0238
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_FLWR_1_SUNFLOWER 79
//[TD Struct]
// color_addr : 0x2D7938
// index_addr : 0x2EFA48
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_FLWR_2_SUNFLOWER_IMG_CTRL 80
//[TD Struct]
// color_addr : 0x307148
// index_addr : 0x31F258
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_FLWR_3_SUNFLOWER_LIVE 81
//[TD Struct]
// color_addr : 0x336958
// index_addr : 0x34EA68
// line_jump  : 800
// width      : 800
// height     : 480
#define ID_TWF_ARIAL 159
//[TWFont Struct]
// base_addr     : 0x366168
// font_width    : 24
// font_height   : 24
#define ID_ICON_SPACE_W16_H24_1BP 5131
//[ICON Struct]
// base_addr  : 0x368D0B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_MASK_FONT 24
//[ICON Struct]
// base_addr  : 0x368D3B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_CTRL_BAR_3 51
//[ICON Struct]
// base_addr  : 0x368D6B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_CTRL_BAR_1 52
//[ICON Struct]
// base_addr  : 0x368D9B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_CTRL_BAR_2 53
//[ICON Struct]
// base_addr  : 0x368DCB
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_ICON_ICON_1BP_FONT_0 5133
//[ICON Struct]
// base_addr  : 0x368DFB
// font_w     : 16
// font_h     : 24
// width      : 15
// height     : 1
// bp         : 1
// count      : 15
#define ID_ICON_AUDIO_MASSAGE_ABNORMALITIES_ENGLISH 121
//[ICON Struct]
// base_addr  : 0x3690CB
// font_w     : 16
// font_h     : 24
// width      : 22
// height     : 1
// bp         : 2
// count      : 22
#define ID_ICON_AUDIO_MASSAGE_ABNORMALITIES_CHINESE 120
//[ICON Struct]
// base_addr  : 0x36990B
// font_w     : 16
// font_h     : 24
// width      : 22
// height     : 1
// bp         : 2
// count      : 22
#define ID_ICON_BACKUP_ENGLISH 123
//[ICON Struct]
// base_addr  : 0x36A14B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_BACKUP_CHINESE 122
//[ICON Struct]
// base_addr  : 0x36A32B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_COPY_ENGLISH 125
//[ICON Struct]
// base_addr  : 0x36A50B
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_COPY_CHINESE 124
//[ICON Struct]
// base_addr  : 0x36A68B
// font_w     : 16
// font_h     : 24
// width      : 4
// height     : 1
// bp         : 2
// count      : 4
#define ID_ICON_COPYING_ENGLISH 127
//[ICON Struct]
// base_addr  : 0x36A80B
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_COPYING_CHINESE 126
//[ICON Struct]
// base_addr  : 0x36AA4B
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_DELETE_ENGLISH 129
//[ICON Struct]
// base_addr  : 0x36AC8B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_DELETE_CHINESE 128
//[ICON Struct]
// base_addr  : 0x36AE6B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_DELETEING_ENGLISH 131
//[ICON Struct]
// base_addr  : 0x36B04B
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_DELETEING_CHINESE 130
//[ICON Struct]
// base_addr  : 0x36B2EB
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_DELETION_ABNORMALITIES_ENGLISH 133
//[ICON Struct]
// base_addr  : 0x36B58B
// font_w     : 16
// font_h     : 24
// width      : 21
// height     : 1
// bp         : 2
// count      : 21
#define ID_ICON_DELETION_ABNORMALITIES_CHINESE 132
//[ICON Struct]
// base_addr  : 0x36BD6B
// font_w     : 16
// font_h     : 24
// width      : 21
// height     : 1
// bp         : 2
// count      : 21
#define ID_ICON_DOORBELL_ABNORMALITIES_ENGLISH 135
//[ICON Struct]
// base_addr  : 0x36C54B
// font_w     : 16
// font_h     : 24
// width      : 17
// height     : 1
// bp         : 2
// count      : 17
#define ID_ICON_DOORBELL_ABNORMALITIES_CHINESE 134
//[ICON Struct]
// base_addr  : 0x36CBAB
// font_w     : 16
// font_h     : 24
// width      : 17
// height     : 1
// bp         : 2
// count      : 17
#define ID_ICON_LNSERTION_ABNORMALITIES_ENGLISH 137
//[ICON Struct]
// base_addr  : 0x36D20B
// font_w     : 16
// font_h     : 24
// width      : 22
// height     : 1
// bp         : 2
// count      : 22
#define ID_ICON_LNSERTION_ABNORMALITIES_CHINESE 136
//[ICON Struct]
// base_addr  : 0x36DA4B
// font_w     : 16
// font_h     : 24
// width      : 22
// height     : 1
// bp         : 2
// count      : 22
#define ID_ICON_MODIFICATION_ABNORMALITIES_ENGLISH 139
//[ICON Struct]
// base_addr  : 0x36E28B
// font_w     : 16
// font_h     : 24
// width      : 23
// height     : 1
// bp         : 2
// count      : 23
#define ID_ICON_MODIFICATION_ABNORMALITIES_CHINESE 138
//[ICON Struct]
// base_addr  : 0x36EB2B
// font_w     : 16
// font_h     : 24
// width      : 23
// height     : 1
// bp         : 2
// count      : 23
#define ID_ICON_NO_AUDIO_ENGLISH 141
//[ICON Struct]
// base_addr  : 0x36F3CB
// font_w     : 16
// font_h     : 24
// width      : 13
// height     : 1
// bp         : 2
// count      : 13
#define ID_ICON_NO_PICTURE_ENGLISH 143
//[ICON Struct]
// base_addr  : 0x36F8AB
// font_w     : 16
// font_h     : 24
// width      : 10
// height     : 1
// bp         : 2
// count      : 10
#define ID_ICON_NO_PICTURE_CHINESE 142
//[ICON Struct]
// base_addr  : 0x36FC6B
// font_w     : 16
// font_h     : 24
// width      : 10
// height     : 1
// bp         : 2
// count      : 10
#define ID_ICON_NO_VIDEO_ENGLISH 145
//[ICON Struct]
// base_addr  : 0x37002B
// font_w     : 16
// font_h     : 24
// width      : 9
// height     : 1
// bp         : 2
// count      : 9
#define ID_ICON_NO_VIDEO_CHINESE 144
//[ICON Struct]
// base_addr  : 0x37038B
// font_w     : 16
// font_h     : 24
// width      : 9
// height     : 1
// bp         : 2
// count      : 9
#define ID_ICON_NOR_SYSTEM_ABNORMALITIES_ENGLISH 147
//[ICON Struct]
// base_addr  : 0x3706EB
// font_w     : 16
// font_h     : 24
// width      : 21
// height     : 1
// bp         : 2
// count      : 21
#define ID_ICON_NOR_SYSTEM_ABNORMALITIES_CHINESE 146
//[ICON Struct]
// base_addr  : 0x370ECB
// font_w     : 16
// font_h     : 24
// width      : 21
// height     : 1
// bp         : 2
// count      : 21
#define ID_ICON_PHOTO_ABNORMALITIES_ENGLISH 149
//[ICON Struct]
// base_addr  : 0x3716AB
// font_w     : 16
// font_h     : 24
// width      : 15
// height     : 1
// bp         : 2
// count      : 15
#define ID_ICON_PHOTO_ABNORMALITIES_CHINESE 148
//[ICON Struct]
// base_addr  : 0x371C4B
// font_w     : 16
// font_h     : 24
// width      : 15
// height     : 1
// bp         : 2
// count      : 15
#define ID_ICON_RECORD_SERVICE_ABNORMALITIES_ENGLISH 151
//[ICON Struct]
// base_addr  : 0x3721EB
// font_w     : 16
// font_h     : 24
// width      : 21
// height     : 1
// bp         : 2
// count      : 21
#define ID_ICON_RECORD_SERVICE_ABNORMALITIES_CHINESE 150
//[ICON Struct]
// base_addr  : 0x3729CB
// font_w     : 16
// font_h     : 24
// width      : 21
// height     : 1
// bp         : 2
// count      : 21
#define ID_ICON_SD_CARD_ENGLISH 153
//[ICON Struct]
// base_addr  : 0x3731AB
// font_w     : 16
// font_h     : 24
// width      : 15
// height     : 1
// bp         : 2
// count      : 15
#define ID_ICON_SD_CARD_CHINESE 152
//[ICON Struct]
// base_addr  : 0x37374B
// font_w     : 16
// font_h     : 24
// width      : 15
// height     : 1
// bp         : 2
// count      : 15
#define ID_ICON_SD_SYSTEM_ABNORMALITIES_ENGLISH 155
//[ICON Struct]
// base_addr  : 0x373CEB
// font_w     : 16
// font_h     : 24
// width      : 20
// height     : 1
// bp         : 2
// count      : 20
#define ID_ICON_SD_SYSTEM_ABNORMALITIES_CHINESE 154
//[ICON Struct]
// base_addr  : 0x37446B
// font_w     : 16
// font_h     : 24
// width      : 20
// height     : 1
// bp         : 2
// count      : 20
#define ID_ICON_VIDEO_SIGNAL_ABNORMALITIES_ENGLISH 157
//[ICON Struct]
// base_addr  : 0x374BEB
// font_w     : 16
// font_h     : 24
// width      : 18
// height     : 1
// bp         : 2
// count      : 18
#define ID_ICON_VIDEO_SIGNAL_ABNORMALITIES_CHINESE 156
//[ICON Struct]
// base_addr  : 0x3752AB
// font_w     : 16
// font_h     : 24
// width      : 18
// height     : 1
// bp         : 2
// count      : 18
#define ID_ICON_BIG_NUM_0 32
//[ICON Struct]
// base_addr  : 0x37596B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_1 33
//[ICON Struct]
// base_addr  : 0x375AEB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_2 34
//[ICON Struct]
// base_addr  : 0x375C6B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_3 35
//[ICON Struct]
// base_addr  : 0x375DEB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_4 36
//[ICON Struct]
// base_addr  : 0x375F6B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_5 37
//[ICON Struct]
// base_addr  : 0x3760EB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_6 38
//[ICON Struct]
// base_addr  : 0x37626B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_7 39
//[ICON Struct]
// base_addr  : 0x3763EB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_8 40
//[ICON Struct]
// base_addr  : 0x37656B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_9 41
//[ICON Struct]
// base_addr  : 0x3766EB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 2
// count      : 4
#define ID_ICON_BIG_NUM_10 42
//[ICON Struct]
// base_addr  : 0x37686B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 2
// bp         : 2
// count      : 2
#define ID_ICON_BIG_NUM_11 43
//[ICON Struct]
// base_addr  : 0x37692B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 2
// bp         : 2
// count      : 2
#define ID_ICON_MAG 44
//[ICON Struct]
// base_addr  : 0x3769EB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 2
// count      : 2
#define ID_ICON_NOSD 45
//[ICON Struct]
// base_addr  : 0x376AAB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 2
// count      : 2
#define ID_ICON_PHOTO1 46
//[ICON Struct]
// base_addr  : 0x376B6B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 2
// count      : 2
#define ID_ICON_VIDEO1 47
//[ICON Struct]
// base_addr  : 0x376C2B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 2
// count      : 2
#define ID_ICON_ARR_DOWN 48
//[ICON Struct]
// base_addr  : 0x376CEB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 2
// count      : 2
#define ID_ICON_ARR_UP 49
//[ICON Struct]
// base_addr  : 0x376DAB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 2
// count      : 2
#define ID_ICON_SET_ARR_2BP 54
//[ICON Struct]
// base_addr  : 0x376E6B
// font_w     : 16
// font_h     : 24
// width      : 3
// height     : 1
// bp         : 2
// count      : 3
#define ID_ICON_SET_BELL_ENG 56
//[ICON Struct]
// base_addr  : 0x376F8B
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_SET_BELL_CHS 55
//[ICON Struct]
// base_addr  : 0x37722B
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_AUO_FS_ENGLISH 161
//[ICON Struct]
// base_addr  : 0x3774CB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_AUO_FS_CHINESE 160
//[ICON Struct]
// base_addr  : 0x3776AB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_BG_ENG 58
//[ICON Struct]
// base_addr  : 0x37788B
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_SET_BG_CHS 57
//[ICON Struct]
// base_addr  : 0x377B2B
// font_w     : 16
// font_h     : 24
// width      : 7
// height     : 1
// bp         : 2
// count      : 7
#define ID_ICON_BROWERS_MODE_ENGLISH 163
//[ICON Struct]
// base_addr  : 0x377DCB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_BROWERS_MODE_CHINESE 162
//[ICON Struct]
// base_addr  : 0x377FAB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_FMT_ENG 61
//[ICON Struct]
// base_addr  : 0x37818B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_FMT_CHS 60
//[ICON Struct]
// base_addr  : 0x37836B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_MD_RANGE_ENGLISH 165
//[ICON Struct]
// base_addr  : 0x37854B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_MD_RANGE_CHINESE 164
//[ICON Struct]
// base_addr  : 0x37872B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_LANG_ENG 63
//[ICON Struct]
// base_addr  : 0x37890B
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_SET_LANG_CHS 62
//[ICON Struct]
// base_addr  : 0x378B4B
// font_w     : 16
// font_h     : 24
// width      : 6
// height     : 1
// bp         : 2
// count      : 6
#define ID_ICON_PIC_FS_ENGLISH 167
//[ICON Struct]
// base_addr  : 0x378D8B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_PIC_FS_CHINESE 166
//[ICON Struct]
// base_addr  : 0x378F6B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_VER_ENG 68
//[ICON Struct]
// base_addr  : 0x37914B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_VER_CHS 67
//[ICON Struct]
// base_addr  : 0x37932B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_TP_CAB_ENGLISH 169
//[ICON Struct]
// base_addr  : 0x37950B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_TP_CAB_CHINESE 168
//[ICON Struct]
// base_addr  : 0x3796EB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_OPT_LANG_ENG 66
//[ICON Struct]
// base_addr  : 0x3798CB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_SET_OPT_LANG_CHS 65
//[ICON Struct]
// base_addr  : 0x379AAB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_0 69
//[ICON Struct]
// base_addr  : 0x379C8B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_1 70
//[ICON Struct]
// base_addr  : 0x379E6B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_2 71
//[ICON Struct]
// base_addr  : 0x37A04B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_3 72
//[ICON Struct]
// base_addr  : 0x37A22B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_4 73
//[ICON Struct]
// base_addr  : 0x37A40B
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_5 74
//[ICON Struct]
// base_addr  : 0x37A5EB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_WEEK_6 75
//[ICON Struct]
// base_addr  : 0x37A7CB
// font_w     : 16
// font_h     : 24
// width      : 5
// height     : 1
// bp         : 2
// count      : 5
#define ID_ICON_NUM 26
//[ICON Struct]
// base_addr  : 0x37A9AB
// font_w     : 16
// font_h     : 24
// width      : 13
// height     : 1
// bp         : 2
// count      : 13
#define ID_ICON_SET_NUM 64
//[ICON Struct]
// base_addr  : 0x37AE8B
// font_w     : 16
// font_h     : 24
// width      : 14
// height     : 1
// bp         : 2
// count      : 14
#define ID_ICON_LED_YELLOW_0 13
//[ICON Struct]
// base_addr  : 0x37B3CB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_LED_YELLOW_1 14
//[ICON Struct]
// base_addr  : 0x37B6CB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 4
// count      : 2
#define ID_ICON_LED_BLUE_0 9
//[ICON Struct]
// base_addr  : 0x37B84B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_LED_BLUE_1 10
//[ICON Struct]
// base_addr  : 0x37BB4B
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 4
// count      : 2
#define ID_ICON_LED_RED_0 11
//[ICON Struct]
// base_addr  : 0x37BCCB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 2
// bp         : 4
// count      : 4
#define ID_ICON_LED_RED_1 12
//[ICON Struct]
// base_addr  : 0x37BFCB
// font_w     : 16
// font_h     : 24
// width      : 2
// height     : 1
// bp         : 4
// count      : 2
#define ID_ICON_DUMMY_W16_H24_2BP 5135
//[ICON Struct]
// base_addr  : 0x37C14B
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 1
#define ID_ICON_DUMMY_W16_H24_1BP 5140
//[ICON Struct]
// base_addr  : 0x37C1AB
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 1
// count      : 1
#define ID_OSD2_1_ORG 5056
//[OREG Struct]
// base_addr  : 0x37C1DB
// length     : 54
#define ID_OSD2_1_MENU 5054
//[MENU Struct]
// base_addr  : 0x37C211
// oram_addr  : 0x1BC5
// length     : 2160
// width      : 50
// height     : 20
#define ID_ICON_LOC_AUDIO_MASSAGE_ABNORMALITIES_ENGLISH_ICON_TBL 5141
//[Tile Struct]
// base_addr  : 0x37CA81
// length     : 2
// ids     :  121 120
#define ID_ICON_LOC_BACKUP_ENGLISH_ICON_TBL 5142
//[Tile Struct]
// base_addr  : 0x37CA85
// length     : 2
// ids     :  123 122
#define ID_ICON_LOC_COPY_ENGLISH_ICON_TBL 5143
//[Tile Struct]
// base_addr  : 0x37CA89
// length     : 2
// ids     :  125 124
#define ID_ICON_LOC_COPYING_ENGLISH_ICON_TBL 5144
//[Tile Struct]
// base_addr  : 0x37CA8D
// length     : 2
// ids     :  127 126
#define ID_ICON_LOC_DELETE_ENGLISH_ICON_TBL 5145
//[Tile Struct]
// base_addr  : 0x37CA91
// length     : 2
// ids     :  129 128
#define ID_ICON_LOC_DELETEING_ENGLISH_ICON_TBL 5146
//[Tile Struct]
// base_addr  : 0x37CA95
// length     : 2
// ids     :  131 130
#define ID_ICON_LOC_DELETION_ABNORMALITIES_ENGLISH_ICON_TBL 5147
//[Tile Struct]
// base_addr  : 0x37CA99
// length     : 2
// ids     :  133 132
#define ID_ICON_LOC_DOORBELL_ABNORMALITIES_ENGLISH_ICON_TBL 5148
//[Tile Struct]
// base_addr  : 0x37CA9D
// length     : 2
// ids     :  135 134
#define ID_ICON_LOC_LNSERTION_ABNORMALITIES_ENGLISH_ICON_TBL 5149
//[Tile Struct]
// base_addr  : 0x37CAA1
// length     : 2
// ids     :  137 136
#define ID_ICON_LOC_MODIFICATION_ABNORMALITIES_ENGLISH_ICON_TBL 5150
//[Tile Struct]
// base_addr  : 0x37CAA5
// length     : 2
// ids     :  139 138
#define ID_ICON_LOC_NO_PICTURE_ENGLISH_ICON_TBL 5151
//[Tile Struct]
// base_addr  : 0x37CAA9
// length     : 2
// ids     :  143 142
#define ID_ICON_LOC_NO_VIDEO_ENGLISH_ICON_TBL 5152
//[Tile Struct]
// base_addr  : 0x37CAAD
// length     : 2
// ids     :  145 144
#define ID_ICON_LOC_NOR_SYSTEM_ABNORMALITIES_ENGLISH_ICON_TBL 5153
//[Tile Struct]
// base_addr  : 0x37CAB1
// length     : 2
// ids     :  147 146
#define ID_ICON_LOC_PHOTO_ABNORMALITIES_ENGLISH_ICON_TBL 5154
//[Tile Struct]
// base_addr  : 0x37CAB5
// length     : 2
// ids     :  149 148
#define ID_ICON_LOC_RECORD_SERVICE_ABNORMALITIES_ENGLISH_ICON_TBL 5155
//[Tile Struct]
// base_addr  : 0x37CAB9
// length     : 2
// ids     :  151 150
#define ID_ICON_LOC_SD_CARD_ENGLISH_ICON_TBL 5156
//[Tile Struct]
// base_addr  : 0x37CABD
// length     : 2
// ids     :  153 152
#define ID_ICON_LOC_SD_SYSTEM_ABNORMALITIES_ENGLISH_ICON_TBL 5157
//[Tile Struct]
// base_addr  : 0x37CAC1
// length     : 2
// ids     :  155 154
#define ID_ICON_LOC_VIDEO_SIGNAL_ABNORMALITIES_ENGLISH_ICON_TBL 5158
//[Tile Struct]
// base_addr  : 0x37CAC5
// length     : 2
// ids     :  157 156
#define ID_OSD2_1_LUT 5055
//[LUT Struct]
// lut_addr   : 0x37CAC9
// main_len   : 20
// sec_len    : 0
// re_2bp_len : 4
// re_bg_len  : 4
#define ID_OSD2_1_TILES 5057
//[Tile Struct]
// base_addr  : 0x37CB21
// length     : 3
//#define ID_OSD2_1_TILES 5136
//[Tile Struct]
// base_addr  : 0x37CB27
// length     : 4
//#define ID_OSD2_1_TILES 5137
//[Tile Struct]
// base_addr  : 0x37CB2F
// length     : 4
#define ID_MAIN_MENU_OREG 5079
//[OREG Struct]
// base_addr  : 0x37CB37
// length     : 54
#define ID_MAIN_MENU 5077
//[MENU Struct]
// base_addr  : 0x37CB6D
// oram_addr  : 0x1BC5
// length     : 2160
// width      : 50
// height     : 20
#define ID_ICON_LOC_SET_LANG_ENG_ICON_TBL 5159
//[Tile Struct]
// base_addr  : 0x37D3DD
// length     : 2
// ids     :  63 62
#define ID_ICON_LOC_AUO_FS_ENGLISH_ICON_TBL 5160
//[Tile Struct]
// base_addr  : 0x37D3E1
// length     : 2
// ids     :  161 160
#define ID_ICON_LOC_SET_BG_ENG_ICON_TBL 5161
//[Tile Struct]
// base_addr  : 0x37D3E5
// length     : 2
// ids     :  58 57
#define ID_ICON_LOC_BROWERS_MODE_ENGLISH_ICON_TBL 5162
//[Tile Struct]
// base_addr  : 0x37D3E9
// length     : 2
// ids     :  163 162
#define ID_ICON_LOC_SET_BELL_ENG_ICON_TBL 5163
//[Tile Struct]
// base_addr  : 0x37D3ED
// length     : 2
// ids     :  56 55
#define ID_ICON_LOC_TP_CAB_ENGLISH_ICON_TBL 5164
//[Tile Struct]
// base_addr  : 0x37D3F1
// length     : 2
// ids     :  169 168
#define ID_ICON_LOC_SET_FMT_ENG_ICON_TBL 5165
//[Tile Struct]
// base_addr  : 0x37D3F5
// length     : 2
// ids     :  61 60
#define ID_ICON_LOC_MD_RANGE_ENGLISH_ICON_TBL 5166
//[Tile Struct]
// base_addr  : 0x37D3F9
// length     : 2
// ids     :  165 164
#define ID_ICON_LOC_PIC_FS_ENGLISH_ICON_TBL 5167
//[Tile Struct]
// base_addr  : 0x37D3FD
// length     : 2
// ids     :  167 166
#define ID_ICON_LOC_SET_VER_ENG_ICON_TBL 5168
//[Tile Struct]
// base_addr  : 0x37D401
// length     : 2
// ids     :  68 67
#define ID_ICON_LOC_SET_OPT_LANG_ENG_ICON_TBL 5169
//[Tile Struct]
// base_addr  : 0x37D405
// length     : 2
// ids     :  66 65
#define ID_MAIN_MENU_LUT 5078
//[LUT Struct]
// lut_addr   : 0x37D409
// main_len   : 100
// sec_len    : 0
// re_2bp_len : 36
// re_bg_len  : 4
#define ID_MAIN_MENU_OPT_CH0_ICON 5170
//[ICON Struct]
// base_addr  : 0x37D5C1
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 148
#define ID_MAIN_MENU_OPT_CH1_ICON 5171
//[ICON Struct]
// base_addr  : 0x380D41
// font_w     : 16
// font_h     : 24
// width      : 1
// height     : 1
// bp         : 2
// count      : 148
#define ID_MAIN_MENU_TILE 5080
//[Tile Struct]
// base_addr  : 0x3844C1
// length     : 3
//#define ID_MAIN_MENU_TILE 5138
//[Tile Struct]
// base_addr  : 0x3844C7
// length     : 1
//#define ID_MAIN_MENU_TILE 5139
//[Tile Struct]
// base_addr  : 0x3844C9
// length     : 1
#endif
