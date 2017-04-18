/**
 *  @file   sh_dfu.c
 *  @brief  command for jump to IROM dfu mode
 *  $Id: sh_uart_switch.c,v 1.1.1.1 2013/12/18 03:43:38 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/12/15  sherman    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <cache.h>

command_init (sh_uart_switch, "uart_switch", "uart_switch <num>");

extern int serial_switch(int num);

static int sh_uart_switch (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc, num;
    
    if (argc < 2)
        goto EXIT;
    rc = get_val(argv[1], &num);
	if (rc < 0)
		goto EXIT;

    rc = serial_switch(num);
    if (rc < 0)
        goto EXIT;
    
    return 0;

EXIT:    
    print_usage(sh_uart_switch);
    printf("1: normal UART \n");
    printf("2: AFC UART \n");
    return -1;
}


