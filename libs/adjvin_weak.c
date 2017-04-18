/**
 *  @file   adjvin.c
 *  @brief  adjust video input parameters for record file function
 *  $Id: adjvin_weak.c,v 1.2 2015/04/02 03:11:37 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2015 Terawins Inc. All rights reserved.
 * 
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <serial.h>
#include <sys.h>
#include <io.h>

int adjvin_blur(void) __attribute__((weak));
int adjvin_blur(void)
{
    printf("[WARN] Not implement adjvin_blur() and return -1 to exit recfile() !!!!\n");
    return -1;
}

void adjvin_restore(void) __attribute__((weak));
void adjvin_restore(void)
{
    printf("[WARN] Not implement adjvin_restore() !!!!\n");
}
