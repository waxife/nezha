/**
 *  @file   memdump.c
 *  @brief  POSIX like memdump function
 *  $Id: memdump.c,v 1.2 2014/08/02 13:21:40 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
 #include <string.h>
#include <stdio.h>
#include <io.h>

#define COLUMN 16
#define ALIGN_4(x)   (const char *)((unsigned int)(x) & (-4))

void
memdump (const char *mem, int len)
{
	int rest, i, c;
    const char *addr = NULL;
    union {
        unsigned long d;
        unsigned char a[4];
    } u;

	// mem = (char *) KSEG1ADDR (mem);
    //printf("dump %p+%d\n", mem, len);
    //printf("ALIGN_4 %p %p %p %p %p\n", ALIGN_4(mem), ALIGN_4(mem+1), ALIGN_4(mem+2), ALIGN_4(mem+3), ALIGN_4(mem+4));
	while (len) {
		rest = len > COLUMN ? COLUMN : len;

		printf ("%p  ", mem);
		for (i = 0; i < COLUMN; i++) {
            if (addr != ALIGN_4(mem+i)) {
                addr = ALIGN_4(mem+i);
                u.d = *(unsigned long *) addr;
            }

			if (i < rest)
				printf ("%02x ", u.a[(unsigned)(mem+i)&0x3]);
			else
				printf ("   ");
		}

		printf ("| ");
		for (i = 0; i < rest; i++) {
            if (addr != ALIGN_4(mem+i)) {
                addr = ALIGN_4(mem+i);
                u.d = *(unsigned long *) addr;
            }
            c = u.a[(unsigned)(mem+i)&0x3];
            
			if (c >= 0x20 && c <= 0x7e)
				putchar (c);
			else
				putchar ('.');
		}

		putchar ('\n');
		mem += rest;
		len -= rest;
	}
}

void
hexdump (unsigned int start, unsigned int len, const void *data)
{
    unsigned int offset = start, l, i;
    unsigned char *head, *tail;

    head = (unsigned char *) data;
    tail = head + len;

    while (head < tail) {
        printf ("%06x  ", (unsigned int)offset);

        /* line dump */
        l = (tail - head) > 16 ? 16 : (tail - head);
        for (i = 0; i < 16; i++) {
            if (i < l)
                printf ("%02x ", head[i]);
            else
                printf ("   ");
        }
        printf ("| ");
        for (i = 0; i < l; i++) {
            if (head[i] >= 0x20 && head[i] < 0x7f)
                printf ("%c", head[i]);
            else
                printf (".");
        }
        printf ("\n");

        head += l;
        offset += l;
    }
}
