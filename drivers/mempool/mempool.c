/**
 *  @file   mempool.c
 *  @brief  memory pool driver
 *  $Id: mempool.c,v 1.1.1.1 2013/12/18 03:43:53 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/09/04  Hugo    New file.
 *
 */
#include <stdio.h>
#include <string.h>
#include <mempool.h>
#include <debug.h>

int
mp_create (struct mempool_desc *mpd, void *buf, uint8_t nmemb, int size)
{
    if (nmemb > CONFIG_MEMPOOL_MAX_ELEMENT)
        return -1;

    memset (mpd, 0, sizeof (struct mempool_desc));
    mpd->mem_base = buf;
    mpd->elements = nmemb;
    mpd->groups = (nmemb+7)/8;
    mpd->size = size;

    return 0;
}

void *
mp_alloc (struct mempool_desc *mpd)
{
    uint8_t i, j, bits;

    if (mpd->elements == mpd->used)
        return NULL;

    for (i = 0; i < mpd->groups; i++) {
        bits = mpd->maps[i];
        if (bits == 0xff)
            continue;

        for (j = 0; j < 8; j++) {
            if ((bits & (1 << j)) == 0) {
                mpd->maps[i] = bits | (1 << j);
                mpd->used++;
                return mpd->mem_base + (mpd->size * (i*8 + j));
            }
        }
    }

    /* never reach here */
    assert (0);
    return NULL;
}

void
mp_free (struct mempool_desc *mpd, void *ptr)
{
    uint8_t bits;
    int offset, index;

    assert (mpd->used > 0);

    offset = ptr - mpd->mem_base;
    assert ((offset % mpd->size) == 0);

    index = offset / mpd->size;
    bits = mpd->maps[index/8];
    assert ((bits & (1 << (index%8))) != 0);

    mpd->maps[index/8] = bits & ~(1 << (index%8));
    mpd->used--;
}

#if 0
#define BUF_LEN 1024
#define BUF_NUM 32
int
main (void)
{
    struct mempool_desc mp[1];
    char mem[BUF_LEN * BUF_NUM];
    void *buf[BUF_NUM];
    int i, loop = 10;
    int rc;

    rc = mp_create (mp, mem, BUF_NUM, BUF_LEN);
    assert (rc == 0);

    while (loop--) {
        for (i = 0; i < BUF_NUM; i++) {
            buf[i] = mp_alloc (mp);
            assert ((int)(buf[i] - mp->mem_base) == (i * BUF_LEN));
        }
        assert (mp->used == BUF_NUM);

        for (i = 0; i < BUF_NUM; i++) {
            mp_free (mp, buf[i]);
        }
        assert (mp->used == 0);
    }

    return 0;
}
#endif
