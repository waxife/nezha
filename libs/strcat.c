/**
 *  @file   strcat.c
 *  @brief  POSIX like strcat function
 *  $Id: strcat.c,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
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
strcat (char *dst, const char *src)
{
	char *tmp = dst;

	while (*dst != '\0') {
		dst++;
	}

	do {
		*dst++ = *src++;
	} while (*src != '\0');
    
    *dst = '\0';

	return tmp;
}

#if 0
int main ()
{
	char dst[32] = "DST";
	char src[32] = "SRC";
	char *tmp;

	printf ("src: %s\n", src);
	printf ("dst: %s\n", dst);
	printf ("\n");

	tmp = strcat (dst, src);
	printf ("src: %s\n", src);
	printf ("dst: %s\n", dst);
	printf ("tmp: %s\n", tmp);
}
#endif
