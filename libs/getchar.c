/**
 *  @file   getchar.c
 *  @brief  POSIX like getchar function
 *  $Id: getchar.c,v 1.1.1.1 2013/12/18 03:43:21 jedy Exp $
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
getchar (void)
{
	return getb ();
}
