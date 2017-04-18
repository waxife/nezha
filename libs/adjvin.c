/**
 *  @file   adjvin.c
 *  @brief  adjust video input parameters for record file function
 *  $Id: adjvin.c,v 1.2 2015/04/01 13:05:25 cnyu Exp $
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
#include <cache.h>
#include "drivers/cq/cq.h"
#include <debug.h>

#define REG_NOISE_THRESHOLD 0xB04026A0
#define REG_CONTRAST        0xB0402080
#define REG_BRIGHTNESS      0xB0402090
#define REG_SATURATION      0xB04020A0
static char blur_step = 0;
static char noise_step = 0x40;
static char cbs_step = 0x08;
static short reg_P2_8A = 0;
static short reg_P2_28 = 0;
static short reg_P2_29 = 0;
static short reg_P2_2A = 0;
static short reg_P2_8A_tmp = 0;
static short reg_P2_28_tmp = 0;
static short reg_P2_29_tmp = 0;
static short reg_P2_2A_tmp = 0;

#define wait_cq_busy() \
({ \
    int wait = 60; \
    \
    while (wait--) { \
        if (!cq_busy()) break; \
        \
        usleep(1000); \
    } \
})

int adjvin_blur(void)
{
    dbg(0, "adjuset video input parameters.\n");

    if (blur_step == 0) {
        blur_step = 1;
        reg_P2_8A_tmp = reg_P2_8A = readl(REG_NOISE_THRESHOLD);
        reg_P2_28_tmp = reg_P2_28 = readl(REG_CONTRAST);
        reg_P2_29_tmp = reg_P2_29 = readl(REG_BRIGHTNESS);
        reg_P2_2A_tmp = reg_P2_2A = readl(REG_SATURATION);
        reg_P2_8A_tmp = (reg_P2_8A_tmp + noise_step) > 0xFF ? 0xFF : reg_P2_8A + noise_step;

        wait_cq_busy();

        writel(reg_P2_8A_tmp, REG_NOISE_THRESHOLD);

        dbg(0, "blur step 1: set P2_8A = 0x%02X\n", reg_P2_8A_tmp);
    } else if (blur_step == 1) {
        reg_P2_8A_tmp = (reg_P2_8A_tmp + noise_step) > 0xFF ? 0xFF : reg_P2_8A_tmp + noise_step;

        wait_cq_busy();

        writel(reg_P2_8A_tmp, REG_NOISE_THRESHOLD);
        if (reg_P2_8A_tmp == 0xFF) {
            blur_step = 2;
        }

        dbg(0, "blur step 2: blur P2_8A = 0x%02X\n", reg_P2_8A_tmp);
    } else if (blur_step == 2) {
        reg_P2_28_tmp = (reg_P2_28_tmp - cbs_step) < 0 ? 0 : reg_P2_28_tmp - cbs_step;
        reg_P2_29_tmp = (reg_P2_29_tmp + cbs_step) > 0xFF ? 0xFF : reg_P2_29_tmp + cbs_step;
        reg_P2_2A_tmp = (reg_P2_2A_tmp - cbs_step) < 0 ? 0 : reg_P2_2A_tmp - cbs_step;

        wait_cq_busy();

        writel(reg_P2_28_tmp, REG_CONTRAST);
        writel(reg_P2_29_tmp, REG_BRIGHTNESS);
        writel(reg_P2_2A_tmp, REG_SATURATION);
        if (reg_P2_28_tmp == 0 && reg_P2_29_tmp == 0xFF && reg_P2_2A_tmp == 0) {
            blur_step = 3;
        }

        dbg(0, "blur step 3:set P2_8A = 0x%02X, P2_28 = 0x%02X, P2_29 = 0x%02X, P2_2A = 0x%02X\n", reg_P2_8A_tmp, reg_P2_28_tmp, reg_P2_29_tmp, reg_P2_2A_tmp);
    } else if (blur_step == 3) {
        /* recfile() will exit if return -1 */
        dbg(0, "failed in blur limit !!\n");
        return -1;
    }

    return 0;
}

void adjvin_restore(void)
{
    if (blur_step) {
        int wait = 60;

        printf("restore video input parameters\n");

        while (wait--) {
            if (!cq_busy())
                break;

            usleep(1000);
        }

        blur_step = 0;
        writel(reg_P2_8A, REG_NOISE_THRESHOLD);
        writel(reg_P2_28, REG_CONTRAST);
        writel(reg_P2_29, REG_BRIGHTNESS);
        writel(reg_P2_2A, REG_SATURATION);
    }
}
