/**
 *  @file   str2upper.c
 *  @brief  convert string to upper case
 *  $Id: str2upper.c,v 1.1 2014/01/08 13:31:55 hugo Exp $
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
str2upper (char *str)
{
    char *ptr = str;

    while (*ptr) {
        *ptr = toupper (*ptr);
        ptr++;
    }

    return str;
}
