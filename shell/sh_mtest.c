/**
 *  @file   sh_mtest.c
 *  @brief  system init entry
 *  $Id: sh_mtest.c,v 1.2 2015/08/06 05:38:46 kevin Exp $
 *  $Author: kevin $
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
#include <sys.h>
#include <unistd.h>

command_init (sh_mtest, "mtest", "-mtest <addr_len> <count>");
command_init (sh_mtest_all, "mtest_all", "-mtest_all");

static int sh_mtest (int argc, char **argv, void **result, void *sys_ctx)
{
    int count, len;
    int saddr, eaddr;
    extern void march(unsigned long saddr, unsigned long eaddr, int count);

    if ( argc != 3 )
        goto EXIT;
        
    if ( get_addr_len(argv[1], &saddr, &len)<0 )
        goto EXIT;
    if ( get_val(argv[2], &count)<0 )
        goto EXIT;

    eaddr = saddr + len;

    printf("NOTE: mtest would destory Tarzan system.\n");
    printf("      After mtest, manually reboot is necessary\n");
    printf("mtest %08x-%08x(not included), count %d\n", saddr, eaddr, count);
    
    sleep(1);

    march(saddr, eaddr-4, count);

    return 0;

EXIT:
    print_usage(sh_mtest);
    return -1;

}
static int sh_mtest_all (int argc, char **argv, void **result, void *sys_ctx)
{
    int count, len;
    int saddr, eaddr;
    extern void march(unsigned long saddr, unsigned long eaddr, int count);


    saddr = 0xA801A000;
    eaddr = saddr + 90112; //88k for T582
    count = 3;
    printf("NOTE: mtest would destory Tarzan system.\n");
    printf("      After mtest, manually reboot is necessary\n");
    printf("mtest %08x-%08x(not included), count %d\n", saddr, eaddr, count);
    
    sleep(1);

    march(saddr, eaddr-4, count);

    return 0;



}
