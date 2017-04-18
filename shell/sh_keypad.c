/**
 *  @file   sh_keypad.c
 *  @brief  keypad test program
 *  $Id: sh_keypad.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/03/15  C.N.Yu    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <ctype.h>
#include <debug.h>
#include "drivers/input/keypad.h"

char keypad_usage[] = "keypad [id] [number_of_tests]";
command_init (sh_keypad, "keypad", keypad_usage);

static int sh_keypad(int argc, char **argv, void **result, void *sys_ctx)
{
    int i = 1;
    int id = 0;
    int cnt = 0;
    int key = 0xFF;

    if (argc < 3) {
        printf("Usage: %s\n", keypad_usage);
        return 0;
    }

    id = atoi(argv[1]);
    cnt = atoi(argv[2]);
    if (cnt <= 0) {
        printf("Usage: %s\n", keypad_usage);
        return 0;
    }

    if(keypad_setup(id) < 0) {
        printf("Usage: %s\n", keypad_usage);
        return 0;
    }
    
    keypad_open();

    printf("[Test %d/%d] Press key : ", i, cnt);
    while (i <= cnt) {
        key = keypad_get_keyvalue();
        if (key > 0) {
            printf("%d\n", key);
            i++;
            printf("[Test %d/%d] Press key : ", i, cnt);
        }
    }

    keypad_close();

    return 0;
}

