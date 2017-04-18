/**
 *  @file   strchr.c
 *  @brief  POSIX like strchr function
 *  $Id: strchr.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
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

char *
strchr (const char *s, int c)
{
	while (*s) {
		if (*s == c)
			return (char *) s;
		s++;
	}

	return 0;
}


#if 1
char *
strrchr (const char *s, int c)
{
    const char *p = NULL;

	while (*s) {
		if (*s == c) {
            p = s;
        }
		s++;
	}

	return (char *) p;
}
#endif

#if 0
char string[] = "abcdefghijklmnopqrstuvgwxyz";

int main ()
{
	printf ("strchr (string, 'p') = %s\n", strchr (string, 'p'));
    printf ("strrchr (strnig, 'g') = %s\n", strrchr(string, 'q'));
}
#endif
