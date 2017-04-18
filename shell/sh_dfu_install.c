/**
 *  @file   sh_dfu_install.c
 *  @brief  load dfu_install code form flash to memory and run it
 *  $Id: sh_dfu_install.c,v 1.9 2014/03/27 10:30:43 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.9 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/  ycshih    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <cache.h>
#include <watchdog.h>
#include <nor.h>

command_init (sh_dfu_install, "dfu_install", "dfu_install");
command_alias (sh_dfu_install, 0, "dfu", "dfu");

__mips32__
void go32_(char *addr)
{
	char *entry;
	int (*go_main)(void);    
	
	entry = addr;
    go_main = (void *)entry;

    printf("go_main = 0x%p\n", go_main);
    cli();
    go_main();    
}

extern unsigned int _fdfui;
extern unsigned int _dfuins_size;
extern unsigned int _dfuins_nor;
extern int serial_switch(int num);

static int sh_dfu_install(int argc, char **argv, void **result, void *sys_ctx)
{
    char *fdfuins, *dfuins_nor;
    unsigned int dfuins_size;
    fdfuins = (char *)&_fdfui;
    dfuins_size = (int) & _dfuins_size;
    dfuins_nor = (char *)&_dfuins_nor;

    if(argc > 1)
        goto EXIT;

    watchdog_disable();

    /* Write unprotect */
    printf("Set write unprotect first.\n");
    while (nor_is_writeprotect()) {
        nor_writeprotect(0);
    }
    
    printf("Nezha DFU \n(support only update text or resource)\n");
    printf("copy dfu install code %p to 0x%p, size = 0x%x\n", 
        dfuins_nor, fdfuins, dfuins_size);
    memcpy(fdfuins, dfuins_nor, dfuins_size);
    go32_(fdfuins);
    return 0;

EXIT:
    print_usage(sh_dfu_install);
    return -1;
}


