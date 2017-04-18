/**
 *  @file   strsep.c
 *  @brief  POSIX like strsep function
 *  $Id: strsep.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#include <string.h>
#include <stdio.h>

/**
 * strsep - Split a string into tokens
 * @s: The string to be searched
 * @ct: The characters to search for
 *
 * strsep() updates @s to point after the token, ready for the next call.
 *
 * It returns empty tokens, too, behaving exactly like the libc function
 * of that name. In fact, it was stolen from glibc2 and de-fancy-fied.
 * Same semantics, slimmer shape. ;)
 */
char * strsep(char **s, const char *ct)
{
        char *sbegin = *s, *end;

        if (sbegin == NULL)
                return NULL;

        end = strpbrk(sbegin, ct);
        if (end)
                *end++ = '\0';
        *s = end;

        return sbegin;
}

