/**
 *  @file   putchar.c
 *  @brief  POSIX like putchar function
 *  $Id: putchar.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#include <stdio.h>
#include <serial.h>

int
putchar (int ch)
{
	if (ch == '\n')
		putb ('\r');
	putb (ch);
	
	return ch;
}
