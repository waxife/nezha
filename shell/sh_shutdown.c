/**
 *  @file   sh_shutdown.c
 *  @brief  system init entry
 *  $Id: sh_shutdown.c,v 1.1.1.1 2013/12/18 03:43:23 jedy Exp $
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
#include <shell.h>
#include <unistd.h>
#include <sys.h>

command_init (sh_shutdown, "shutdown", "shutdown [<delay>]");


static int sh_shutdown (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    int val;

    if (argc > 1) {
        rc = get_val(argv[1], &val);
        if (rc < 0) 
            goto EXIT;

        printf("After %d second(s) system will be shutdown\n", val);
        sleep(val);
    }

    sys_shutdown();

    return 0;

EXIT:
    print_usage(sh_shutdown);
    return -1;
}


