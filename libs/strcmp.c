/**
 *  @file   strcmp.c
 *  @brief  POSIX like strncat function
 *  $Id: strcmp.c,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <ctype.h>
#include <string.h>

int
strcmp (const char *s1, const char *s2)
{
	while (*s1++ == *s2) {
		if (*s2++ == '\0')
			return 0;
	}
	return ((int)s1[-1] - (int)s2[0]);
}

int 
strcasecmp(const char *s1, const char *s2)
{
    while(toupper(*s1++) == toupper(*s2)) {
        if (*s2++ == '\0')
            return 0;
    }
    return (((int)toupper(s1[-1])) - (int)toupper(s2[0]));
}

#if 0
char str0[] = "abcdefgh";
char str1[] = "abcdefgh";
char str2[] = "abcdEFGH";

int main ()
{
	printf ("strcmp (str0, str1) = %d\n", strcmp (str0, str1));
	printf ("strcmp (str1, str2) = %d\n", strcmp (str1, str2));
	printf ("strcasecmp (str0, str1) = %d\n", strcasecmp (str0, str1));
	printf ("strcasecmp (str1, str2) = %d\n", strcasecmp (str1, str2));
}
#endif
