/**
 *  @file   keymap.h
 *  @brief  key/value, key/string mapping
 *  $Id: keymap.h,v 1.2 2014/07/22 08:25:48 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/26  hugo    new file
 *
 */
#ifndef __KEYMAP_H
#define __KEYMAP_H

#define MAKE_KEY_STRING(KEY)    {KEY, #KEY}

typedef const struct {
    int key;
    char *str;
} KeyString;

typedef const struct {
    int key;
    int val;
} KeyValue;


char *key2str (KeyString* table, int tableSize, int key);
int str2key (KeyString* table, int tableSize, char *str);
void ksList (KeyString* table, int tableSize);

const int key2val (KeyValue* table, int tableSize, int key);
const int val2key (KeyValue* table, int tableSize, int val);

#endif /* __KEYMAP_H */
