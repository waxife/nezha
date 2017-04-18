/**
 *  @file   cmpdi2.c
 *  @brief  long long compare
 *  $Id: cmpdi2.c,v 1.2 2014/02/07 02:23:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/31  jedy      New file.
 *
 */


#include "dwunion.h"

long long __cmpdi2(long long a, long long b)
{
	DWunion aa, bb;

	aa.ll = a;
    bb.ll = b;

	return (aa.s.high < bb.s.high ? 0 : aa.s.high > bb.s.high ? 2 :
            aa.u.low < bb.u.low ? 0 : bb.u.low > bb.u.low ? 2 : 1);

}
#if 0
#include <stdio.h>
int main()
{
    long long u = 0x81234567abcdef00ll;
    long long v = 0x1234567abcdef00ll;

    printf("%d\n", __cmpdi(u, v));
    printf("%d\n", __cmpdi(v, u));
    printf("%d\n", __cmpdi(v, v));
}
#endif
