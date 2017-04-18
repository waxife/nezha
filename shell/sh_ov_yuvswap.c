/**
 *  @file   sh_ov_yuvswap.c
 *  @brief  get/set YUV output sequence of OmniVision camera chip
 *  $Id: sh_ov_yuvswap.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/06  hugo    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <debug.h>

#include <drivers/ov/ov.h>

static const char *mode[] = { "YUYV", "YVYU", "UYVY", "VYUY" };

command_init (sh_ov_yuvswap, "ov_yuvswap", "ov_yuvswap [<val>:0~3]");

static int
sh_ov_yuvswap (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;
    unsigned char val, r3a, r3d;
    int rc;

    if (argc > 1) {
        val = atoi (argv[1]);
        r3a = ((val & (1 << 1)) != 0) << 3;
        r3d = ((val & (1 << 0)) != 0) << 0;

        /* R3a bit[3] */
        rc = ov_set_reg_mask (pov, 0x3a, r3a, (1 << 3));
        if (rc < 0) {
            ERROR ("ov_set_reg_mask(%02X,%02X,%02X)\n", 0x3a, r3a, (1 << 3));
            goto EXIT;
        }

        /* R3d bit[0] */
        rc = ov_set_reg_mask (pov, 0x3d, r3d, (1 << 0));
        if (rc < 0) {
            ERROR ("ov_set_reg_mask(%02X,%02X,%02X)\n", 0x3d, r3d, (1 << 0));
            goto EXIT;
        }
    }

    /* R3a bit[3] */
    rc = ov_get_reg (pov, 0x3a, &r3a);
    if (rc < 0) {
        ERROR ("ov_get_reg(%02X)\n", 0x3a);
        goto EXIT;
    }

    /* R3d bit[0] */
    rc = ov_get_reg (pov, 0x3d, &r3d);
    if (rc < 0) {
        ERROR ("ov_get_reg(%02X)\n", 0x3d);
        goto EXIT;
    }

    /* show yuvswap no */
    val = (((r3a & (1 << 3)) != 0) << 1) | (((r3d & (1 << 0)) != 0) << 0);
    printf ("mode=%d (%s)\n", val, mode[val]);

    return 0;
  EXIT:
    print_usage (sh_ov_yuvswap);
    return -1;
}
