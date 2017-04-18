/**
 *  @file   sh_sysinfo.c
 *  @brief  system init entry
 *  $Id: sh_sysinfo.c,v 1.3 2014/04/29 08:59:57 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.3 $
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

command_init (sh_sysinfo, "sysinfo", "sysinfo");

extern int term;

#define SDRAM_BASE          0xb8000000
#define SDRAM_MRS           (SDRAM_BASE + 0x00)

extern int codec_ver;
extern int __NEZHA_VER;
extern int __BUILD_DATE;
extern int __BUILD_NUMBER;

static int sh_sysinfo (int argc, char **argv, void **result, void *sys_ctx)
{
	unsigned long config0;
	unsigned long config1;
    char *mmutype;
    int ic_linesz, ic_sets, ic_ways, ic_size;
    int dc_linesz, dc_sets, dc_ways, dc_size;
    int lsize;
    //int memsize;
    char *coherency;
    char *vt;
    char *chip;

    sysclk_info();

    config0 = read_c0_config32(0);
    config1 = read_c0_config32(1);

    switch((config0 >> 7) & 0x7) {
    case 1:
        mmutype = "Standard TLB"; break;
    case 3:
        mmutype = "Fixed Mapping"; break;
    default:
        mmutype = "Unknown"; break;
    }     

    lsize = (config1) >> 19 & 7;
    ic_linesz = (lsize) ? (2 << lsize) : 0;
    ic_sets = 64 << ((config1 >> 22) & 7);
    ic_ways = 1 + ((config1 >> 16) & 7);
    ic_size = ic_sets * ic_ways * ic_linesz;

    lsize = (config1) >> 10 & 7;
    dc_linesz = (lsize) ? (2 << lsize) : 0;
    dc_sets = 64 << ((config1 >> 13) & 7);
    dc_ways = 1 + ((config1 >> 7) & 7);
    dc_size = dc_sets * dc_ways * dc_linesz;

    printf("[Nezha]\n");
    printf("Version : %d.%02d (codec v%d.%02d)\n",
           (unsigned)&__NEZHA_VER/100, (unsigned)&__NEZHA_VER%100, codec_ver / 100, codec_ver % 100);
    printf("Build : %d-%d\n", (unsigned)&__BUILD_DATE, (unsigned)&__BUILD_NUMBER);

    printf("\n[CPU]\n");
    printf("CPU : %sr%ld %s endian\n" , 
         (config0 & (3 << 13)) ?  "mips other" : "mips32", 
         ((config0 >> 10) & 0x7) + 1, 
         (config0 & (1 << 15)) ? "big" :  "little");
    printf("MMU : %ld entries %s\n", (config1 >> 25) & 0x1f, mmutype);
    printf("ICache : %d KB %d sets %d ways (%d bytes/line)\n", 
        ic_size/1024, ic_sets, ic_ways, ic_linesz);
    printf("DCache : %d KB %d sets %d ways (%d bytes/line)\n", 
        dc_size/1024, dc_sets, dc_ways, dc_linesz);

    if ((config0 & 0x07) == 0) {
        coherency = "cachable, write-through, no write allocation";
    } else if ((config0 & 0x07) == 1) {
        coherency = "cachable, write-through, write allocation";
    } else if ((config0 & 0x07) == 2 || (config0 & 0x07) == 7) {
        coherency = "uncached";
    } else {
        coherency = "cacheable, write-back, write allocation";
    }

    printf("Kseg0 : %s\n", coherency);

    printf("\n[System]\n");
    chip = sys_chip_ver_name();

    

    printf("Chip Ver     : %s\n", chip);
    //printf("Chip ID      : %c%c %02x%02x\n", readl(0xb0400f30), readl(0xb0400f40), 
    //                                         readl(0xb0400f50), readl(0xb0400f60));
    printf("Clock Source : %lu.%02luMHz\n", sys_clk_src/1000000, (sys_clk_src/10000)%100);
    printf("CPU Clock    : %lu.%02luMHz\n", sys_cpu_clk/1000000, (sys_cpu_clk/10000)%100);
    printf("AHB Clock    : %lu.%02luMHz\n", sys_ahb_clk/1000000, (sys_ahb_clk/10000)%100);
    printf("APB Clock    : %lu.%02luMHz\n", sys_apb_clk/1000000, (sys_apb_clk/10000)%100);

    //printf("\n[Memory]\n");
    //memsize = sys_memsize();
    //printf("SDRAM : %dMB\n",  );
    //printf("      : %dbits\n", readl(SDRAM_MRS) & (1 << 8) ? 32 : 16); 

    printf("\n[Console]\n");
    if (term == T_VT100)
        vt = "vt100";
    else if (term == T_VT102)
        vt = "vt102";
    else 
        vt = "ansi";

    printf("Term : %s\n", vt);
    printf("Baudrate : %lu\n", config_sys_uart_baud);
    printf("\n");

    return 0;
}

