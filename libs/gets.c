/**
 *  @file   gets.c
 *  @brief  POSIX like gets function
 *  $Id: gets.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
 #include <stdio.h>

char *
gets (char *buf)
{
	int c;
	char *s = buf;

	while ((c = getchar ()) != '\n') {
		if (c == '\r')
			break;
		*s++ = c;
	}

	*s = 0;
	return buf;
}
