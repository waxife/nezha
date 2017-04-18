/**
 *  @file   str2lower.c
 *  @brief  convert string to lower case
 *  $Id: str2lower.c,v 1.1 2014/01/08 13:31:55 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/08  hugo New file.
 *
 */

#include <string.h>
#include <ctype.h>

char *
str2lower (char *str)
{
    char *ptr = str;

    while (*ptr) {
        *ptr = tolower (*ptr);
        ptr++;
    }

    return str;
}
