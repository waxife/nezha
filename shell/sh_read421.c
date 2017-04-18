/**
 *  @file   sh_read421.c
 *  @brief  system init entry
 *  $Id: sh_read421.c,v 1.1.1.1 2013/12/18 03:43:40 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <shell.h>
#include <sys.h>

command_init (sh_read4, "read4", "read4 <addr>");
command_init (sh_read2, "read2", "read2 <addr>");
command_init (sh_read1, "read1", "read1 <addr>");


static int sh_read4 (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    unsigned val;

	if (argc < 2) 
        goto EXIT;
	

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;

    rc = check_addr_aligned(addr, 0, 4);
    if (rc < 0)
        goto EXIT;

    val = readl(addr);
    printf("%p = %08xh (%d)\n", addr, val, val);


    return 0;

EXIT:
    print_usage (sh_read4);
    return -1;
}

static int sh_read2 (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    unsigned val;

	if (argc < 2) 
        goto EXIT;
	

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;

    rc = check_addr_aligned(addr, 0, 2);
    if (rc < 0)
        goto EXIT;


    val = readw(addr);
    printf("%p = %04xh (%d)\n", addr, val, val);


    return 0;

EXIT:
    print_usage (sh_read2);
    return -1;
}

static int sh_read1 (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    unsigned val;

	if (argc < 2) 
        goto EXIT;
	

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;


    val = readb(addr);
    printf("%p = %02xh (%d)\n", addr, val, val);


    return 0;

EXIT:
    print_usage (sh_read1);
    return -1;
}

