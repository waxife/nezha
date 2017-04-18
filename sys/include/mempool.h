/**
 *  @file   mempool.h
 *  @brief  Memory pool buffer management
 *  $Id: mempool.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2013/09/04  hugo    New file.
 *
 */
#ifndef __MEMPOOL_H
#define __MEMPOOL_H

#include <stdint.h>
#include <config.h>

struct mempool_desc {
    void *mem_base;
    uint8_t elements;
    uint8_t groups;
    uint8_t used;
    int size;
    unsigned char maps[(CONFIG_MEMPOOL_MAX_ELEMENT+7)/8];
};

int mp_create (struct mempool_desc *mpd, void *buf, uint8_t nmemb, int size);
void *mp_alloc (struct mempool_desc *mpd);
void mp_free (struct mempool_desc *mpd, void *ptr);

#endif /* __MEMPOOL_H */
