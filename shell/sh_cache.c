/**
 *  @file   sh_cache.c
 *  @brief  system init entry
 *  $Id: sh_cache.c,v 1.2 2014/07/09 05:32:38 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
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
#include <mipsregs.h>
#include <io.h>
#include <sys.h>

command_init (sh_cache, "$cache", "-$cache [on | off]");
command_init (sh_cache_inv, "$inv", "-$inv addr-len");

static int sh_cache(int argc, char **argv, void **result, void *sys_ctx)
{
    if (argc < 2) 
        goto EXIT;

    if (strcasecmp(argv[1], "on") == 0) {
        change_c0_config32(0x7, 1);
    } else if (strcasecmp(argv[1], "off") == 0) {
        change_c0_config32(0x7, 2);
    } else {
        goto EXIT;
    }

    return 0;

EXIT:
    print_usage(sh_cache);
    return -1;
}

static int sh_cache_inv(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr; 
    int len;

    if (argc < 2)
        goto EXIT;

    rc = get_addr_len(argv[1], &addr, &len);
    if (rc < 0)
        goto EXIT;

    printf("addr %p len %d\n", addr, len);

    dma_cache_inv((unsigned long)addr, len);

    return 0;
EXIT:
    print_usage(sh_cache);
    return -1;            
}
