/**
 *  @file   sh_ov_dump.c
 *  @brief  dump all registers of OmniVision camera chip
 *  $Id: sh_ov_dump.c,v 1.1.1.1 2013/12/18 03:43:33 jedy Exp $
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

command_init (sh_ov_dump, "ov_dump", "ov_dump");

static int
sh_ov_dump (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;

    ov_dump (pov);

    return 0;
}
