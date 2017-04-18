/**
 *  @file   sh_ov_id.c
 *  @brief  read IDs of OmniVision camera chip
 *  $Id: sh_ov_id.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
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

command_init (sh_ov_id, "ov_id", "ov_id [<cnt>]");

static int
sh_ov_id (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;
    unsigned char midh, midl, pid, ver;
    int total = 1, pass = 0, i, rc;

    if (argc > 1)
        total = atoi (argv[1]);

    for (i = 0; i < total; i++) {
        /* read OV manufacturer id - R1C=0x7f, R1D=0xa2 */
        rc = ov_get_reg (pov, 0x1C, &midh);
        if (rc < 0) {
            ERROR ("ov_get_reg(0x1C)\n");
            goto EXIT;
        }

        rc = ov_get_reg (pov, 0x1D, &midl);
        if (rc < 0) {
            ERROR ("ov_get_reg(0x1D)\n");
            goto EXIT;
        }

        /* read OV product id - R0A=0x76, R0B=0x73 */
        rc = ov_get_reg (pov, 0x0A, &pid);
        if (rc < 0) {
            ERROR ("ov_get_reg(0x0A)\n");
            goto EXIT;
        }

        rc = ov_get_reg (pov, 0x0B, &ver);
        if (rc < 0) {
            ERROR ("ov_get_reg(0x0B)\n");
            goto EXIT;
        }

        /* check IDs */
        if (midh != 0x7f || pid != 0x76 || ver != 0x73)
            DBG_PRINT ("#%-6d %02x %02x %02x %02x\n", i, midh, midl, pid, ver);
        else
            pass++;
    }

    DBG_PRINT ("pass=%d/%d\n", pass, total);

    return 0;
  EXIT:
    print_usage (sh_ov_id);
    return -1;
}
