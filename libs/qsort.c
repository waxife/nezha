/**
 *  @file   qsort.c
 *  @brief  sorts an array
 *  $Id: qsort.c,v 1.3 2014/04/25 06:03:42 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/08/19  hugo    New file.
 */
#include <stdlib.h>

/* This code is stealed from qsort_r() in uClibc-0.9.33.2 */
void
qsort (void *base, int nmemb, int size, __compare_fn_t comp)
{
    int wgap, i, j, k;
    char tmp;

    if ((nmemb > 1) && (size > 0)) {
        wgap = 0;
        do {
            wgap = 3 * wgap + 1;
        } while (wgap < (nmemb-1)/3);
        /* From the above, we know that either wgap == 1 < nmemb or */
        /* ((wgap-1)/3 < (int) ((nmemb-1)/3) <= (nmemb-1)/3 ==> wgap <  nmemb. */
        wgap *= size;          /* So this can not overflow if wnel doesn't. */
        nmemb *= size;           /* Convert nmemb to 'wnel' */
        do {
            i = wgap;
            do {
                j = i;
                do {
                    register char *a;
                    register char *b;
                    j -= wgap;
                    a = j + ((char *)base);
                    b = a + wgap;
                    if ((*comp)(a, b) <= 0) {
                        break;
                    }
                    k = size;
                    do {
                        tmp = *a;
                        *a++ = *b;
                        *b++ = tmp;
                    } while (--k);
                } while (j >= wgap);
                i += size;
            } while (i < nmemb);
            wgap = (wgap - size)/3;
        } while (wgap);
    }
}


