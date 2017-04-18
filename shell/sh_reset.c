/**
 *  @file   sh_reset.c
 *  @brief  system init entry
 *  $Id: sh_reset.c,v 1.2 2014/01/27 12:26:24 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
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

command_init (sh_reset, "reset", "reset [<delay>]");
command_alias (sh_reset, 0, "reboot", "reboot [<delay>]");


static int sh_reset (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    int val;

    if (argc > 1) {
        rc = get_val(argv[1], &val);
        if (rc < 0) 
            goto EXIT;

        printf("After %d second(s) system will be reset\n", val);
        sleep(val);
    }

    sys_reset();

    return 0;

EXIT:
    print_usage(sh_reset);
    return -1;
}


