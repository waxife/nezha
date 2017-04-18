/**
 *  @file   sh_sleep.c
 *  @brief  sleep/usleep command
 *  $Id: sh_sleep.c,v 1.4 2014/02/11 11:59:21 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys.h>
#include <time.h>

command_init (sh_sleep, "sleep", "-sleep <sec>");
command_init (sh_usleep, "usleep", "-usleep <usec>");
command_init (sh_cpu_count, "cpu_count", "cpu_count");
command_init (sh_cpu_timer, "cpu_timer", "cpu_timer");

static int sh_sleep(int argc, char **argv, void **result, void *sys_ctx)
{
    int s = 0;
    int rc = 0;
    time_t t;

    if (argc < 2)
        goto EXIT;
    
    rc = get_val(argv[1], &s);
    if (rc < 0)
        goto EXIT;

    t = time();
    printf("before sleep %d : %ld\n", s, t);

    sleep(s);

    t = time();
    printf("end sleep : %ld\n", t);

EXIT:
    return 0;
}

static int sh_usleep(int argc, char **argv, void **result, void *sys_ctx)
{
    int s = 0;
    int rc = 0;
    int i;
    time_t t;

    if (argc < 2)
        goto EXIT;
    
    rc = get_val(argv[1], &s);
    if (rc < 0)
        goto EXIT;

    printf("test usleep %d * 1000*1000\n", s);

    t = time();
    printf("before sleep %d : %ld\n", s, t);

    for(i = 0; i < 1000*1000; i++)
        usleep(s);

    t = time();
    printf("end sleep : %ld\n", t);

    printf("end usleep\n");

EXIT:
    return 0;

}


static int sh_cpu_count(int argc, char **argv,  void **reslut, void *sys_ctx)
{
    int i;
    unsigned long long t;

    for (i = 0; i < 10; i++) {
        t = read_cpu_count();
        printf("count = %lld (%ld sec  %ld msec)\n", 
            t, hz2sec(t), hz2msec(t));
        sleep(1);
    }

    for (i = 0; i < 10; i++) {
        t = read_cpu_count();
        printf("count = %lld (%ld sec  %ld msec)\n", 
            t, hz2sec(t), hz2msec(t));
        sleep(20);
    }

    return 0;
}

static int sh_cpu_timer(int argc, char **argv,  void **reslut, void *sys_ctx)
{
    unsigned long long t;

    t = read_cpu_count();
    printf("count = %lld (%ld sec  %ld msec)\n", 
            t, hz2sec(t), hz2msec(t));

    return 0;
}
