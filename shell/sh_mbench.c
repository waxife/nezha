/*
 *  @file   sh_mbench.c
 *  @brief  test program of memory performance
 *  $Id $
 *  $Author $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2008 Terawins Inc. All rights reserved.
 * 
 *  @date   2008/10/01  New file.(Jonathan Kuo)
 *
 */


#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <shell.h>
#include <cache.h>
#include <stopwatch.h>
#include <io.h>


#define TFUNC_BW           1
#define TFUNC_LC           0
#define CACHE_ON           1
#define CACHE_OFF          0
#define READ_MEM           1
#define PREFETCH_READ_MEM  2
#define WRITE_MEM          0
#define RES_480            1
#define RES_800            0
#define GRAPH_ON           1
#define GRAPH_OFF          0


#define OS_TIMER_CLK_DIV_REG    0xB1C00010
#define OS_TIMER_CNT_REG        0xB1C00014
#define OS_TIMER_IRQ_MASK_REG   0xB1C0001C



command_init (sh_mbench, "mbench", "mbench [-f bw|lc] [-c on|off] [-m r|w|p] [-g on|off] [-t times]");

__mips32__
void read1M(void *addr)
{
   __asm__ __volatile__ (
    "add        $12, %0, 1024*1024;" \
    "1: "\
    "add        %0, 16; "\
    "lw $8, -16(%0);" \
    "lw $9, -12(%0);" \
    "lw $10, -8(%0);" \
    "lw $11, -4(%0);" \
    "bne        $12, %0, 1b;"\
    : "+r"(addr));
}

#if 0
__mips32__
void write1M(void *addr)
{
   __asm__ __volatile__ (
    "add        $12, %0, 1024*1024;" \
    "1: "\
    "add        %0, 16; "\
    "sw $8, -16(%0);" \
    "sw $8, -12(%0);" \
    "sw $8, -8(%0);" \
    "sw $8, -4(%0);" \
    "bne        $12, %0, 1b;"\
    : "+r"(addr));
}

__mips32__
void read1M_Prefetch(void *addr)
{
   __asm__ __volatile__ (
    "pref 4, 0(%0); " \
    "add        $12, %0, 1024*1024;" \
    "1: "\
    "pref       4, 16(%0); " \
    "add        %0, 16; "\
    "lw $8, -16(%0);" \
    "lw $9, -12(%0);" \
    "lw $10, -8(%0);" \
    "lw $11, -4(%0);" \
    "bne        $12, %0, 1b;"\
    : "+r"(addr));
}
#endif
/*
void read_lc_1024(void *addr)
{
   __asm__ __volatile__ (
    "add        $12, %0, 1024*1024;" \
    "add        $11, $0, 8; "\
    "move       $10, $0; "\
    "1: "\
    "add        $9, $0, %0; "\
    "2: "\
    "lw         $8, 0($9);" \
    "addi       $9, 8 * 1024; "\
    "bne        $12, $9, 2b;"\
    "move       $7, $10;" \
    "addi       $10, $7, 1;" \
    "bne        $11, $10, 1b;"\
    : "+r"(addr));
}
void write_lc_1024(void *addr)
{
   __asm__ __volatile__ (
    "add        $12, %0, 1024*1024;" \
    "add        $11, $0, 8; "\
    "move       $10, $0; "\
    "1: "\
    "add        $9, $0, %0; "\
    "2: "\
    "sw         $8, 0($9);" \
    "addi       $9, 8 * 1024; "\
    "bne        $12, $9, 2b;"\
    "move       $7, $10;" \
    "addi       $10, $7, 1;" \
    "bne        $11, $10, 1b;"\
    : "+r"(addr));
}
 */

#if 0
__mips32__
void read_lc_1024(void *addr)
{
   __asm__ __volatile__ (
    "add        $12, %0, 1024*1024;" \
    "add        $9, $0, %0; "\
    "2: "\
    "lw         $8, 0($9);" \
    "add        $9, 16; "\
    "bne        $12, $9, 2b;"\
    : "+r"(addr));
}
#endif
#if 0
__mips32__
void write_lc_1024(void *addr)
{
   __asm__ __volatile__ (
    "add        $12, %0, 1024*1024;" \
    "add        $9, $0, %0; "\
    "2: "\
    "sw         $8, 0($9);" \
    "add        $9, 16; "\
    "bne        $12, $9, 2b;"\
    : "+r"(addr));
}
#endif

static int sh_mbench(int argc, char **argv, void **result, void *sys_ctx)
{
    int  rc;

    int  tfunc_flag = TFUNC_BW;
    int  cache_on = CACHE_ON;
    int  rw_flag = READ_MEM;
    int  graph_on = GRAPH_OFF;
    char *start_addr = (char *)0x00500000;
    char *addr;
    int  repeat_num = 500;
    int  val;
    int  i, j;
    unsigned long count_start = 0, count_end = 0, consumed;
    int  apb_clk_freq = 30 * 1000000;
    int  tick_unit;
    int  clk_div;


    //new_stopwatch(sw);
#if 0
    for (i = 1; i < argc; i++) {
        if (strcmp("-f", argv[i]) == 0) {
            i++;
            if (i >= argc) break;
            if (strcmp("bw", argv[i]) == 0) {
                tfunc_flag = TFUNC_BW;
            } else if (strcmp("lc", argv[i]) == 0) {
                tfunc_flag = TFUNC_LC;
            }
            //printf("test function = %d\n", tfunc_flag);
        } else if (strcmp("-c", argv[i]) == 0) {
            i++;
            if (i >= argc) break;
            if (strcmp("on", argv[i]) == 0) {
                cache_on = CACHE_ON;
            } else if (strcmp("off", argv[i]) == 0) {
                cache_on = CACHE_OFF;
            }
            //printf("cache on = %d\n", cache_on);
        } else if (strcmp("-m", argv[i]) == 0) {
            i++;
            if (i >= argc) break;
            if (strcmp("r", argv[i]) == 0) {
                rw_flag = READ_MEM;
            } else if (strcmp("p", argv[i]) == 0) {
                rw_flag = PREFETCH_READ_MEM;
            } else if (strcmp("w", argv[i]) == 0) {
                rw_flag = WRITE_MEM;
            }
            //printf("read flag/write = %d\n", rw_flag);
        } else if (strcmp("-g", argv[i]) == 0) {
            i++;
            if (i >= argc) break;
            if (strcmp("on", argv[i]) == 0) {
                graph_on = GRAPH_ON;
            } else if (strcmp("off", argv[i]) == 0) {
                graph_on = GRAPH_OFF;
            }
            //printf("graph on = %d\n", graph_on);
        } else if (strcmp("-t", argv[i]) == 0) {
            i++;
            if (i >= argc) break;
            rc = get_val(argv[i], (int *)&repeat_num);
            if (rc < 0)
                goto EXIT;
            if (repeat_num < 0) repeat_num = 1;
            //printf("repeat times = %d\n", repeat_num);
        }
    }
#endif
    repeat_num = 1;
    //mark_start(sw);

#if 0
    printf("Memory performance test program\n");

    printf("TFunc: %s, cache: %s, mode: %s, graph: %s, repeat: %d\n", 
            (tfunc_flag == TFUNC_BW) ? "bw" : "lc",
            (cache_on == CACHE_ON) ? "on" : "off",
            (rw_flag == PREFETCH_READ_MEM) ? "prefetch" : ((rw_flag == READ_MEM) ? "read" : "write"),
            (graph_on == GRAPH_ON) ? "on" : "off",
            repeat_num
          );

    if (graph_on) {
   	writel(readl(0xb04019D0) | 0xE0, 0xb04019D0);
	writel(0x11,0xb0400E20);
    } else 
#endif
    {
   	writel(readl(0xb04019D0) & 0x7F, 0xb04019D0);
	writel(0x11,0xb0400E20);
    }

    //printf("os timer irq mask reg = %08X(should be 0x0F)\n", readl(OS_TIMER_IRQ_MASK_REG));
    clk_div = readl(OS_TIMER_CLK_DIV_REG);
    //printf("os timer div reg = %d(%08X)\n", clk_div, clk_div);
    if (clk_div == 0) {
        clk_div = 999;
        writel(clk_div, OS_TIMER_CLK_DIV_REG);
        //printf("os timer div reg = %d(%08X)\n", clk_div, clk_div);
    }
    tick_unit = (apb_clk_freq / (readl(OS_TIMER_CLK_DIV_REG) + 1));

    while (1) {

        if (cache_on)
            addr = (char *)(((long)start_addr & 0x1FFFFFFF) | 0x80000000);
        else
            addr = (char *)(((long)start_addr & 0x1FFFFFFF) | 0xA0000000);

        /* read/write */
        //if (rw_flag == READ_MEM) 
        {

            count_start = readl(OS_TIMER_CNT_REG); /* read os timer counter */

            //for (j = 0; j < repeat_num; j++) 
            {
#if 0
                for (i = 0; i < (0x00000001 << 18); i++)
                    val = readl(((long)addr + 4*i));
#else
                //if (tfunc_flag == TFUNC_BW)
                    read1M((void *)addr);
#if 0
                else if (tfunc_flag == TFUNC_LC)
                    read_lc_1024((void *)addr);
                else
                    printf("ERROR: wrong test function\n");
#endif
#endif
            }

            count_end = readl(OS_TIMER_CNT_REG); /* read os timer counter */

        }
#if 0
        else if (rw_flag == PREFETCH_READ_MEM) {

            count_start = readl(OS_TIMER_CNT_REG); /* read os timer counter */

            for (j = 0; j < repeat_num; j++) {
#if 0
                for (i = 0; i < (0x00000001 << 18); i++)
                    val = readl(((long)addr + 4*i));
#else
                if (tfunc_flag == TFUNC_BW)
                    read1M_Prefetch((void *)addr);
                else if (tfunc_flag == TFUNC_LC)
                    read_lc_1024((void *)addr);
                else
                    printf("ERROR: wrong test function\n");
#endif
            }

            count_end = readl(OS_TIMER_CNT_REG); /* read os timer counter */

        } else {
            val = 0xFFEEDDCC;

            count_start = readl(OS_TIMER_CNT_REG); /* read os timer counter */

            for (j = 0; j < repeat_num; j++) {
#if 0
                for (i = 0; i < (0x00000001 << 18); i++)
                    writel(val, ((long)addr + 4*i));
#else
                if (tfunc_flag == TFUNC_BW)
                    write1M((void *)addr);
                else if (tfunc_flag == TFUNC_LC)
                    write_lc_1024((void *)addr);
                else
                    printf("ERROR: wrong test function\n");
#endif
            }

            count_end = readl(OS_TIMER_CNT_REG); /* read os timer counter */

        }
#endif
        consumed = count_end - count_start;
        printf("execution time: %ld.%ld sec\n", consumed / tick_unit, (consumed % tick_unit) * 1000 / tick_unit);
        //printf("    (%lu - %lu) = %lu\n", count_end, count_start, consumed);
        //printf("    consumed %lu, tick_unit %d\n", consumed, tick_unit);

        break;

    }


    printf("end ...\n");

    //flush_cache_all();
    
    //mark_stop(sw);
    //mark_show(sw);

    return 0;

EXIT:
#if 0
    print_usage (sh_mbench);
    printf("Option: -h           show usage\n"
           "        -f [bw|lc]   test function: bandwidth test/latency test(default: bandwidth test)\n"
           "        -c [on|off]  cache: on/off(default: on)\n"
           "        -m [r|w]     r/w mode: read from memory to reg(default)/write from reg to memory\n"
           "        -g [on|off]  graphic mode: on/off(default: off)\n"
           "        -v [on|off]  video mode: on/off(default: off)\n"
           "        -r [480|800] resolution: 480x360/800x600(default: 480x360)\n"
           "        -t [number]  repeat times: repeat times of copy 1M data(default: 500)\n"
           );
#endif
    return -1;
}

