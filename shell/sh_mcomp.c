/**
 *  @file   sh_mcomp.c
 *  @brief  system init entry
 *  $Id: sh_mcomp.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
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
#include <shell.h>
#include <cache.h>

command_init (sh_mcomp, "mcomp", "mcomp <addr-len> <addr2>");

static int sh_mcomp (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    unsigned char *addr, *addr2;
    unsigned char *eaddr;
    unsigned char *p, *q;
    int len;

	if (argc < 3) 
        goto EXIT;
	

    rc = get_addr_len(argv[1], &addr, &len);
    if (rc < 0) 
        goto EXIT;
    
    
    rc = get_addr(argv[2], &addr2);
    if (rc < 0) 
        goto EXIT;

    eaddr= addr + len;
    for(p = addr, q = addr2; p < eaddr; p++, q++) {
        if (*p != *q) {
            printf("addr %p (%02x) is not same as %p (%02x)\n", p, *p, q, *q);
            break; 
        }
    }

    if (p == eaddr) {
        printf("%p-%d is totally equal to %p\n", addr, len, addr2);
    }


    return 0;

EXIT:
    print_usage (sh_mcomp);
    return -1;
}




