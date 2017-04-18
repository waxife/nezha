/**
 *  @file   puts.c
 *  @brief  POSIX like puts function
 *  $Id: puts.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#include <stdio.h>

int puts (const char *buf)
{
	while (*buf)
		putchar (*buf++);
    putchar('\n');
		
	return 1;		
}

int puts0 (const char *buf)
{
	while (*buf)
		putchar (*buf++);
    
    return 1;
}
