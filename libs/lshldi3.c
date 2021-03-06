/**
 *  @file   lshldi3.c
 *  @brief  long long shift left
 *  $Id: lshldi3.c,v 1.2 2014/02/07 02:23:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/31  jedy      New file.
 *
 */

#include "dwunion.h"

long long __lshldi3(long long u, word_type b)
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
    unsigned long long u = 0x1f3456789abcll;

    printf("u  = %llx %llx\n", (u << 8), (unsigned long long)__lshldi3(u, 8));
    printf("u  = %llx %llx\n", (u << 16), (unsigned long long)__lshldi3(u, 16));
    printf("u  = %llx %llx\n", (u << 32), (unsigned long long)__lshldi3(u, 32));
    printf("u  = %llx %llx\n", (u << 1), (unsigned long long)__lshldi3(u, 1));
    printf("u  = %llx %llx\n", (u << 0), (unsigned long long)__lshldi3(u, 0));
    printf("u  = %llx %llx\n", (u << 38), (unsigned long long)__lshldi3(u, 38));
    printf("u  = %llx %llx\n", (1ll << 63), (unsigned long long)__lshldi3(1, 63));
}
#endif
