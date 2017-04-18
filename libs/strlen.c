/**
 *  @file   strlen.c
 *  @brief  POSIX like strlen function
 *  $Id: strlen.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
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
strlen (const char *s)
{
	const char *start = s;
	while (*s) 
		s++;

	return (s - start);
}

#if 0
char str1[] = "12345678";
char str2[] = "\0";

int main ()
{
	printf ("strlen (str1) = %d\n", strlen (str1));
	printf ("strlen (str2) = %d\n", strlen (str2));
}
#endif
