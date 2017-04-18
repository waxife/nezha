/**
 *  @file   strncat.c
 *  @brief  POSIX like strncat function
 *  $Id: strncat.c,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  jedy New file.
 *  @date   2007/04/16  jedy pad '\0' to end of string. 
 *
 */
#include <string.h>

char *strncat(char *dst, const char *src, int  n)
{
    char *rtn = dst;

    while (*dst) {
        dst++;  n--;
    }
    
    while (n-- > 0) {
        *dst++ = *src++;
    }

    if (n == 0)
	    *dst = '\0';

    return rtn;
}

