/**
 *  @file   strncmp.c
 *  @brief  POSIX like strncmp function
 *  $Id: strncmp.c,v 1.2 2014/01/28 09:13:27 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <stdio.h> 
#include <string.h>
	
int
strncmp (const char *s1, const char *s2, int n)
{
	int i;
	for (i = 0; i < n; i++)
		if (s1[i] == '\0' || s1[i] != s2[i])
			return (s1[i] - s2[i]);
	return 0;
}

#if 0
char str0[] = "abcdefgh";
char str1[] = "abcdefgh";
char str2[] = "abcdEFGH";

int main ()
{
	printf ("strncmp (str0, str1, 8) = %d\n", strncmp (str0, str1, 8));
	printf ("strncmp (str1, str2, 8) = %d\n", strncmp (str1, str2, 8));
	printf ("strncmp (str1, str2, 4) = %d\n", strncmp (str1, str2, 4));
}
#endif
