/**
 *  @file   strstr.c
 *  @brief  POSIX like strstr function
 *  $Id: strstr.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#include <string.h>

char *
strstr (const char *s, const char *find)
{
	const char *ptr;
	int len = strlen (find);

	for (ptr = s; *ptr; ptr++)
		if (!strncmp (ptr, find, len))
			return (char *)ptr;

	return 0;
}

#if 0
char string[] = "abcdefghijklmnopqrstuvwxyz";

int main ()
{
	printf ("strstr (string, \"pqr\") = %s\n", strstr (string, "pqr"));
}
#endif
