/**
 *  @file   sh_spi_cmd.c
 *  @brief  test spi command rdsr wrsr function
 *  $Id: sh_spi_cmd.c,v 1.2 2014/01/17 06:15:42 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/04/  ycshih    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <cache.h>
#include <nor.h>
#include <debug.h>

command_init (sh_spi_cmd, "spi_cmd", "spi_cmd <idx>");
command_init (sh_spi_cmd_test, "spi_cmd_test", "spi_cmd_test <loops>");

static int spi_cmd(int idx)
{
	int state;
    switch(idx)
    {
        case 0:
            printf("spi wren\n");

            if(nor_wren() < 0)
				goto EXIT;
            printf("spi wren successful\n");
            break;
        case 1:
            printf("spi rdsr\n");
            if(nor_rdsr(&state) < 0)
				goto EXIT;
            printf("spi rdsr value= %x\n",state);
            break;
        case 2:
            printf("spi wrsr unprotect\n");
            if(nor_wrsr(0) < 0)
				goto EXIT;
            printf("spi wrsr unprotect finish\n");
            break;
		case 3:
            printf("spi wrsr protect\n");
            if(nor_wrsr(0x3c) < 0)
				goto EXIT;
            printf("spi wrsr protect finish\n");
            break;
        default:
            printf("unsupport function\n");
            break;
    }
    return 0;
EXIT:
	printf("CMD ERROR\n");
	return 0;
}

static int sh_spi_cmd_test(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    int loops;
    int i;

    rc = get_val(argv[1], &loops);
    if(loops < 1)
        goto EXIT;

    for(i = 0; i < loops; i++)
    {
        printf("******** loop %d ************\n", i);
        spi_cmd(0);
        spi_cmd(3);
        spi_cmd(1);
        spi_cmd(3);
        printf("******** done %d**************\n", i);
    }
    return 0;

EXIT:
    print_usage(sh_spi_cmd_test);
    return -1;

}

static int sh_spi_cmd(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc, idx;
    
    if (argc < 2)
        goto EXIT;

    rc = get_val(argv[1], &idx);
    if(rc < 0)
        goto EXIT;

    spi_cmd(idx);

    if(rc == -1)
        return -1;

    return 0;
EXIT:
    print_usage(sh_spi_cmd);
    printf("idx = 0: write enable spi nor flash\n");
    printf("idx = 1: read  status spi nor flash\n");
    printf("idx = 2: unprotected block spi nor flash\n");
	printf("idx = 3: protected block spi nor flash\n");
    return -1;
}
