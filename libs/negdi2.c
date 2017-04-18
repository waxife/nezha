/**
 *  @file   negdi2.c
 *  @brief  long long negative
 *  $Id: negdi2.c,v 1.2 2014/02/07 02:23:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/31  jedy      New file.
 *
 */

#include "dwunion.h"

long long __negdi2(long long u)
{
	DWunion w;
	DWunion uu;

	uu.ll = u;

	w.s.low = -uu.s.low;
	w.s.high = -uu.s.high - ((unsigned long) w.s.low > 0);

	return w.ll;
}
#if 0
#include <stdio.h>
int main()
{
    long long u = 0x81234567abcdef00ll;
    long long v = 0x1234567abcdef00ll;

    printf("%llx %llx\n", -u, __negdi2(u));
    printf("%llx %llx\n", -v, __negdi2(v));

}
#endif
