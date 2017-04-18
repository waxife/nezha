/**
 *  @file   sh_gpioa.c
 *  @brief  A sample code of GPIOA operation
 *  $Id: sh_gpioa.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/06/10  ycshih    New file.
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
#include <unistd.h>
#include <stdlib.h>
#include <interrupt.h>
#include <irq.h>
#include <gpio.h>

#define     GPIOA_LEVEL         (GPIO_BASE + 0x80)
#define     GPIOA_OE            (GPIO_BASE + 0x84)
#define     GPIOA_SET           (GPIO_BASE + 0x88)
#define     GPIOA_CLEAR         (GPIO_BASE + 0x8C)
#define     GPIOA_RED           (GPIO_BASE + 0x90)
#define     GPIOA_FED           (GPIO_BASE + 0x94)
#define     GPIOA_RED_STATUS    (GPIO_BASE + 0x98)
#define     GPIOA_FED_STATUS    (GPIO_BASE + 0x9C)
#define     GPIOA_INT_MASK      (GPIO_BASE + 0xA4)
#define     GPIOA_MODE          (GPIO_BASE + 0xA8)
#define     GPIOA_DRIVE         (GPIO_BASE + 0xAC)

command_init (sh_gpioa_read, "gpioa_read", "gpioa_read [GPIOA number]");
command_init (sh_gpioa_set, "gpioa_set", "gpioa_set [GPIOA number]");
command_init (sh_gpioa_clear, "gpioa_clear", "gpioa_clear [GPIOA number]");
command_init (sh_gpioa_in, "gpioa_in", "gpioa_in [GPIOA number]");
command_init (sh_gpioa_out, "gpioa_out", "gpioa_out [GPIOA number]");
command_init (sh_gpioa_in_nbits, "gpioa_in_nbits", "gpioa_in_nbits [GPIOA pin2bit]");
command_init (sh_gpioa_out_nbits, "gpioa_out_nbits", "gpioa_out_nbits [GPIOA pin2bit]");
command_init (sh_gpioa_set_drive, "gpioa_set_drive", "gpioa_set_drive [GPIOA pin2bit]");
command_init (sh_gpioa_drive, "gpioa_drive", "gpioa_drive [GPIOA pin2bit]");
command_init (sh_gpioa_test_trigger, "gpioa_test_trigger", "gpioa_test_trigger");
command_init (sh_gpioa_test_drive, "gpioa_test_drive", "gpioa_test_drive");
//command_init (sh_gpioa_isr_init, "gpioa_isr_init", "gpioa_isr_init");
//command_init (sh_gpioa_isr_release, "gpioa_isr_release", "gpioa_isr_release");

static int gpioa_read(void)
{
    unsigned int val = 0;

    val = readl(GPIOA_LEVEL);
    printf("Read input value : 0x%08X\n", val);

    val = readl(GPIOA_RED_STATUS);
    printf("Read rising edge status : 0x%08X\n", val);
    writel(val, GPIOA_RED_STATUS);

    val = readl(GPIOA_FED_STATUS);
    printf("Read falling edge status : 0x%08X\n", val);
    writel(val, GPIOA_FED_STATUS);

    return 0;
}

static int gpioa_set(unsigned int gpioa_no)
{
    unsigned int tmp = 0;

    tmp = readl(GPIO_ALTFUNC_SEL);
    if (!(tmp & (1 << 15))) {
        printf("altfunc_sel does not select GPIOA !!!!\n");
        goto EXIT;
    }

    if (gpioa_no == 11) {
        if (!(tmp & (1 << 20))) {
            printf("altfunc_sel does not select GPIOA !!!!\n");
            goto EXIT;
        }
    }

    if (((readl(GPIOA_OE) >> gpioa_no) & 0x01) != 1) {
        printf("GPIOA%02d is GP_OUT mode!!!!\n", gpioa_no);
        goto EXIT;
    }

    writel((1 << gpioa_no), GPIOA_SET);

EXIT:
    return 0;
}

static int gpioa_clear(unsigned int gpioa_no)
{
    unsigned int tmp = 0;

    tmp = readl(GPIO_ALTFUNC_SEL);
    if (!(tmp & (1 << 15))) {
        printf("altfunc_sel does not select GPIOA !!!!\n");
        goto EXIT;
    }

    if (gpioa_no == 11) {
        if (!(tmp & (1 << 20))) {
            printf("altfunc_sel does not select GPIOA !!!!\n");
            goto EXIT;
        }
    }

    if (((readl(GPIOA_OE) >> gpioa_no) & 0x01) != 1) {
        printf("GPIO%02d is GP_OUT mode!!!!\n", gpioa_no);
        goto EXIT;
    }

    writel((1 << gpioa_no), GPIOA_CLEAR);

EXIT:
    return 0;
}

static int gpioa_in_nbits(unsigned int gpioa_nbits)
{
    unsigned int func_sel = 0;

    func_sel = readl(GPIO_ALTFUNC_SEL) | (1 << 15);
    if (gpioa_nbits & (1 << 11))
        func_sel |= (1 << 20);

    writel(func_sel, GPIO_ALTFUNC_SEL);

    writel(readl(GPIOA_OE) & ~gpioa_nbits, GPIOA_OE);
    writel(readl(GPIOA_RED) | gpioa_nbits, GPIOA_RED);
    writel(readl(GPIOA_FED) | gpioa_nbits, GPIOA_FED);
    writel(gpioa_nbits, GPIOA_RED_STATUS);
    writel(gpioa_nbits, GPIOA_FED_STATUS);

    return 0;
}

static int gpioa_out_nbits(unsigned int gpioa_nbits)
{
    unsigned int func_sel = 0;

    func_sel = readl(GPIO_ALTFUNC_SEL) | (1 << 15);
    if (gpioa_nbits & (1 << 11))
        func_sel |= (1 << 20);

    writel(func_sel, GPIO_ALTFUNC_SEL);

    writel(readl(GPIOA_OE) | gpioa_nbits, GPIOA_OE);
    writel(readl(GPIOA_RED) | gpioa_nbits, GPIOA_RED);
    writel(readl(GPIOA_FED) | gpioa_nbits, GPIOA_FED);
    writel(gpioa_nbits, GPIOA_RED_STATUS);
    writel(gpioa_nbits, GPIOA_FED_STATUS);

    return 0;
}

static int gpioa_release_nbits(unsigned int gpioa_nbits)
{
    writel(readl(GPIOA_RED) & ~gpioa_nbits, GPIOA_RED);
    writel(readl(GPIOA_FED) & ~gpioa_nbits, GPIOA_FED);
    writel(gpioa_nbits, GPIOA_RED_STATUS);
    writel(gpioa_nbits, GPIOA_FED_STATUS);

    return 0;
}

static int gpioa_in(unsigned int gpioa_no)
{
    unsigned int func_sel = 0;

    func_sel = readl(GPIO_ALTFUNC_SEL) | (1 << 15);
    if (gpioa_no == 11)
        func_sel |= (1 << 20);

    writel(func_sel, GPIO_ALTFUNC_SEL);

    writel(readl(GPIOA_OE) & ~(1 << gpioa_no), GPIOA_OE);
    writel(readl(GPIOA_RED) | (1 << gpioa_no), GPIOA_RED);
    writel(readl(GPIOA_FED) | (1 << gpioa_no), GPIOA_FED);
    writel((1 << gpioa_no), GPIOA_RED_STATUS);
    writel((1 << gpioa_no), GPIOA_FED_STATUS);

    return 0;
}

static int gpioa_out(unsigned int gpioa_no)
{
    unsigned int func_sel = 0;

    func_sel = readl(GPIO_ALTFUNC_SEL) | (1 << 15);
    if (gpioa_no == 11)
        func_sel |= (1 << 20);

    writel(func_sel, GPIO_ALTFUNC_SEL);

    writel(readl(GPIOA_OE) | (1 << gpioa_no), GPIOA_OE);
    writel(readl(GPIOA_RED) & ~(1 << gpioa_no), GPIOA_RED);
    writel(readl(GPIOA_FED) & ~(1 << gpioa_no), GPIOA_FED);
    writel((1 << gpioa_no), GPIOA_RED_STATUS);
    writel((1 << gpioa_no), GPIOA_FED_STATUS);

    return 0;
}

static int gpioa_set_drive(unsigned int gpioa_nbits)
{
    writel(readl(GPIOA_MODE) | gpioa_nbits, GPIOA_MODE);

    return 0;
}

static int gpioa_set_trigger(unsigned int gpioa_nbits)
{
    writel(readl(GPIOA_MODE) & ~gpioa_nbits, GPIOA_MODE);

    return 0;
}

static int gpioa_drive(unsigned int val)
{
    writel(val, GPIOA_DRIVE);

    return 0;
}

static int sh_gpioa_read(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc = 0;

    rc = gpioa_read();

    return rc;
}

static int sh_gpioa_set(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_set);
        rc = -1;
        goto EXIT;
    }

    gpioa_no = atoi(argv[1]);
    if (gpioa_no < 0 || gpioa_no > 11) {
        printf("Error GPIOA number %d !! (must be 0 ~ 11)\n", gpioa_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_set(gpioa_no);

EXIT:
    return rc;
}

static int sh_gpioa_clear(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_clear);
        rc = -1;
        goto EXIT;
    }

    gpioa_no = atoi(argv[1]);
    if (gpioa_no < 0 || gpioa_no > 11) {
        printf("Error GPIOA number %d !! (must be 0 ~ 11)\n", gpioa_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_clear(gpioa_no);

EXIT:
    return rc;
}

static int sh_gpioa_in(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_in);
        rc = -1;
        goto EXIT;
    }

    gpioa_no = atoi(argv[1]);
    if (gpioa_no < 0 || gpioa_no > 11) {
        printf("Error GPIOA number %d !! (must be 0 ~ 11)\n", gpioa_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_in(gpioa_no);

EXIT:
    return rc;
}

static int sh_gpioa_out(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_out);
        rc = -1;
        goto EXIT;
    }

    gpioa_no = atoi(argv[1]);
    if (gpioa_no < 0 || gpioa_no > 11) {
        printf("Error GPIOA number %d !! (must be 0 ~ 11)\n", gpioa_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_out(gpioa_no);

EXIT:
    return rc;
}

static int sh_gpioa_in_nbits(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_nbits = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_in_nbits);
        rc = -1;
        goto EXIT;
    }

    gpioa_nbits = strtol(argv[1], NULL, 16);
    rc = gpioa_in_nbits(gpioa_nbits);

EXIT:
    return rc;
}

static int sh_gpioa_out_nbits(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_nbits = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_out_nbits);
        rc = -1;
        goto EXIT;
    }

    gpioa_nbits = strtol(argv[1], NULL, 16);
    rc = gpioa_out_nbits(gpioa_nbits);

EXIT:
    return rc;
}

static int sh_gpioa_set_drive(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_nbits = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_set_drive);
        rc = -1;
        goto EXIT;
    }

    gpioa_nbits = strtol(argv[1], NULL, 16);
    rc = gpioa_set_drive(gpioa_nbits);

EXIT:
    return rc;
}

static int sh_gpioa_drive(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int val = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpioa_drive);
        rc = -1;
        goto EXIT;
    }

    val = strtol(argv[1], NULL, 16);
    rc = gpioa_drive(val);

EXIT:
    return rc;
}



static char dev_id = 0;
static char irq_flag = 0;
static int gpioa_level = 0, gpioa_red = 0, gpioa_fed = 0;

void gpioa_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
    gpioa_level = readl(GPIOA_LEVEL);
    gpioa_red = readl(GPIOA_RED_STATUS);
    gpioa_fed = readl(GPIOA_FED_STATUS);

    writel(gpioa_red, GPIOA_RED_STATUS);
    writel(gpioa_fed, GPIOA_FED_STATUS);

    irq_flag = 1;
    
    printf("[gpioa_irq_handler] Input value : 0x%08X\n", gpioa_level);
    printf("[gpioa_irq_handler] Rising edge status : 0x%08X\n", gpioa_red);
    printf("[gpioa_irq_handler] Falling edge status : 0x%08X\n", gpioa_fed);
}

static int gpioa_isr_init(void)
{
    int rc = 0;

    rc = request_irq(IRQ_GPIO, gpioa_irq_handler, SA_SHIRQ, "GPIO", &dev_id);
    if (rc < 0) {
        printf("setup_irq(IRQ_GPIO), rc=%d\n", rc);
        rc = -1;
        goto EXIT;
    }

    return 0;

EXIT:
    return rc;
}

static int gpioa_isr_release(void)
{
    int rc = 0;

    free_irq(IRQ_GPIO, &dev_id);
    disable_irq(IRQ_GPIO);

    return rc;
}

static unsigned int gpioa_bit_mask = 0xFFF;
static unsigned int gpioa_lbits = 0x03F;
static unsigned int gpioa_mbits = 0xFC0;
static int sh_gpioa_test_trigger(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_no = 0;
    int i = 0;

    gpioa_isr_init();

    for (gpioa_no = 0; gpioa_no <= 11; gpioa_no++) {
        writel(readl(GPIOA_INT_MASK) & ~(1 << gpioa_no), GPIOA_INT_MASK);
    }

    printf("\n==== OUT : GPIOA[5:0] , IN : GPIOA[11:6] ====\n");
    gpioa_out_nbits(gpioa_lbits);
    gpioa_in_nbits(gpioa_mbits);
    gpioa_set_trigger(gpioa_lbits);

    for (i = 0; i < 6; i++) { /* pull high GPIOA00 ~ 05 */
        printf("\n>>>> Pull high GPIOA%02d\n", i);
        gpioa_set(i);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;

        if ((gpioa_red >> 6) != (1 << i)) {
            printf("Rising status error !!\n");
            goto EXIT;
        }

        if ((gpioa_fed >> 6) != 0) {
            printf("Falling status error !!\n");
            goto EXIT;
        }
    }

    for (i = 0; i < 6; i++) { /* pull low GPIOA00 ~ 05 */
        printf("\n>>>> Pull low GPIOA%02d\n", i);
        gpioa_clear(i);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;

        if ((gpioa_fed >> 6) != (1 << i)) {
            printf("Rising status error !!\n");
            goto EXIT;
        }

        if ((gpioa_red >> 6) != 0) {
            printf("Falling status error !!\n");
            goto EXIT;
        }
    }

    printf("\n==== OUT : GPIOA[11:6] , IN : GPIOA[5:0] ====\n");
    gpioa_out_nbits(gpioa_mbits);
    gpioa_in_nbits(gpioa_lbits);
    gpioa_set_trigger(gpioa_mbits);

    for (i = 6; i < 12; i++) { /* pull high GPIOA06 ~ 11 */
        printf("\n>>>> Pull high GPIOA%02d\n", i);
        gpioa_set(i);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;

        if ((gpioa_red & gpioa_lbits) != (1 << (i - 6))) {
            printf("Rising status error !!\n");
            goto EXIT;
        }

        if ((gpioa_fed & gpioa_lbits) != 0) {
            printf("Falling status error !!\n");
            goto EXIT;
        }
    }

    for (i = 6; i < 12; i++) { /* pull low GPIOA06 ~ 11 */
        printf("\n>>>> Pull low GPIOA%02d\n", i);
        gpioa_clear(i);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;

        if ((gpioa_fed & gpioa_lbits) != (1 << (i - 6))) {
            printf("Rising status error !!\n");
            goto EXIT;
        }

        if ((gpioa_red & gpioa_lbits) != 0) {
            printf("Falling status error !!\n");
            goto EXIT;
        }
    }

EXIT:
    gpioa_release_nbits(gpioa_bit_mask);
    gpioa_isr_release();

    return 0;
}

static int sh_gpioa_test_drive(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpioa_no = 0;
    unsigned int val = 0;
    int i = 0, j = 0;

    gpioa_isr_init();

    for (gpioa_no = 0; gpioa_no <= 11; gpioa_no++) {
        writel(readl(GPIOA_INT_MASK) & ~(1 << gpioa_no), GPIOA_INT_MASK);
    }

    printf("\n==== OUT : GPIOA[5:0] , IN : GPIOA[11:6] ====\n");
    gpioa_out_nbits(gpioa_lbits);
    gpioa_set_drive(gpioa_lbits);
    gpioa_in_nbits(gpioa_mbits);

    for (i = 5, j = 0; i >= 0; i--, j++) {
        val = 0x3F >> i;
        printf("\n>>>> Drive value 0x%02X\n", val);
        gpioa_drive(val);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;
        if (gpioa_level != (val | (val << 6))) {
            printf("Level error !!\n");
            goto EXIT;
        }

        val = (1 << j);
        if (gpioa_red != (val | (val << 6))) {
            printf("Rising status error !!\n");
            goto EXIT;
        }

        if (gpioa_fed != 0) {
            printf("Falling status error !!\n");
            goto EXIT;
        }
    }

    for (i = 1, j = 0; i <= 6; i++, j++) {
        val = 0x3F >> i;
        printf("\n>>>> Drive value 0x%02X\n", val);
        gpioa_drive(val);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;
        if (gpioa_level != (val | (val << 6))) {
            printf("Level error !!\n");
            goto EXIT;
        }

        val = (0x20 >> j);
        if (gpioa_fed != (val | (val << 6))) {
            printf("Falling status error !!\n");
            goto EXIT;
        }

        if (gpioa_red != 0) {
            printf("Rising status error !!\n");
            goto EXIT;
        }
    }

    printf("\n==== OUT : GPIOA[11:6] , IN : GPIOA[5:0] ====\n");
    gpioa_out_nbits(gpioa_mbits);
    gpioa_set_drive(gpioa_mbits);
    gpioa_in_nbits(gpioa_lbits);

    for (i = 5, j = 0; i >= 0; i--, j++) {
        val = 0x3F >> i;
        printf("\n>>>> Drive value 0x%02X\n", val);
        gpioa_drive(val << 6);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;
        if (gpioa_level != (val | (val << 6))) {
            printf("Level error !!\n");
            goto EXIT;
        }

        val = (1 << j);
        if (gpioa_red != (val | (val << 6))) {
            printf("Rising status error !!\n");
            goto EXIT;
        }

        if (gpioa_fed != 0) {
            printf("Falling status error !!\n");
            goto EXIT;
        }
    }

    for (i = 1, j = 0; i <= 6; i++, j++) {
        val = 0x3F >> i;
        printf("\n>>>> Drive value 0x%02X\n", val);
        gpioa_drive(val << 6);
        while (!irq_flag)
            printf("(wait interrupt ...)\n");

        irq_flag = 0;
        if (gpioa_level != (val | (val << 6))) {
            printf("Level error !!\n");
            goto EXIT;
        }

        val = (0x20 >> j);
        if (gpioa_fed != (val | (val << 6))) {
            printf("Falling status error !!\n");
            goto EXIT;
        }

        if (gpioa_red != 0) {
            printf("Rising status error !!\n");
            goto EXIT;
        }
    }

EXIT:
    gpioa_release_nbits(gpioa_bit_mask);
    gpioa_isr_release();

    return 0;
}

