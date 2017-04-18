/**
 *  @file   sh_ov_init.c
 *  @brief  init OmniVision camera chip
 *  $Id: sh_ov_init.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/01/13  hugo    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <debug.h>
#include <sys.h>

#include <drivers/ov/ov.h>

command_init (sh_ov_init, "ov_init", "ov_init");

static int
sh_ov_init (int argc, char **argv, void **result, void *sys_ctx)
{
    struct ovc *pov = ((struct sys_ctx_t *) sys_ctx)->pov;

    ov_init (pov);
    return 0;
}
