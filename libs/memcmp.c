/**
 *  @file   memcmp.c
 *  @brief  POSIX like memcmp function
 *  $Id: memcmp.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <string.h>

int
memcmp (const void *_s1, const void *_s2, int n)
{
	unsigned char *s1 = (unsigned char *) _s1;
	unsigned char *s2 = (unsigned char *) _s2;

	while (n--) {
		if (*s1 != *s2)
			return (*s1 - *s2);
		s1++;
		s2++;
	}
	return 0;
}

#if 0
char str1[] = "abcdefgh";
char str2[] = "abcdEFGH";

int main ()
{
	printf ("memcmp (str1, str2, 5) = %d\n", memcmp (str1, str2, 5));
	printf ("memcmp (str1, str2, 4) = %d\n", memcmp (str1, str2, 4));
}
#endif
