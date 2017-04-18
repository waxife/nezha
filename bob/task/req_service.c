/**
 *  @file   req_service.c
 *  @brief
 *  $Id: req_service.c,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ls.h>
#include <codec.h>
#include <video_det.h>

#include "tw_widget_sys.h"

#define DBG_HELPER
#include <debug.h>

#include "play_service.h"
#include "rec_service.h"
#include "pview_service.h"
#include "snap_service.h"
#include "auplay_service.h"
#include "aumsg_service.h"
#include "slide_service.h"
#include "md_service.h"
#include "lsf_service.h"

#include "mdvid.h"
#include "file_manip.h"

#include "req_service.h"

static struct playsrv_opt_t
playsrv_opt = {
	FATFS,
	0
};
/**
 * @func    req_playback_srv
 * @brief   request a playback service
 * @param   none
 * @return  none
 */
void req_playback_srv (void)
{
	play_srv(&playsrv_opt);
}

static struct recsrv_opt_t
recsrv_opt = {
	FATFS
};
/**
 * @func    req_record_srv
 * @brief   request a record service
 * @param   none
 * @return  none
 */
void req_record_srv (void)
{
	rec_srv(&recsrv_opt);
}

static struct pviewsrv_opt_t
pviewsrv_opt = {
	NORFS,
	0
};

int set_pviewsrv_fs (int fs)
{
	int rc = 0;

	if(fs == NORFS) {
		pviewsrv_opt.dev = NORFS;
	} else if (fs == FATFS) {
		pviewsrv_opt.dev = FATFS;
	} else {
		ERROR("unknown of file system!\n");
		rc = -1;
	}

	return rc;
}

/**
 * @func    req_picview_srv
 * @brief   request a picture view service
 * @param   none
 * @return  none
 */
void req_picview_srv (int fs)
{
	set_pviewsrv_fs(fs);
	pview_srv(&pviewsrv_opt);
}

static struct snapsrv_opt_t
snapsrv_opt = {
	NORFS
};

int set_snapsrv_fs (int fs)
{
	int rc = 0;

	if(fs == NORFS) {
		snapsrv_opt.dev = NORFS;
	} else if (fs == FATFS) {
		snapsrv_opt.dev = FATFS;
	} else {
		ERROR("unknown of file system!\n");
		rc = -1;
	}

	return rc;
}

/**
 * @func    req_snapshot_srv
 * @brief   request a snapshot service
 * @param   none
 * @return  none
 */
void req_snapshot_srv (int fs)
{
	set_snapsrv_fs(fs);
	snap_srv(&snapsrv_opt);
}

static struct auplaysrv_opt_t
auplaysrv_opt = {
	NORFS,
	0
};

int set_auplaysrv_fs (int fs)
{
	int rc = 0;

	if(fs == NORFS) {
		auplaysrv_opt.dev = NORFS;
	} else if (fs == FATFS) {
		auplaysrv_opt.dev = FATFS;
	} else {
		ERROR("unknown of file system!\n");
		rc = -1;
	}

	return rc;
}
/**
 * @func    req_auplay_srv
 * @brief   request a audio play service
 * @param   none
 * @return  none
 */
void req_auplay_srv (int fs)
{
	set_auplaysrv_fs(fs);
	auplay_srv(&auplaysrv_opt);
}

static struct aumsgsrv_opt_t
aumsgsrv_opt = {
	NORFS
};

int set_aumsgsrv_fs (int fs)
{
	int rc = 0;

	if(fs == NORFS) {
		aumsgsrv_opt.dev = NORFS;
	} else if (fs == FATFS) {
		aumsgsrv_opt.dev = FATFS;
	} else {
		ERROR("unknown of file system!\n");
		rc = -1;
	}

	return rc;
}
/**
 * @func    req_aumsg_srv
 * @brief   request a record audio service
 * @param   none
 * @return  none
 */
void req_aumsg_srv (int fs)
{
	set_aumsgsrv_fs(fs);
	aumsg_srv(&aumsgsrv_opt);
}

/**
 * @func    req_slide_srv
 * @brief   request a slideshow service
 * @param   none
 * @return  none
 */
void req_slide_srv (void)
{
	slide_srv();
}

static struct md_opt_t
md_opt ={
	720,
	240,
	0, 0, 0, 0,
	0, 0, 720, 240,
	NORFS,
	PIC_LS
};
int set_md_opt_fs (int fs)
{
	int rc = 0;

	if(fs == NORFS) {
		md_opt.dev = NORFS;
	} else if (fs == FATFS) {
		md_opt.dev = FATFS;
	} else {
		ERROR("unknown of file system!\n");
		rc = -1;
	}

	return rc;
}
/**
 * @func    req_md_srv
 * @brief   request a slideshow service
 * @param   none
 * @return  none
 */
void req_md_srv (int x, int y, int w, int h, int fs)
{
	md_opt.mask_x = x;
	md_opt.mask_y = y;
	md_opt.mask_w = w;
	md_opt.mask_h = h;
	set_md_opt_fs(fs);

	md_srv(&md_opt);
}

static struct lsfsrv_opt_t
lsfsrv_opt = {
	PIC_LS,
	NORFS,
	0
};

int set_lsfsrv_fs (int fs)
{
	int rc = 0;

	if(fs == NORFS) {
		lsfsrv_opt.dev = NORFS;
	} else if (fs == FATFS) {
		lsfsrv_opt.dev = FATFS;
	} else {
		ERROR("unknown of file system!\n");
		rc = -1;
	}

	return rc;
}
/**
 * @func    req_picture_lsf
 * @brief   request a picture lsf service
 * @param   fs	file system
 * @return  none
 */
void req_picture_lsf (int fs)
{
	lsfsrv_opt.mark = 0;
	set_lsfsrv_fs(fs);
	lsfsrv_opt.media = PIC_LS;
	lsf_srv(&lsfsrv_opt);

//	pviewsrv_opt.dev = lsfsrv_opt.dev;
//	pviewsrv_opt.mark = lsfsrv_opt.mark;
}

/**
 * @func    req_aumsg_lsf
 * @brief   request a audio lsf service
 * @param   fs	file system
 * @return  none
 */
void req_aumsg_lsf (int fs)
{
	lsfsrv_opt.mark = 0;
	set_lsfsrv_fs(fs);
	lsfsrv_opt.media = AUMSG_LS;
	lsf_srv(&lsfsrv_opt);

//	auplaysrv_opt.dev = lsfsrv_opt.dev;
//	auplaysrv_opt.mark = lsfsrv_opt.mark;
}

/**
 * @func    req_video_lsf
 * @brief   request a video lsf service
 * @param   fs	file system
 * @return  none
 */
void req_video_lsf (void)
{
	int fs = FATFS;
	lsfsrv_opt.mark = 0;
	set_lsfsrv_fs(fs);
	lsfsrv_opt.media = VIDEO_LS;
	lsf_srv(&lsfsrv_opt);

//	playsrv_opt.dev = lsfsrv_opt.dev;
//	playsrv_opt.mark = lsfsrv_opt.mark;
}

/**
 * @func    req_exit_sys
 * @brief   request exit system
 * @param   none
 * @return  none
 */
void req_exit_sys (void)
{
	int id = get_cur_page_id();

	if(id > 0) {
		put_quit_page_event(id);
		dbg(0, "Quit page ID:%d\n", id);
	} else {
		WARN("No active page so unknow quit where!\n");
	}
}
