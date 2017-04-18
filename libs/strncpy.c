/**
 *  @file   strncpy.c
 *  @brief  POSIX like strncpy function
 *  $Id: strncpy.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  jedy New file.
 *  @date   2007/04/16  jedy pad '\0' to strncpy
 *
 */

#include <string.h>

char
*strncpy (char *to, const char *from, int length)
{
	char *p = to;
	char *limit = p + length;
	
	while (p < limit) {
		if ((*p++ = *from++) == '\0')
			break;
	}

    if (p < limit)
        *p = '\0';

	return to;
}

#if 0
char str0[16] = "abcdefgh";
char str1[16];
char str2[16] = "abcdEFGH";

int main ()
{
	printf ("strncpy (str1, str0, 15): %s\n", strncpy (str1, str0, 15));
	printf ("strncpy (str1, str2, 6): %s\n",  strncpy (str1, str2, 6));
}
#endif
