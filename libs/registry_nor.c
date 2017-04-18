/*
 *  @file   registry_nor.c
 *  @brief  registry for nor
 *  $Id $
 *  $Author $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2009 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/16  New file.
 *
 */
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <debug.h>
#include <errno.h>
#include <unistd.h>
#include <registry.h>
#include <nor.h>
#include <config.h>
#include <sys.h>
#include <cache.h>



int nor_read(struct registry_t *reg, char *buf, int len, int pu, int off)
{   
    int i;
    
    /* read back */
    memset(buf, 0, len);
    for (i = 0; i < len; i++) {
        buf[i] = readb((pu|NOR_DATA_BASE) + off +i);   
    }
    
    return len;
}


int nor_write(struct registry_t *reg, char *buf, int len, int pu, int off)
{
    int rc = 0;  
    //int i;
    //char rbuf[80*4];
    
    if(off == 0) {
        rc = nor_block_erase(pu);
        assert(rc == 0);
    }
    nor_data_write(pu + off, len, (char *)buf);
    

//dbg(0, "dump from NOR: \n");
/*
    memset(rbuf, 0, 80*4);
    for (i = 0; i < 80*4; i++) {
        rbuf[i] = readb((pu|NOR_DATA_BASE) + i);   
    }
    memdump(rbuf, 80*4);
*/
    return len;
}

