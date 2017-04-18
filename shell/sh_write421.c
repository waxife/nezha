/**
 *  @file   sh_write421.c
 *  @brief  system init entry
 *  $Id: sh_write421.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
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

command_init (sh_write4, "write4", "write4 <addr> <val>");
command_init (sh_write2, "write2", "write2 <addr> <val>");
command_init (sh_write1, "write1", "write1 <addr> <val>");


static int sh_write4 (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    unsigned int val;

	if (argc < 3) 
        goto EXIT;
	

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;

    rc = get_val(argv[2], (int *)&val);
    if (rc < 0)
        goto EXIT;

    rc = check_addr_aligned(addr, 0, 4);
    if (rc < 0)
        goto EXIT;


    writel(val, addr);
    printf("%p = %xh (%d)\n", addr, val, val);


    return 0;

EXIT:
    print_usage (sh_write4);
    return -1;
}

static int sh_write2 (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    unsigned int val;

	if (argc < 3) 
        goto EXIT;
	

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;

    rc = get_val(argv[2], (int *)&val);
    if (rc < 0)
        goto EXIT;

    rc = check_addr_aligned(addr, 0, 2);
    if (rc < 0)
        goto EXIT;


    writew(val, addr);
    printf("%p = %xh (%d)\n", addr, val, val);


    return 0;

EXIT:
    print_usage (sh_write2);
    return -1;
}

static int sh_write1 (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    unsigned int val;

	if (argc < 3) 
        goto EXIT;
	

    rc = get_addr(argv[1], &addr);
    if (rc < 0) 
        goto EXIT;

    rc = get_val(argv[2], (int *)&val);
    if (rc < 0)
        goto EXIT;


    writeb(val, addr);
    printf("%p = %xh (%d)\n", addr, val, val);

    return 0;

EXIT:
    print_usage (sh_write1);
    return -1;
}

