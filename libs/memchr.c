/**
 *  @file   memchr.c
 *  @brief  POSIX like memchr function
 *  $Id: memchr.c,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#include <string.h>
#include <stdio.h>


void *memchr (const void *_s, int c, unsigned int n)
{

	unsigned char *s = (unsigned char *) _s;

	while (n--) {
		if (*s == c)
			return s;
		s++;
	}

	return NULL;
}

#if 0
char string[] = "abcdefghijklmnopqrstuvwxyz";

int main ()
{
	printf ("memchr (string, 'p') = %s\n", 
            memchr (string, 'p', strlen(string)));
	printf ("memchr (string, 'A') = %p\n", 
            memchr(string, 'A', strlen(string)));
}
#endif
