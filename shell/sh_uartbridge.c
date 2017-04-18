/**
 *  @file   sh_uartbridge.c
 *  @brief  use uart path to read write reigsters
 *  $Id: sh_uartbridge.c,v 1.2 2014/05/21 07:42:46 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/12  ycshih    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mconfig.h>
#include <codec.h>
#include <codec_eng/decpic.h>
#include <uartbridge.h>

command_init (sh_uartbridge, "uartbridge", "uartbridge: accress registers through uart path");
command_init (sh_decpic_ub, "decpic_ub", "decpic_ub [<fs>][<name>]");

int sh_uartbridge(int argc, char **argv, void **result, void *sys_ctx)
{
    printf("Set UART bridge mode...");

    while (1) {
        uart_bridge();
    }

    return 0;
}

static int decpic_ub_callback (void **arg)
{
    uart_bridge();

    return 0;
}

static int sh_decpic_ub(int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
    char *fs = NULL;
    char *ch = NULL;
    struct dpic_opt_t opt;
    int rc;

	if (argc < 3)
        goto EXIT;

    memset(&opt, 0, sizeof(struct dpic_opt_t));

    fs = str2upper(argv[1]);
    if (fs != NULL) {
        if (strcmp("FATFS", fs) == 0)
            opt.dev = FATFS;
        else if(strcmp("NORFS", fs) == 0)
            opt.dev = NORFS;
		else
            goto EXIT;
    }
	
    fname = argv[2];
	
    ch = strchr(fname, '.');
    if (ch == NULL || (strncmp(ch + 1, "jpg", 3) != 0)) {
        goto EXIT;
    }
	
    printf("media = %d, file name = %s\n", opt.dev, fname);

    rc = decpic(fname, &opt, decpic_ub_callback);
    
    if (rc < 0)
        goto EXIT;
    
    return 0;

EXIT:
    print_usage(sh_decpic_ub);

    return -1;
}
