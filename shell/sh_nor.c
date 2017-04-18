/**
 *  @file   sh_nor.c
 *  @brief  command for nor test
 *  $Id: sh_nor.c,v 1.7 2014/04/01 05:22:51 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.7 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/03/21  sherman    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <cache.h>
#include <nor.h>
#include <debug.h>
#include <heap.h>
#include <serial.h>

command_init (sh_norw, "norw", "norw ");
command_init (sh_norwp, "norwp", "norwp [0 | 1]");
command_init (sh_norw_bm, "norw_bm", "norw_bm [block+block_num | block] [length] <count>");
command_init (sh_norwp_t, "norwp_t", "norwp_t num");

#define BUF_SIZE 1024
static int sh_norw (int argc, char **argv, void **result, void *sys_ctx)
{    
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct norc_t *norc = sys->pnor;
    char ref_buf[BUF_SIZE], rbuf[BUF_SIZE];
    int ba, pa, i, rc;
       
    for (i = 0; i < BUF_SIZE; i++) 
        ref_buf[i] = i;
        
    ba = 8;
    pa = ba2pa(norc, ba);
    if (pa < 0)
        goto EXIT;
    
    rc = nor_block_erase(pa);
    if (rc < 0)
        goto EXIT;    
        
    rc = nor_data_write(pa, BUF_SIZE, ref_buf);
    if (rc < 0)
        goto EXIT;
        
    /* read back */
    memset(rbuf, 0, BUF_SIZE);
    for (i = 0; i < BUF_SIZE; i++) {
        rbuf[i] = readb((pa|0x80000000)+i);   
    }
    
    for (i = 0; i < BUF_SIZE; i++) {
        if (ref_buf[i] != rbuf[i]) {
            dbg(0, "cmp error, act 0x%02x exp 0x%02x \n", rbuf[i], ref_buf[i]);
            dbg(0, "ref data ... \n");
            memdump(ref_buf, BUF_SIZE);
            dbg(0, "actual data ... \n");
            memdump(rbuf, BUF_SIZE);
        }
    }
    
    printf("read/write/compare passed \n");
    return 0;

EXIT:
    print_usage(sh_norw);
    return -1;
}

static int sh_norw_bm (int argc, char **argv, void **result, void *sys_ctx)
{    
    struct sysc_t *sys = (struct sysc_t *)sys_ctx;
    struct norc_t *norc = sys->pnor;
    char *buf = NULL;
    unsigned int buf_len = 0;
    int ba, pa, i, rc;
    int ba_start = 0, ba_end = 0, ba_num = 0;
    unsigned long long start_t = 0, start_s = 0, start_ms = 0;
    unsigned long long end_t = 0, end_s = 0, end_ms = 0;
    unsigned long long diff_s = 0, diff_ms = 0;
    unsigned long long total_diff_ms = 0;
    int cnt = 1, total_cnt = 0;;
    int c = 0;
    char *p = NULL;

    if (argc < 3)
        goto EXIT;

    p = strchr(argv[1], '+');
    if (p) {
        ba_start = atoi(argv[1]);
        ba_num = atoi(p + 1);
    } else {
        ba_start = atoi(argv[1]);
        ba_num = 1;
    }

    ba_end = ba_start + ba_num - 1;

    if (strchr(argv[2], 'k') || strchr(argv[2], 'K')) {
        buf_len = atoi(argv[2]) * 1024;
    } else {
        buf_len = atoi(argv[2]);
    }

    if (argc >= 4)
        cnt = atoi(argv[3]);

    printf("Test %d NOR write %d bytes in block %d ~ %d ? (y/n): ", cnt, buf_len, ba_start, ba_end);

    while (1) {
        if ((c = getb2()) >= 0) {
            printf("%c\n", c);
            if (c == 'y' || c == 'Y') {
                break;
            } else if (c == 'n' || c == 'N') {
                return 0;
            } else {
                printf("Test %d NOR write %d bytes in block %d ~ %d ? (y/n): ", cnt, buf_len, ba_start, ba_end);
            }
        }
    }
   
    buf = heap_alloc(buf_len);
   
    for (i = 0; i < cnt; i++) {
        printf("======== [%d] ========\n", i);
        ba = ba_start;
        while (ba <= ba_end) {
            printf("NOR write test block %d.\n", ba);
            memset(buf, (i + ba) & 0xFF, buf_len);
            pa = ba2pa(norc, ba);
            if (pa < 0)
                goto EXIT;
   
            rc = nor_block_erase(pa);
            if (rc < 0)
                goto EXIT;    
       
            start_t = read_cpu_count();
            start_s = hz2sec(start_t);
            start_ms =  hz2msec(start_t);
            printf("start time : %lld.%03lld\n", start_s, start_ms);

            rc = nor_data_write(pa, buf_len, buf);
            if (rc < 0)
                goto EXIT;
        
            end_t = read_cpu_count();
            end_s = hz2sec(end_t);
            end_ms =  hz2msec(end_t);
            printf("end time : %lld.%03lld\n", end_s, end_ms);

            if (end_ms < start_ms) {
                diff_s = end_s - start_s - 1;
                diff_ms = end_ms + 1000 - start_ms;
            } else {
                diff_s = end_s - start_s;
                diff_ms = end_ms - start_ms;
            }

            printf("consume time : %lld.%03lld\n", diff_s, diff_ms);

            total_diff_ms += ((diff_s * 1000) + diff_ms);
            total_cnt++;

            rc = memcmp(buf, (char *)(pa | 0x80000000), buf_len);
            if (rc) {
                printf("compare data error!!! (%d)\n", rc);
                break;
            }

            ba++;

            flush_cache_all();
        }
    }

    total_diff_ms /= total_cnt;
    printf("Average NOR write %d bytes : %lld.%03lld\n", buf_len, total_diff_ms / 1000, total_diff_ms % 1000);

    heap_release(buf);

    return 0;

EXIT:
    print_usage(sh_norw);

    if (buf)
        heap_release(buf);

    return -1;
}

static int sh_norwp(int argc, char **argv, void **result, void *sys_ctx)
{
    if (argc == 2) {
        if (argv[1][0] == '0') {
            nor_writeprotect(0);
            printf("set write unprotect.\n");
        } else if (argv[1][0] == '1') {
            nor_writeprotect(1);
            printf("set write protect.\n");
        } else {
            goto EXIT;
        }
    }

    if (nor_is_writeprotect()) {
        printf("NOR flash is write protected.\n");
    } else {
        printf("NOR flash is write unprotected.\n");
    }

    return 0;

EXIT:
    print_usage(sh_norw);
    return -1;
}

static int sh_norwp_t(int argc, char **argv, void **result, void *sys_ctx)
{
    extern int nor_wrsr(unsigned int value);
    unsigned int num = 1;
    unsigned int i = 0;
    unsigned long long start_t = 0, start_s = 0, start_ms = 0;
    unsigned long long end_t = 0, end_s = 0, end_ms = 0;
    unsigned long long diff_s = 0, diff_ms = 0;

    if (argc == 2) {
        num = atoi(argv[1]);
    }

    start_t = read_cpu_count();
    start_s = hz2sec(start_t);
    start_ms =  hz2msec(start_t);
    printf("start time : %lld.%03lld\n", start_s, start_ms);

    for (i = 0; i < num; i++) {
        nor_writeprotect(0);
        nor_writeprotect(1);
    }

    end_t = read_cpu_count();
    end_s = hz2sec(end_t);
    end_ms =  hz2msec(end_t);
    printf("end time : %lld.%03lld\n", end_s, end_ms);

    if (end_ms < start_ms) {
        diff_s = end_s - start_s - 1;
        diff_ms = end_ms + 1000 - start_ms;
    } else {
        diff_s = end_s - start_s;
        diff_ms = end_ms - start_ms;
    }

    printf("test %d NOR write protect/unprotect : %lld.%03lld\n", num, diff_s, diff_ms);

    return 0;
}
