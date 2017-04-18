/**
 *  @file   mdvid.h
 *  @brief	header file for motion detect process
 *  $Id: mdvid.h,v 1.4 2015/08/06 01:36:28 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2014/01/29  ken New file.
 *
 */
#ifndef _MDVID_H_
#define _MDVID_H_
#include <sys.h>
#include <callback.h>
#define MD_OS_MAGIC		0x12344321

struct md_opt_t{
	int width;
	int height;
	int mt_x;
	int mt_y;
	int mt_w;
	int mt_h;
    int mask_x;
	int mask_y;
	int mask_w;
	int mask_h;
	int dev;
	int media;
    int sensitive;
    int detect_channels;
    int md_channel;
};

enum {
    SEN_HIGH     = 0,
    SEN_MEDIAN   = 4,  
    SEN_LOW      = 8,  
};

/**
 * @func    mdvid
 * @brief   motion detect in video.
 * @param   md_opt_t       	motion detect option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int mdvid (struct md_opt_t *opt, cmd_callback_t cmd_callback);
/**
 * @func    md_status
 * @brief   get motion detect status.
 * @param   status          status data structure is declared in sys/include/sys.h
 * @return  0 success, or return negative error code.
 */
int md_status(struct sysc_status_t *status);

#endif /* _MDVID_H_ */
