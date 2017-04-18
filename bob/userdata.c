/**
 *  @file   userdata.c
 *  @brief
 *  $Id: userdata.c,v 1.1.1.1 2014/12/17 03:30:09 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/3/12  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <registry2.h>
#include <mconfig.h>
#include "userdata.h"

const int default_data[USERDATA_TOTAL] = {
	0,		// RU_SET_LANG_INFO
	0,		// RU_SET_WP_INFO,
	0,		// RU_SET_BELL_INFO,
	0x80,	// CONTRAST,
	0x80,	// BRIGHTNESS,
	0x80,	// SATURATION,
	NORFS,	// PICTURE_FS,
	NORFS,	// AUDMASG_FS,
	0,		// MD_RANGE_X,
	0,		// MD_RANGE_Y,
	720,	// MD_RANGE_W,
	480,	// MD_RANGE_H,
	0,		// BROWS_TYPE
};

const int limit_data[USERDATA_TOTAL] = {
	2,		// RU_SET_LANG_INFO
	4,		// RU_SET_WP_INFO,
	3,		// RU_SET_BELL_INFO,
	0xFF,	// CONTRAST,
	0xFF,	// BRIGHTNESS,
	0xFF,	// SATURATION,
	FS_NUM,	// PICTURE_FS,
	FS_NUM,	// AUDMASG_FS,
	(720-8),// MD_RANGE_X,
	(480-8),// MD_RANGE_Y,
	720,	// MD_RANGE_W,
	480,	// MD_RANGE_H,
	2, 		// BROWS_TYPE
};

int user_datas[USERDATA_TOTAL] = {
	0,		// RU_SET_LANG_INFO
	0,		// RU_SET_WP_INFO,
	0,		// RU_SET_BELL_INFO,
	0x80,	// CONTRAST,
	0x80,	// BRIGHTNESS,
	0x80,	// SATURATION,
	NORFS,	// PICTURE_FS,
	NORFS,	// AUDMASG_FS,
	0,		// MD_RANGE_X,
	0,		// MD_RANGE_Y,
	720,	// MD_RANGE_W,
	480,	// MD_RANGE_H,
	0,		// BROWS_TYPE
};

void get_userdata (int id)
{
	registry2_get(id, &user_datas[id], default_data[id]);
	if(user_datas[id] >= limit_data[id])
		user_datas[id] = default_data[id];
}

void get_userdata_all (void)
{
	int i = 0;

	for(i = 0; i < USERDATA_TOTAL; i++) {
		get_userdata(i);
    }
}

void save_userdata (int id)
{
	registry2_set(id, user_datas[id]);
}

void save_userdata_all (void)
{
	registry2_burst_set(RU_SET_LANG_INFO, user_datas, USERDATA_TOTAL);
}

void userdata_init (void)
{
	get_userdata_all();
}
