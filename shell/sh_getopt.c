/**
 *  @file   sh_getopt.c
 *  @brief  test getopt()
 *  $Id: sh_getopt.c,v 1.1 2014/08/15 06:42:05 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/08/15  Hugo      New file.
 *
 */
#include <stdio.h>
#include <unistd.h>
#include <shell.h>

command_init (sh_getopt, "getopt", "getopt [-a] [-b <arg>] <non-option arg>");

static int
sh_getopt (int argc, char **argv, void **result, void *sys_ctx)
{
    int c;
    int rc = 0;

    if (argc < 2)
        goto EXIT;

    while ((c = getopt (argc, argv, "-ab:")) != -1) {
        switch (c) {
            case 'a':
                printf ("option %c\n", c);
                break;

            case 'b':
                printf ("option %c with value '%s'\n", c, optarg);
                break;

            case '?':
                printf ("undefined option '%c'\n", optopt);
                break;

            case 1:
                printf ("non-option argument: %s\n", optarg);
                break;

            default:
                printf ("?? getopt returned character code 0%o ??\n", c);
                break;
        }
    }

    return 0;
EXIT:
    print_usage (sh_getopt);
    return rc;
}
