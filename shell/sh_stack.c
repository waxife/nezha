/**
 *  @file   sh_rx.c
 *  @brief  loadx command
 *  $Id: sh_stack.c,v 1.3 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/01  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <cache.h>

command_init (sh_stack, "stack", "stack");

extern unsigned int _fdata, _edata, _data_size;
extern unsigned int _fbss, _ebss, _bss_size;

static int sh_stack (int argc, char **argv, void **result, void *sys_ctx)
{
   printf("stack barrier %x\n", stack_barrier());
   printf("stack pointer %lx\n", (uint32_t)get_sp());
   printf("stack size %d\n", stack_size());
   printf("------------------\n");
   printf("data %x - %x (%d)\n", (int)(&_fdata), (int)(&_edata), (int)(&_data_size));
   printf("bss  %x - %x (%d)\n", (int)(&_fbss), (int)(&_ebss), (int)(&_bss_size));

   return 0;

//EXIT:
//    print_usage(sh_stack);
//    return -1;
}

