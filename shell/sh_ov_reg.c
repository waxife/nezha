/**
 *  @file   sh_ov_reg.c
 *  @brief  read/write register of OmniVision camera chip
 *  $Id: sh_ov_reg.c,v 1.1.1.1 2013/12/18 03:43:23 jedy Exp $
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

command_init (sh_ov_reg, "ov_reg", "ov_reg <reg> [<val>]");

static int
sh_ov_reg (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;
    unsigned char reg, val;
    int i, rc;

    if (argc < 2)
        goto EXIT;

    reg = strtol (argv[1], NULL, 16);

    if (argc > 2) {
        val = strtol (argv[2], NULL, 16);
        rc = ov_set_reg (pov, reg, val);
        if (rc < 0) {
            ERROR ("ov_set_reg(%02X,%02X)\n", reg, val);
            goto EXIT;
        }
    }

    val = 0x00;
    rc = ov_get_reg (pov, reg, &val);
    if (rc < 0) {
        ERROR ("ov_get_reg(%02X)\n", reg);
        goto EXIT;
    }

    /* show register and value */
    printf ("R%02X = 0x%02X (", reg, val);
    for (i = 7; i >= 0; i--)
        printf ("%d", (val & (1 << i)) != 0);
    printf (")\n");

    return 0;
  EXIT:
    print_usage (sh_ov_reg);
    return -1;
}
