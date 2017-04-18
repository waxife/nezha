/**
 *  @file   strtol.c
 *  @brief  POSIX like strtol function
 *  $Id: strtol.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>


static long int strtol_10(const char *nptr, char **endptr)
{
    long int negative = 0;
    unsigned long val = 0;
    const char *p;

    /* skip leading space */
    for (p = nptr; *p != '\0'; p++) {
        if (!isblank(*p))
            break;
    }

    if (*p == '\0')
        goto EXIT;

    /* optional '+' or '-' */
    if (*p == '-' || *p == '+') {
        if (*p == '-') {
            negative = 1;
            p++;
        } else {
            negative = 0;
            p++;
        }

        /* skip whitespace */
        for (; *p != '\0'; p++) {
            if (!isblank(*p))
                break;
        }
        
    }

    if (*p == '\0')
        goto EXIT;


    /* digit ? */
    for (; *p != '\0'; p++) {
        if (isdigit(*p)) {
            val = (val * 10) + (*p) - '0';
        } else {
            break;
        }
    }

    if (endptr) {
        *endptr = (*p == '\0') ? NULL : (char *)p;
    }

    return (negative) ? (long) -val : (long) val;

EXIT:
    if (endptr) {
        *endptr = (char *)nptr;
    }
    return 0;
}


static long int strtol_16(const char *nptr, char **endptr)
{
    unsigned long val = 0;
    int xval;
    const char *p;

    /* skip leading space */
    for (p = nptr; *p != '\0'; p++) {
        if (!isblank(*p))
            break;
    }

    if (*p == '\0')
        goto EXIT;

    /* skip leading '0x' */
    if (*p == '0' && (*(p+1) == 'x' || *(p+1) == 'X'))
        p += 2;

    /* xdigit ? */
    for (; *p != '\0'; p++) {
        if ((*p) >= '0' && (*p) <= '9')
            xval = (*p) - '0';
        else if ((*p) >= 'A' && (*p) <= 'F') 
            xval = (*p) - 'A' + 10;
        else if ((*p) >= 'a' && (*p) <= 'f')
            xval = (*p) - 'a' + 10;
        else
            break;

        val = val * 16 + xval;
    }
    if (endptr) {
        *endptr = (*p == '\0') ? NULL : (char *) p;
    }

    return (long) val;

EXIT:
    if (endptr) {
        *endptr = (char *)nptr;
    }
    return 0;
}


long int strtol(const char *nptr, char **endptr, int base)
{
    if (base == 10) {
        return strtol_10(nptr, endptr);
    } else if (base == 16) {
        return strtol_16(nptr, endptr);
    } else {
        if (*nptr == '0' && (*(nptr+1) == 'X' || *(nptr+1) == 'x'))
            return strtol_16(nptr, endptr);
        else
            return strtol_10(nptr, endptr);
    } 

    return 0;
}

