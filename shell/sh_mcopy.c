/**
 *  @file   sh_mcopy.c
 *  @brief  system init entry
 *  $Id: sh_mcopy.c,v 1.1.1.1 2013/12/18 03:43:38 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>

command_init (sh_mcopy, "mcopy", "mcopy <addr-len> <addr2>");

static int sh_mcopy (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr, *addr2;
    int len;

	if (argc < 3) 
        goto EXIT;

    rc = get_addr_len(argv[1], &addr, &len);
    if (rc < 0)
        goto EXIT;
    
    rc = get_addr(argv[2], &addr2);
    if (rc < 0) 
        goto EXIT;
    
    memcpy(addr2, addr, len);

    return 0;

EXIT:
    print_usage (sh_mcopy);
    return -1;
}




