/**
 *  @file   photo.c
 *  @brief  photo encode/decode driver
 *  $Id: photo.c,v 1.2 2014/01/08 03:26:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/10/02  hugo     New file.
 *
 */

#include <config.h>
#include <debug.h>
#include <string.h>
#include <photo.h>
#include <io.h>
#include <jreg.h>

#define SET_JCODEC_IMG_INFO_0_REG(H_REAL, H, W) writel((((H_REAL) &0x3ff) << 20) | (((H)&0x3ff) << 10) | (((W) & 0x3ff) << 0), JCODEC_IMG_INFO_0_REG)
#define SET_JCODEC_IMG_INFO_1_REG(H,W) writel((H)*(W)/32 - 1, JCODEC_IMG_INFO_1_REG)


int
jcodec_reset ()
{
	return 0;
}

int
jcodec_config ()
{
#if 0
    /* set jpeg decode reg */
    SET_JCODEC_IMG_INFO_0_REG (dec->img_h_real, dec->img_h, dec->img_w);
    SET_JCODEC_IMG_INFO_1_REG (dec->img_h, dec->img_w);
    SET_JCODEC_DESP_CONF_REG ();
    writel (JC_CMD_DEC, JCODEC_CMD_REG);

    /* set pre-play timing reg */

    /* setup quantization table */
    dec_set_q_table (quality);
#endif
    return 0;

}

int
photo_open (struct photo_ctx *ph, int mode)
{
    assert (mode == PH_MODE_WRONLY || mode == PH_MODE_RDONLY);

    memset (ph, 0, sizeof (struct photo_ctx));

    /* header */

    /* init codec */
    jcodec_reset ();
    jcodec_config ();

    /* start codec */

    return 0;
}

int
photo_close (struct photo_ctx *ph)
{
    /* stop codec */
    writel (-1, JCODEC_INT_MASK_REG);
    return 0;
}

int
photo_read_nonblock (struct photo_ctx *ph, void *buffer, int length)
{
    return 0;
}

int
photo_read (struct photo_ctx *ph, void *buffer, int length)
{
    return 0;
}

int
photo_write_nonblock (struct photo_ctx *ph, void *buffer, int length)
{
    return 0;
}

int
photo_write (struct photo_ctx *ph, void *buffer, int length)
{
    return 0;
}

int
photo_ioctl (struct photo_ctx *ph, int request, void *arg)
{
    return 0;
}
