/**
 *  @file   heap.c
 *  @brief  heap memory management functions
 *  $Id: heap.c,v 1.2 2014/03/13 10:31:35 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/3/13  New file.
 *
 */

#include <debug.h>
#include <heap.h>

#define MAX_HEAP_ITEMS  4

static struct heap_t {
    char    *pbuf;
    int     rsize:24;
    int     idx:4;
    const char     *_func[MAX_HEAP_ITEMS];
    unsigned short _lno[MAX_HEAP_ITEMS];
} hp;


int heap_create(char *start, int size)
{
    if (hp.pbuf != NULL) {
        ERROR("Invalid call sequence\n");
        return -1;
    }
    
    if (((unsigned int)start & 0x3) != 0) {
        ERROR("heap start must alignment 4 bytes\n");
        return -1;
    }

    hp.pbuf = start;
    hp.rsize = size;
    hp.idx = 0;

    printf("Create heap %p size %d\n", hp.pbuf, hp.rsize);

    return 0;
}

void heap_debug(void)
{
    int i;
    printf("pbuf = %p size = %d\n", hp.pbuf, hp.rsize);
    for (i = 0; i < hp.idx; i++) {
        printf("[%d] %s:%d alloc heap\n", i, hp._func[i], hp._lno[i]);
    }
}

char *__heap_alloc(const char *func, int lno, int size)
{
    char *pbuf;

    if (hp.pbuf == NULL) {
        ERROR("Invalid call sequence\n");
        return NULL;
    }

    if (hp.idx >= MAX_HEAP_ITEMS) {
        ERROR("too many heap alloction invokation\n");
        heap_debug();
        return NULL;
    }

    size = (size + 3) & -4;     /* alignment 4 */
    if (size > hp.rsize) {
        ERROR("no more memory for allocation %d\n", size);
        heap_debug();
        return NULL;
    }

    hp._func[hp.idx] = func;
    hp._lno[hp.idx] = (unsigned short)lno;
    pbuf = hp.pbuf;
    hp.pbuf = pbuf + size;
    hp.rsize -= size;
    hp.idx++;

    return pbuf;
}

int __heap_release(const char *func, int lno, char *p)
{
    int size;
    extern unsigned int _fheap;

    if (hp.pbuf == NULL) {
        ERROR("invalid call sequence\n");
        return -1;
    }

    if (((unsigned int)p & 0x3) != 0) {
        ERROR("invalid release pointer %p\n", p);
        return -1;
    }

    if (func != hp._func[hp.idx - 1]) {
        ERROR("broken heap management rule\n");
        heap_debug();
        return -1;
    }

    if (p < (char *)&_fheap) {
        ERROR("invalid address\n");
        return -1;
    }
    
    size = hp.pbuf - p;

    if (hp.pbuf <= p) {
        ERROR("invalid release sequence pbuf=%p p=%p size=%d\n", hp.pbuf, p, size);
        heap_debug();
        return -1;
    }
    hp.idx--;
    hp._func[hp.idx] = NULL;
    hp._lno[hp.idx] = 0;
    hp.rsize += size;
    hp.pbuf = p;

    return 0;
}




