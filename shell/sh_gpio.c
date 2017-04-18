/**
 *  @file   sh_gpio.c
 *  @brief  A sample code of gpio operation
 *  $Id: sh_gpio.c,v 1.3 2014/06/23 09:10:51 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.3 $
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

command_init (sh_gpio_read, "gpio_read", "gpio_read [GPIO number]");
command_init (sh_gpio_set, "gpio_set", "gpio_set [GPIO number]");
command_init (sh_gpio_clear, "gpio_clear", "gpio_clear [GPIO number]");
command_init (sh_gpio_in, "gpio_in", "gpio_in [GPIO number]");
command_init (sh_gpio_out, "gpio_out", "gpio_out [GPIO number]");

command_init (sh_gpio_isr_set, "gpio_isr_set", "gpio_isr_set");
command_init (sh_gpio_isr_close, "gpio_isr_close", "gpio_isr_close");
command_init (sh_gpio_set_rising, "gpio_set_rising", "gpio_set_rising [GPIO number]");
command_init (sh_gpio_set_falling, "gpio_set_falling", "gpio_set_falling [GPIO number]");

command_init (sh_gpioa_read, "gpioa_read", "gpioa_read [GPIO number]");
command_init (sh_gpioa_set, "gpioa_set", "gpioa_set [GPIO number]");
command_init (sh_gpioa_clear, "gpioa_clear", "gpioa_clear [GPIO number]");
command_init (sh_gpioa_in, "gpioa_in", "gpioa_in [GPIO number]");
command_init (sh_gpioa_out, "gpioa_out", "gpioa_out [GPIO number]");

command_init (sh_gpioa_set_rising, "gpioa_set_rising", "gpioa_set_rising [GPIO number]");
command_init (sh_gpioa_set_falling, "gpioa_set_falling", "gpioa_set_falling [GPIO number]");

static int sh_gpio_read(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_read);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }
	
    rc = gpio_read(gpio_no);
	printf("gpio no %d value = %d\n",gpio_no,rc);
EXIT:
    return rc;
}

static int sh_gpio_set(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_set);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpio_set(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpio_clear(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_clear);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpio_clear(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpio_in(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_in);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 28)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpio_in(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpio_out(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_out);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpio_out(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpio_isr_set(int argc, char **argv, void **result, void *sys_ctx)
{
	int rc =0;
	printf("gpio open\r\n");
    rc = gpio_isr_open();
	if(rc<0){
		printf("gpio isr failed");
	}
    return 0;
}

static int sh_gpio_isr_close(int argc, char **argv, void **result, void *sys_ctx)
{
	printf("gpio irq close\r\n");
    gpio_isr_close();
	return 0;
}

static int sh_gpio_set_rising(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_clear);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpio_int_rising(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpio_set_falling(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_clear);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 31) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpio_int_falling(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpioa_read(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_read);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }
	
    rc = gpioa_read(gpio_no);
	printf("gpio no %d value = %d\n",gpio_no,rc);
EXIT:
    return rc;
}

static int sh_gpioa_set(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_set);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_set(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpioa_clear(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_clear);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_clear(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpioa_in(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_in);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 28)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_in(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpioa_out(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_out);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 31)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_out(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpioa_set_rising(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_out);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 10)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_int_rising(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}

static int sh_gpioa_set_falling(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned int gpio_no = 0;
    int rc = 0;

    if (argc < 2) {
        print_usage(sh_gpio_out);
        rc = -1;
        goto EXIT;
    }

    gpio_no = atoi(argv[1]);
    if (gpio_no < 0 || gpio_no > 11) {
        printf("Error GPIO number %d !! (must be 0 ~ 10)\n", gpio_no);
        rc = -1;
        goto EXIT;
    }

    rc = gpioa_int_falling(gpio_no);
	if(rc == 0)
		printf("The gpio no %d is not supported\n",gpio_no);

EXIT:
    return rc;
}
