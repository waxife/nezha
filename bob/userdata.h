/**
 *  @file   userdata.h
 *  @brief
 *  $Id: userdata.h,v 1.1.1.1 2014/12/17 03:30:09 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2014/3/12  Ken 	New file.
 *
 */


#ifndef USERDATA_H_
#define USERDATA_H_
#if 0
enum {
	RU_SET_LANG_INFO = 0,
	RU_SET_WP_INFO,
	RU_SET_BELL_INFO,
	CONTRAST,
	BRIGHTNESS,
	SATURATION,
	PICTURE_FS,
	AUDMASG_FS,
	MD_RANGE_X,
	MD_RANGE_Y,
	MD_RANGE_W,
	MD_RANGE_H,
	BROWS_TYPE,
	USERDATA_TOTAL
};
#endif

enum {
	RESERVE_0 = 0,
	RESERVE_1,
	RESERVE_2,
	E_CONTRAST,
	E_BRIGHTNESS,
	E_SATURATION,
	E_PICTURE_FS,
	E_AUDMASG_FS,
	E_YEAR,
	E_MONTH,
	E_DAY,
	E_HOUR,
	E_MINUTE,
	USERDATA_TOTAL
};


extern const int default_data[];
extern const int limit_data[];
extern int user_datas[];
#define background_idx	user_datas[RU_SET_WP_INFO]
#define bell_idx		user_datas[RU_SET_BELL_INFO]
#define PHOTO_FS		user_datas[PICTURE_FS]
#define AUDIO_FS		user_datas[AUDMASG_FS]
#define md_mask_x		user_datas[MD_RANGE_X]
#define md_mask_y		user_datas[MD_RANGE_Y]
#define md_mask_w		user_datas[MD_RANGE_W]
#define md_mask_h		user_datas[MD_RANGE_H]
#define BROWSER_MODE	user_datas[BROWS_TYPE]

void get_userdata (int id);
void get_userdata_all (void);
void save_userdata (int id);
void save_userdata_all (void);
void userdata_init (void);

#endif /* USERDATA_H_ */
