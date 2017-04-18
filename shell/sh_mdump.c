/**
 *  @file   sh_mdump.c
 *  @brief  system init entry
 *  $Id: sh_mdump.c,v 1.1.1.1 2013/12/18 03:43:37 jedy Exp $
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
#include <cache.h>
#include <ctype.h>
#include <io.h>

command_init (sh_mdump, "mdump", "mdump [ <addr-len> | <addr> ]; mdump");

static int sh_mdump (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    unsigned char *addr;
    // unsigned char *p, *q, *eaddr;
    int len;
    // int i;
    static unsigned char *laddr = (unsigned char *)KSEG0;
    static int llen;

	if (argc >= 2) {
        if (strchr(argv[1], '+') || strchr(argv[1], '-')) {
      	    rc = get_addr_len(argv[1], &addr, &len);
      	} else {
            rc = get_addr(argv[1], &addr);
            len = 32;
        }
        
        if (rc < 0)
            goto EXIT;

    } else {
        addr = laddr;
        len = llen;
    }
    
    llen = len;
    laddr = addr + len;
    
    /* flush cache */
    dma_cache_wback_inv((unsigned long)addr, len);

#if 1
    memdump((void *)addr, len);
#else
    /* dump memory */
    p = addr;
    eaddr = addr + (len & (-16));
    for (; p < eaddr;) {
        q = p;
        printf("%p  ", p);
        for (i = 0; i < 16; i++, p++) {
            printf("%02x ", *p);
        }
        printf("| ");
        for (i = 0; i < 16; i++, q++) {
            if (isprint(*q) && (*q > ' '))
                putchar(*q);
            else
                putchar('.');
        }
        putchar('\n');
    }

    if (len & (16-1)) {

        printf("%p  ", p);

        q = p;
        eaddr = addr + len;
        for (i = 0; i < 16; i++, p++) {
            if (p < eaddr) 
                printf("%02x ", *p);
            else
                printf("   ");
        }
    
        printf("| ");
        for (; q < eaddr; q++) {
            if (isprint(*q)) 
                putchar(*q);
            else
                putchar('.');
        }
        putchar('\n');
    }        

    putchar('\n');
#endif

    return 0;

EXIT:
    print_usage (sh_mdump);
    return -1;
}




