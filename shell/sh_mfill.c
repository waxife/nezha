/**
 *  @file   sh_mfill.c
 *  @brief  system init entry
 *  $Id: sh_mfill.c,v 1.1.1.1 2013/12/18 03:43:23 jedy Exp $
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
#include <stdlib.h>


command_init (sh_mfill, "mfill", "mfill <addr-len> <val>[-1 | -2 | -4] [c | + | r]");

static int sh_mfill (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *addr;
    void *eaddr;
    int len, val, width;
    int mval = 'c';

	if (argc < 3) 
        goto EXIT;

    rc = get_addr_len(argv[1], &addr, &len);
    if (rc < 0) 
        goto EXIT;
    
    rc = get_val_width(argv[2], &val, &width);
    if (rc < 0) 
        goto EXIT;
        
    if (argc >= 4) {
    	mval = argv[3][0];
    }

    /* fill memory */
    if (mval == '+') { /* increament */
    	if (width == 1) {
    		unsigned char *p;
    		unsigned char c = (unsigned char) val;
	        eaddr = addr + len;
	        for (p = (unsigned char *)addr; (void *)p < eaddr; p++) 
	            *p = c++;
    	} else if (width == 2) {
	        unsigned short *p;
    		unsigned short c = (unsigned short) val;
	        len = len & (-2);
	        eaddr = addr + len;
	        for (p = (unsigned short *)addr; (void *)p < eaddr; p++) 
	            *p = c++;
    	}  else if (width == 4) {
	        unsigned int *p;
			unsigned int c = (unsigned int) val;
	        len = len & (-4);
	        eaddr = addr + len;        
	        for (p = (unsigned int *)addr; (void *)p < eaddr; p++) 
	            *p = c++;
	    }
    		
	} else if (mval == 'r') { /* randam */
		unsigned char *p;
        eaddr = addr + len;
        for (p = (unsigned char *)addr; (void *)p < eaddr; p++) 
            *p = rand();
	} else { /* constant */
	    if (width == 1) {
	        memset(addr, val, len);
	    } else if (width == 2) {
	        unsigned short *p;
	        len = len & (-2);
	        eaddr = addr + len;
	
	        for (p = (unsigned short *)addr; (void *)p < eaddr; p++) 
	            *p = val;
	    } else if (width == 4) {
	        unsigned int *p;
	        len = len & (-4);
	        eaddr = addr + len;        
	
	        for (p = (unsigned int *)addr; (void *)p < eaddr; p++) 
	            *p = val;
	    }
	}
	
//    dma_cache_wback_inv((unsigned long)addr, len);
    flush_dcache_all();
    
    printf("%d bytes filled\n", len);

    return 0;

EXIT:
    print_usage (sh_mfill);
    return -1;
}

