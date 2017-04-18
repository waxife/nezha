/**
 *  @file   keymap.c
 *  @brief  key/value or key/string mapping
 *  $Id: keymap.c,v 1.3 2014/02/07 07:29:01 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/26  hugo    New file.
 *
 */
#include <stdio.h>
#include <string.h>
#include "./keymap.h"

char *
key2str (KeyString* table, int tableSize, int key)
{
    int i;

    for (i = 0; i < tableSize; i++) {
        if (key == table[i].key)
            return table[i].str;
    }

    return "unknown";
}

int
str2key (KeyString* table, int tableSize, char *str)
{
    int i;

    for (i = 0; i < tableSize; i++) {
        if (!strcasecmp (table[i].str, str))
            return table[i].key;
    }

    return -1;
}

void
ksList (KeyString* table, int tableSize)
{
    int i;

    for (i = 0; i < tableSize; i++)
        printf ("    %2d:  %s\n", table[i].key, table[i].str);
}

const int
key2val (KeyValue* table, int tableSize, int key)
{
    int i;

    for (i = 0; i < tableSize; i++)
        if (key == table[i].key)
            return table[i].val;

    return -1;
}

const int
val2key (KeyValue* table, int tableSize, int val)
{
    int i;

    for (i = 0; i < tableSize; i++)
        if (val == table[i].val)
            return table[i].key;

    return -1;
}


