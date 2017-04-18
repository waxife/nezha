/**
 *  @file   mm.c
 *  @brief  POSIX like malloc relative functions
 *  $Id: mm.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <stdio.h>
//#include <stdlib.h>
#include <debug.h>

#define ALIGN4(x)   (((x)+3)&(-4))

#define C2P(c) ((char *)(c) + 4)
#define P2C(p) (struct cell *)((char *)(p) - 4)
#define ISADJ(c1,c2) ((struct cell *)(C2P(c1) + (c1)->size) == (struct cell *)(c2))

struct cell {
    unsigned int size;
    struct cell *next;
};

static struct simple {
    struct cell *tail;
    unsigned int size;
    void        *base;
} s;

#define SANITY_CHECK(s, c)		((((void *)c) >= (void *)(s).base) && (((void *)c) <= (void *)((s).base + (s).size)))
 
void dump_tail()
{
	printf("s.tail->next %p \n", s.tail->next);
}

void heap_info()
{
    struct cell *c1;
    int free = 0;
    printf("** Free Heap --\n");
    printf("Heap size %d \n", s.size);
    c1 = s.tail->next;
   
    do {
        printf("  %p - %p (%xh, %d) %p\n", c1, ((char *)c1) + c1->size + 4, c1->size, c1->size, c1->next);
        free += c1->size;
        c1 = c1->next;
    } while (c1 != s.tail->next);

    printf("mem %p tail %p\n", s.base, s.tail);
    printf("free size %d\n", free);
    printf("allocated %d\n", s.size - free);
    printf("\n");
}


int heap_init(void *heapbase, unsigned int heapsize)
{
    s.base = s.tail = heapbase;
    s.tail->size = heapsize - 4;
    s.tail->next = s.tail;
    s.size = heapsize;

    return 0;
}

void *__malloc (unsigned long size, const char *fname, int lineno)
{
    struct cell *c1, *c2, *c3;

    size = ALIGN4(size);

    c1 = s.tail;
    assert(SANITY_CHECK(s, c1));
    while(c1->next->size < size) {
        if (c1->next == s.tail) {
            printf("Ran out of heap memory (want %ld)\n", size);
            heap_info();
            return NULL;
        }
        c1 = c1->next;
	    assert(SANITY_CHECK(s, c1));
    }

    c2 = c1->next;
    if (c2->size > (4 + size)) { /* split new cell */
        c3 = (struct cell *)(C2P(c2) + size);
        if (c1 == c2) {
            c1 = c3;
        }
        c3->size = c2->size - (size + 4);
        c3->next = c2->next;
        c2->size = size;
        c1->next = c3;
    } else { /* use the entire cell */
        c1->next = c2->next;
    }

    if (c2 == s.tail)
        s.tail = c1;

    return C2P(c2);
   
}

void __free (void *p, const char *fname, int lineno)
{
    struct cell *c1, *c2, *c3;
    int j1, j2;

	if (!SANITY_CHECK(s, p)) {
		dbg(0, "%s:%d invalid address %p for free\n", fname, lineno, p);
		return;
	}

    /* splice the cell back into the list */
    c1 = s.tail;
    c2 = P2C(p);
    assert(SANITY_CHECK(s, c1));
    assert(SANITY_CHECK(s, c2));
    if (c2 > c1) { /* append to end of list */
        if (ISADJ(c1,c2)) { /* join with last cell */
            c1->size += 4 + c2->size;
            return;
        }
        c2->next = c1->next;
        c1->next = c2;
        s.tail = c2;
        return;
    }

    while (c1->next < c2) { /* find insertion point */
        c1 = c1->next;
    }
    c3 = c1->next;

    j1 = (c1 == c3) ? 0 : ISADJ(c1, c2); /* c1 and c2 need to be joined */
    j2 = ISADJ(c2,c3); /* c2 and c3 need to be joined */

    if (j1) {
        if (j2) { /* splice all three cells together */
            c1->next = c3->next;
            c1->size += 4 + c3->size;
            if (c3 == s.tail)
                s.tail = c1;
        }
        c1->size += 4 + c2->size;
    } else {
        c1->next = c2;
        if (j2) {
            c2->next = c3->next;
            c2->size += 4 + c3->size;
            if (c3 == s.tail)
                s.tail = c2;
        } else {
            c2->next = c3;
        }
    }    
}

#if 0

#include <assert.h>
static char mem[64*1024];

int main(void)
{
    
    int i, size;
    char *ptr[64];

    heap_init(mem, 65536);
    heap_info();
printf("exp 4\n");
#if 0
    for (i = 0;i < 64; i++) {
        size = rand()%2048;
        printf("size = %d\n", size);
        if ((ptr[i] = malloc(size)) == NULL) {
            printf("Error to alloc memory\n");
            return -1;
        }
    }

    heap_info();

    for (i = 0; i < 64; i+=4) {
        free(ptr[i]);
        ptr[i] = NULL;
    }

    heap_info();

    for (i = 0; i < 64; i++) {
        if (ptr[i]) 
            free(ptr[i]);
    }

    heap_info();

#else
    ptr[0] = malloc(3);
    heap_info();
printf("exp 12\n");
    free(ptr[0]);

    ptr[0] = malloc(1024);
    heap_info();
printf("exp 1032\n");
    free(ptr[0]);
    heap_info();
printf("exp 4\n");
    ptr[1] = malloc(31);
    heap_info();
printf("exp 40\n");
    ptr[0] = malloc(1024);
    heap_info();
printf("exp 1068\n");
    free(ptr[1]);
    heap_info();
printf("exp 1032\n");
    free(ptr[0]);

    heap_info();
printf("exp 4\n");

    ptr[0] = malloc(25);
    heap_info();
    ptr[1] = malloc(25);
    heap_info();
    ptr[2] = malloc(25);
    heap_info();
    ptr[3] = malloc(25);
    heap_info();
    ptr[4] = malloc(25);
    heap_info();

    free(ptr[3]);
    heap_info();
    free(ptr[2]);
    heap_info();
    free(ptr[1]);
    heap_info();
    free(ptr[4]);
    heap_info();
    free(ptr[0]);
    heap_info();




    ptr[0] = malloc(25);
    heap_info();
    ptr[1] = malloc(25);
    heap_info();
    ptr[2] = malloc(25);
    heap_info();
    ptr[3] = malloc(25);
    heap_info();
    ptr[4] = malloc(25);
    heap_info();

    free(ptr[0]);
    heap_info();
    free(ptr[1]);
    heap_info();
    free(ptr[2]);
    heap_info();
    free(ptr[3]);
    heap_info();
    free(ptr[4]);
    heap_info();


    ptr[0] = malloc(25);
    heap_info();
    ptr[1] = malloc(25);
    heap_info();
    ptr[2] = malloc(25);
    heap_info();
    ptr[3] = malloc(25);
    heap_info();
    ptr[4] = malloc(25);
    heap_info();

    free(ptr[4]);
    heap_info();
    free(ptr[3]);
    heap_info();
    free(ptr[2]);
    heap_info();
    free(ptr[1]);
    heap_info();
    free(ptr[0]);
    heap_info();


    ptr[0] = malloc(25);
    heap_info();
    ptr[1] = malloc(25);
    heap_info();
    ptr[2] = malloc(25);
    heap_info();
    ptr[3] = malloc(25);
    heap_info();
    ptr[4] = malloc(25);
    heap_info();

    free(ptr[0]);
    heap_info();
    free(ptr[2]);
    heap_info();
    free(ptr[4]);
    heap_info();
    free(ptr[1]);
    heap_info();
    free(ptr[3]);
    heap_info();


    ptr[0] = malloc(25);
    heap_info();
    ptr[1] = malloc(25);
    heap_info();
    ptr[2] = malloc(25);
    heap_info();
    ptr[3] = malloc(25);
    heap_info();
    ptr[4] = malloc(25);
    heap_info();

    free(ptr[4]);
    heap_info();
    free(ptr[2]);
    heap_info();
    free(ptr[0]);
    heap_info();
    free(ptr[3]);
    heap_info();
    free(ptr[1]);
    heap_info();
return -1;

    free(ptr[0]);
    free(ptr[1]);
    heap_info();

#endif

    heap_info();
}
#endif

