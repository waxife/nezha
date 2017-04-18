/**
 *  @file   req_service.c
 *  @brief
 *  $Id: req_service.c,v 1.3 2016/07/27 10:00:02 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.3 $
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


#include "md_service.h"

#include "mdvid.h"

#include "req_service.h"


static struct md_opt_t
md_opt ={
	720,
	240,
	0, 0, 0, 0,
	0, 0, 720, 240,
	NORFS,
	PIC_LS,
    SEN_MEDIAN,
    1               // =3 will auto switch 3 CVBS channels.
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
