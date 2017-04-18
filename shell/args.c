/**
 *  @file   args.c
 *  @brief  shell argument helper functions
 *  $Id: args.c,v 1.1.1.1 2013/12/18 03:43:23 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  jedy    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys.h>
#include <shell.h>

int check_addr_aligned(void *addr, int len, int align)
{
    if (! is_valid_addr(addr) || !is_valid_addr((unsigned)addr+len))
        goto EXIT;

    if (align && ((unsigned)addr & (align -1)) != 0)
        goto EXIT;

    return 0;

EXIT:    
    if (len == 0) {
        printf("addr %p is not a valid address\n", addr);
    } else {        
        printf("addr %p~%p is not a valid address range\n", addr, (addr+len));
    }

    return -1;
}

static int check_addr(void *addr, int len)
{
    return check_addr_aligned(addr, len, 0);
}


/**
 * <addr> = 0xADDRESS or <addr> = ADDRESS 
 * NOTE: <addr> is treat as a hexdecimal value whether '0x' prefix or not
 */
int get_addr(const char *arg, void *addr)
{
    char *e;
    unsigned long uaddr;
    int rc;

    uaddr = strtol(arg, &e, 16);
    if (e == arg && uaddr == 0) 
        goto EXIT;

    rc = check_addr((void *)uaddr, 0);
    if (rc < 0) 
        goto EXIT;

    *(unsigned long *) addr = uaddr;
    return 0;

EXIT:
    printf("addr %s is not a valid <addr> argument\n", arg);
    return -1;
}

/**
 * <addr-len> = <addr>+<len> |
 *              <addr>-<addr>
 */ 
int get_addr_len(const char *arg, void *addr, int *len)
{
    char *e, *q;
    unsigned long uaddr, uaddr2;
    int ulen;
    int rc;

    uaddr = strtol(arg, &e, 16);
    if (e == arg && uaddr == 0) 
        goto EXIT;
    
    if (e == NULL) {
        ulen = 0;
    } else {
        q = e+1;
        if (*e == '+') {
            ulen = strtol(q, &e, 0);
            if (e == q && ulen == 0)
                goto EXIT;
            if (ulen < 0)
                goto EXIT;

            if (e && (*e == 'k' || *e == 'K'))
                ulen = ulen * 1024;
            else if (e && (*e == 'm' || *e == 'M'))
                ulen = ulen * 1024 * 1024;

        } else if (*e == '-') {
            uaddr2 = strtol(q, &e, 16);
            if (e == q && uaddr2 == 0)
                goto EXIT;
            if (uaddr2 < uaddr)
                goto EXIT;
            ulen = uaddr2 - uaddr;
        } else {
            ulen = 0;
        }
    }

    rc = check_addr((void *)uaddr, ulen);
    if (rc < 0) 
        goto EXIT;


    *(unsigned long *)addr = uaddr;
    *len = ulen;

    return 0;
    
EXIT:
    printf("addr %s is not a valid <addr-len> argument\n", arg);
    return -1;
}


/**
 * <val> = [+ | -]digits
 */
int get_val(const char *arg, int *val)
{
    char *e;
    int uval;

    uval = strtol(arg, &e, 0);
    if (e == arg && uval == 0)
        goto EXIT;

    *val = uval;

    return 0;
EXIT:
    printf("val %s is not a valid <val> argument\n", arg);

    return -1;
}


/**
 * <val-width> = <val>[-1 | -2 | -4]
 */
int get_val_width(const char *arg, int *val, int *width)
{
    char *e, *q;
    int uval, uwidth = 1;

    uval = strtol(arg, &e, 0);
    if (e == arg && uval == 0)
        goto EXIT;
    
    if (e != NULL) {
        if (*e != '-')
            goto EXIT;

        q = e+1;
        uwidth = strtol(q, &e, 10);
        if (e == q && uwidth == 0)
            goto EXIT;

        if (uwidth != 1 && uwidth != 2 && uwidth != 4)
            goto EXIT;
    }


    *val = uval;
    *width = uwidth;

    return 0;
EXIT:
    printf("%s is not a valid <val>[-1 | -2 | -4] argument\n", arg);
    printf("no space is allow amoung <val>, hyphen, and 1, 2, 4\n");

    return -1;
}

int get_object(const char *arg, int *obj, void *addr, int *len, char **name)
{
    int rc;

    if (*(arg+1) == ':') { /* should be file */
        *name = (char *)arg;
        *obj = OBJ_FILE;
    } else if (*arg == '@') { /* should be image */
        *name = (char *)arg;
        *obj = OBJ_IMAGE;
    } else {
        rc = get_addr_len(arg, addr, len);
        if (rc < 0) 
            return rc;

        *obj = OBJ_ADDR; 
    }

    return 0;
}

