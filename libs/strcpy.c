/**
 *  @file   strcpy.c
 *  @brief  POSIX like strcpy function
 *  $Id: strcpy.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
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

char *
strcpy (char *to, const char *from)
{
	char *p = to;

	while ((*p++ = *from++));
    *p = '\0';

	return to;
}

#if 0
char str0[16];
char str1[16] = "abcdefgh";

int main ()
{
	printf ("strcpy (str0, str1): %s\n", strcpy (str0, str1));
}
#endif
