/**
 *  @file   sh_ov_autoexp.c
 *  @brief  get/set auto exposure of OmniVision camera chip
 *  $Id: sh_ov_autoexp.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
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

command_init (sh_ov_autoexp, "ov_autoexp", "ov_autoexp [<val>]");

static int
sh_ov_autoexp (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;
    unsigned char val;
    int rc;

    if (argc > 1) {
        val = atoi (argv[1]);
        rc = ov_set_autoexp (pov, val);
        if (rc < 0) {
            ERROR ("ov_set_autoexp(%d)\n", val);
            goto EXIT;
        }
    }

    val = 0x00;
    rc = ov_get_autoexp (pov, &val);
    if (rc < 0) {
        ERROR ("ov_get_autoexp()\n");
        goto EXIT;
    }
    printf ("autoexp: %d\n", val);

    return 0;
  EXIT:
    print_usage (sh_ov_autoexp);
    return -1;
}
