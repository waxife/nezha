/**
 *  @file   sh_md5.c
 *  @brief  system init entry
 *  $Id: sh_md5.c,v 1.1.1.1 2013/12/18 03:43:37 jedy Exp $
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
#include <sys.h>
#include <io.h>
#include "md5.h"


command_init (sh_md5, "md5", "md5 <addr-len>");

static int sh_md5 (int argc, char **argv, void **result, void *sys_ctx)
{
    md5_state_t state;
    md5_byte_t digest[16];
    unsigned char *addr;
    int len;
    int rc;
    int di;

	if (argc < 2) 
        goto EXIT;

    rc = get_addr_len(argv[1], &addr, &len);
    if (rc < 0) 
        goto EXIT;
   
    md5_init(&state);
    md5_append(&state, (const md5_byte_t *)addr, len);
    md5_finish(&state, digest);
    
    printf("addr %p %d bytes calculate\n", addr, len);
    printf("MD5 sum = ");
    for (di = 0; di < 16; di++)
        printf("%02x", digest[di]);
    printf("\n");
        
    return 0;

EXIT:
    print_usage(sh_md5);
    return -1;
}
