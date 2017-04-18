/**
 *  @file   sh_ov_pattern.c
 *  @brief  get/set test pattern of OmniVision camera chip
 *  $Id: sh_ov_pattern.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
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

command_init (sh_ov_pattern, "ov_pattern", "ov_pattern [<val>:0~3]");

static int
sh_ov_pattern (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;
    unsigned char val, r70, r71;
    int rc;

    if (argc > 1) {
        val = atoi (argv[1]);
        r70 = (val & 0x1) << 7;
        r71 = (val & 0x2) << 6;

        /* R70 bit[7] */
        rc = ov_set_reg_mask (pov, 0x70, r70, 0x80);
        if (rc < 0) {
            ERROR ("ov_set_reg_mask(%02X,%02X,%02X)\n", 0x70, r70, 0x80);
            goto EXIT;
        }

        /* R71 bit[7] */
        rc = ov_set_reg_mask (pov, 0x71, r71, 0x80);
        if (rc < 0) {
            ERROR ("ov_set_reg_mask(%02X,%02X,%02X)\n", 0x71, r71, 0x80);
            goto EXIT;
        }
    }

    /* R70 bit[7] */
    rc = ov_get_reg (pov, 0x70, &r70);
    if (rc < 0) {
        ERROR ("ov_get_reg(%02X)\n", 0x70);
        goto EXIT;
    }

    /* R71 bit[7] */
    rc = ov_get_reg (pov, 0x71, &r71);
    if (rc < 0) {
        ERROR ("ov_get_reg(%02X)\n", 0x71);
        goto EXIT;
    }

    /* show pattern no */
    printf ("pattern: %d\n", ((r70 & 0x80) >> 7) | ((r71 & 0x80) >> 6));

    return 0;
  EXIT:
    print_usage (sh_ov_pattern);
    return -1;
}
