/**
 *  @file   norfs.deprecated.c
 *  @brief  NOR Filesystem deprecated API
 *  $Id: norfs.deprecated.c,v 1.2 2014/02/11 11:59:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/02/07  hugo    New file.
 *
 */
#include <string.h>
#include <norfs.h>
#include <nor.h>
#include <time.h>
#include <sys.h>
#include <mconfig.h>
#include <config.h>
#include <debug.h>

int norfs_forward (struct norfs_dir *dir)
{
    return 0;
}

int norfs_backward (struct norfs_dir *dir)
{
    return 0;
}

void norfs_pos2head (void)
{
}

void norfs_pos2tail (void)
{
}
