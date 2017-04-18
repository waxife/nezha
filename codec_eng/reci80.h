/**
 *  @file   reci80.h
 *  @brief
 *  $Id: reci80.h,v 1.1 2015/04/16 03:43:29 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2014/7/10  Ken 	New file.
 *
 */

#ifndef _RECI80_H_
#define _RECI80_H_

#include <sys.h>

struct reci80_opt_t {
    int width;          /* video frame width */
    int height;         /* vide frame height */
    int8_t standard;    /* frame rate */
    int8_t quality;     /* encode quality, 0 ~ 100 */
    int8_t fps_ratio;   /* 1 / 2^fps_ratio frame rate. only support 0 , 1 (1/2 fps), 2 (1/4 fps), and 3 (1/8 fps) */
    int8_t en_aud;   	/* enable audio */
};

/**
 * @func    reci80
 * @brief   video streamer by i80 interface.
 * @param   reci80_opt_t       record option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int reci80(struct reci80_opt_t *rec_opt, int (*cmd_callback)(void **arg));

#endif	/* _RECI80_H_ */
