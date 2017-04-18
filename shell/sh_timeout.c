/**
 *  @file   sh_timeout.c
 *  @brief  millisecond and time-out macro test
 *  $Id: sh_timeout.c,v 1.2 2014/01/28 02:15:14 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/13  C.N.Yu    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <shell.h>
#include <mipsregs.h>
#include <sys.h>

char timeout_usage[] = "timeout [M <millisecond>] | [m <microseonc>]";
command_init (sh_timeout, "timeout", timeout_usage);

static int sh_timeout(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int tm_out = 0;

    if (argc < 3) {
        printf("%s\n", timeout_usage);
        return 0;
    }

    if (!(strncmp(argv[1], "M", 1))) {
        tm_out = read_c0_count32() + millisec(atoi(argv[2]));
        printf("Wait %d millisecond (%ld ticks)......\n", atoi(argv[2]), millisec(atoi(argv[2])));
    } else if (!(strncmp(argv[1], "m", 1))) {
        tm_out = read_c0_count32() + microsec(atoi(argv[2]));
        printf("Wait %d microsecond (%ld ticks)......\n", atoi(argv[2]), microsec(atoi(argv[2])));
    } else {
        printf("%s\n", timeout_usage);
        return 0;
    }

    while(1) {
        if (is_time_out(tm_out))
            break;
    }

    printf("Timeout !!\n");

    return 0;
}
