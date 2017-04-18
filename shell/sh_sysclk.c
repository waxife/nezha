/**
 *  @file   sh_sysclk.c
 *  @brief  system init entry
 *  $Id: sh_sysclk.c,v 1.2 2014/01/27 02:27:48 jedy Exp $
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
#include <mipsregs.h>
#include <sys.h>
#include <unistd.h>

command_init (sh_sysclk, "sysclk", "sysclk");
command_init (sh_cpuidle, "cpuidle", NULL);

static int sh_sysclk (int argc, char **argv, void **result, void *sys_ctx)
{
    sysclk();
    sysclk_info();

    
    printf("\n");

    return 0;
}

__mips32__
static void r4k_wait(void)
{
	__asm__(".set\tmips3\n\t"
		"wait\n\t"
		".set\tmips0");
}

static int sh_cpuidle (int argc, char **argv, void **result, void *sys_ctx)
{
    r4k_wait();

    return 0;
}

#if 0
static int sh_sysapb(int argc, char **argv, void **result)
{
    int speed;
    int rc;

    if (argc < 2)
        goto EXIT;

    rc = get_val(argv[1], (int *)&speed);
    if (rc < 0)
        goto EXIT;

    printf("APB Clock    : %d.%02dMHz\n", speed/1000000, (speed/10000)%100);
    sys_apb_clk = speed;

    usleep(30000);
    serial_init();

    return 0;
EXIT:
    print_usage (sh_sysapb);
    return -1;
}

#endif
