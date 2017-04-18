/**
 *  @file   div64.c
 *  @brief  arithmatic long long shift left
 *  $Id: ashldi3.c,v 1.2 2014/02/07 02:23:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/31  jedy      New file.
 *
 */

#include "dwunion.h"

long long __ashldi3(long long u, word_type b)
{
    DWunion uu, w;
    word_type bm;
     
    if (b == 0)
        return u;
     
    uu.ll = u;
    bm = 32 - b;
     
    if (bm <= 0) {
        w.s.low = 0;
        w.s.high = (unsigned int) uu.s.low << -bm;
    } else {
        const unsigned int carries = (unsigned int) uu.s.low >> bm;
        w.s.low = (unsigned int) uu.s.low << b;
        w.s.high = ((unsigned int) uu.s.high << b) | carries;
    }
 
    return w.ll;
}

#if 0
#include <stdio.h>

int main()
{
    long long u = 0x81234567abcdef88ll;
    long long v = 0x1234567abcedf88ll;

    printf("%llx %llx\n", (u << 8), __ashldi3(u, 8));
    printf("%llx %llx\n", (v << 8), __ashldi3(v, 8));

    printf("%llx %llx\n", (u << 16), __ashldi3(u, 16));
    printf("%llx %llx\n", (v << 16), __ashldi3(v, 16));
    printf("%llx %llx\n", (u << 32), __ashldi3(u, 32));
    printf("%llx %llx\n", (v << 32), __ashldi3(v, 32));
    printf("%llx %llx\n", (u << 38), __ashldi3(u, 38));
    printf("%llx %llx\n", (v << 38), __ashldi3(v, 38));
    printf("%llx %llx\n", (u << 1), __ashldi3(u, 1));
    printf("%llx %llx\n", (v << 1), __ashldi3(v, 1));
    printf("%llx %llx\n", (u << 0), __ashldi3(u, 0));
    printf("%llx %llx\n", (v << 0), __ashldi3(v, 0));
}
#endif

