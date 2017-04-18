/**
 *  @file   sh_mv_file.c
 *  @brief  shell command of mave fie
 *  $Id: sh_mv_file.c,v 1.3 2014/02/24 10:19:55 onejoe Exp $
 *  $Author: onejoe $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 *
 *  @date   2014/02/24  onejoe     New file.
 *
 */
 
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <keymap.h>
#include <sys.h>
#include <ctype.h>
#define DBG_LEVEL   4
#include <debug.h>

#include <norfs.h>
#include <fat32.h>
#include <mconfig.h>

#include "./drivers/mvfile/mvfile.h"

command_init (sh_copynor2sd, "copynor2sd", "<src type> <norfile name> <sdfile name>");
command_init (sh_copysd2nor, "copysd2nor", "<src type> <sdfile name> <norfile name>");

static int sh_copynor2sd (int argc, char **argv, void **result, void *sys_ctx)
{
	int rc;

	if (argc < 4)
        goto EXIT;

	if (argv[1][0] > 0x33 || argv[1][0] < 0x30 ) {
		goto EXIT;
	}else{
		rc = copynor2sd(argv[1][0] - 0x30, argv[2], argv[3]);
		return 0; 
	}
	
EXIT:
	return -1;	
}

static int sh_copysd2nor (int argc, char **argv, void **result, void *sys_ctx)
{
	int rc;

	if (argc < 4)
        goto EXIT;

	if (argv[1][0] > 0x33 || argv[1][0] < 0x30 ) {
		goto EXIT;
	}else{
		rc = copysd2nor(argv[1][0] - 0x30, argv[2], argv[3]);
		return 0; 
	}
	
EXIT:
	return -1;	
}
