/**
 *  @file   init.c
 *  @brief  system init entry
 *  $Id: init.c,v 1.44 2015/04/02 02:12:13 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.44 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <sys.h>
#include <ctype.h>
#include <fs.h>
#include <crc.h>
#include <gpio.h>
#include <debug.h>
#include <fat32.h>
#include <sd.h>
#include <nor.h>
#include <norfs.h>
#include <string.h>
#include <audio.h>
#include "display.h"
#include <registry2.h>
#include "../drivers/scaler/scaler.h"
#include "../drivers/cq/cq.h"
#include <mconfig.h>
#include <watchdog.h>
#include <heap.h>
#include <adjvin.h>

extern void osd2_init (void);
extern int codec_init (void);
extern int main_drive(void *sys_ctx);
unsigned long    sys_cpu_clk;   
unsigned long    sys_plld_clk;   


struct sysc_t gSYSC;

void get_sys_ctx(void **sys_ctx)
{
    *sys_ctx = &gSYSC;
}

extern int codec_ver;
extern unsigned int _fregistry;
extern char __BUILD_DATE;
extern char __BUILD_NUMBER;
extern char __NEZHA_VER;

static struct fat_t gFAT;
static struct norc_t gNORC;

int (* adjvin_blur_cb)(void);
void (* adjvin_restore_cb)(void);

__mips32__
void init(int flash_type, char *argv[], uint32_t magic)
{
    extern void init_IRQ(void);
    unsigned int chip_id = sys_chip_ver();    
    struct sysc_t *sys_ctx = &gSYSC;
    int  rc;
    
    /*Pni Function Select*/
	writel(VAL_SYSMGM_ALTFUNC_SEL, SYSMGM_ALTFUNC_SEL);
    
    writel(0xD8, 0xB0400E00);
    writel(0x00, 0xB0400E30);
    
    sysclk();
    serial_init(config_sys_uart_baud);
    cache_init();
    trap_init();
    init_IRQ();
    sti32();  /* enable interrupts */
    ah1_bus_init();

    /* logo */
    printf("\n\nWelcome to \x1b[32mNezha %u.%02u\x1b[0m (codec v%d.%02d)\n", 
        ((unsigned)&__NEZHA_VER)/100, ((unsigned)&__NEZHA_VER)%100, codec_ver / 100, codec_ver % 100);
    printf("Build \x1b[32m%u-%u\x1b[0m\n", (unsigned) &__BUILD_DATE, (unsigned) &__BUILD_NUMBER);
    printf("Copyright 2010 Terawins Inc.\n");
    printf("\n");
    if ((chip_id>>16) != CHIP_T582) {
        printf("Only support T582 chip id, abort... (%X)\n", chip_id);
    } else {
        printf("Nezha T%3X \n", chip_id);
    }

    printf("argv[0]:%p '%s'\n", argv[0], argv[0]);

	#define DEFAULT_NOR_LAYOUT  "NOR=(64,0-63)"

    memset((char *)&gNORC, 0, sizeof(struct norc_t));
    if (argv[0] == NULL || strncmp(argv[0], "NOR=", 4) != 0) {
        argv[0] = DEFAULT_NOR_LAYOUT;
    }

    rc = nor_init(&gNORC, argv[0]+4);
    if (rc < 0) {
        printf("Use default NOR layout "DEFAULT_NOR_LAYOUT".\n");
        nor_init(&gNORC, DEFAULT_NOR_LAYOUT);
    } else {
        printf("NOR layout %s.\n", argv[0]+4);
    }

    printf("NOR ID: manu id 0x%04x, dev id 0x%04x\n", nor_manuid(), nor_deviceid());
    sys_ctx->pnor = &gNORC;
    nor_writeprotect(1);

    sysclk_info();

	extern unsigned int _fheap;
	extern unsigned int _heap_size;
    rc=heap_create((char *)(&_fheap), (int)(&_heap_size));
    assert(rc == 0);


	registry2_init();
	printf("Registry2 Initial at Address %x \n",(unsigned int)&_fregistry);

    /* init fat */
    sys_ctx->pfat = &gFAT;
    fat_drv_init ();

    sd_donothing(); // force import sdraw.o


    /* setup gpio 9 to control audio mute */
    gpio_out(9);

    /* change audio pll */
    writel(0x05fd5bf8, 0xb7c00114); /* 4.096 MHz */

    audio_init ();

    norfs_init ();

    scaler_open();
	
    cq_init();

    watchdog_init();

    rc = codec_init();
    if(rc < 0)
    	printf("\n\n[ERROR] Initialize Codec Failed!\n\n");

    adjvin_blur_cb = adjvin_blur;
    adjvin_restore_cb = adjvin_restore;

    main_drive((void *)sys_ctx);

    /* enter shell */
    shell((void *)sys_ctx);
}

__mips32__
int main(int argc, char *argv[], char **envp)
{
    init(argc, argv, (uint32_t) envp);
    return 0;
}

