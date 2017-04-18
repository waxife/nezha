/**
 *  @file   sh_gpio.c
 *  @brief  GPIO DRIVER
 *  $Id: gpio.c,v 1.4 2014/06/23 09:10:32 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/06/10  ycshih    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
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
#include <debug.h>

unsigned int altfunc_sel_gpio[32] = {
    0, 0, 0, 0, 0, /* GPIO00 ~ 04 */
    (1 << 5), (1 << 5), /* GPIO05 ~ 06 */
    (1 << 14), (1 << 14), /* GPIO07 ~ 08 */
    (1 << 4), (1 << 4), /* GPIO09 ~ 10 */
    (1 << 9), /* GPIO11 */
    0, 0, 0, /* GPIO12 ~ 14 */
    (1 << 0), /* GPIO15 */
    (2 << 2), /* GPIO16 */
    (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), /* GPIO17 ~ 24 */
    (3 << 6), /* GPIO25 */
    (2 << 2), (2 << 2), (2 << 2), (2 << 2), (2 << 2), (2 << 2) /* GPIO26 ~ 31 */
};

unsigned int altfunc_sel_gpio_mask[32] = {
    (1 << 10), (1 << 10), (1 << 10), (1 << 10), (1 << 10), /* GPIO00 ~ 04 */
    (1 << 5), (1 << 5), /* GPIO05 ~ 06 */
    (1 << 14), (1 << 14), /* GPIO07 ~ 08 */
    (1 << 4), (1 << 4), /* GPIO09 ~ 10 */
    (1 << 9), /* GPIO11 */
    0, 0, 0, /* GPIO12 ~ 14 */
    (1 << 0), /* GPIO15 */
    (3 << 2), /* GPIO16 */
    (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), (1 << 11), /* GPIO17 ~ 24 */
    (3 << 6), /* GPIO25 */
    (3 << 2), (3 << 2), (3 << 2), (3 << 2), (3 << 2), (3 << 2) /* GPIO26 ~ 31 */
};

unsigned int GPIO_INT_FLAG = 0;
unsigned int GPIOA_INT_FLAG = 0;

int gpio_read(unsigned int gpio_no)
{
    unsigned int val = 0;
    val = readl(GPIO_LEVEL);
    if (((val >> gpio_no) & 0x01) == 1) {
		return 1;
    }
	else{
		return 0;
	}
}

int gpioa_read(unsigned int gpioa_no)
{
    unsigned int val = 0;
    val = readl(GPIOA_LEVEL);
	if (((val >> gpioa_no) & 0x01) == 1) {
		return 1;
    }
	else{
		return 0;
	}
}

int gpio_set(unsigned int gpio_no)
{
    unsigned int tmp = 0;
    tmp = readl(GPIO_ALTFUNC_SEL);
    tmp &= altfunc_sel_gpio_mask[gpio_no];
    if (tmp != altfunc_sel_gpio[gpio_no]) {
        goto EXIT;
    }


    if (((readl(GPIO_OE) >> gpio_no) & 0x01) != 1) {
            goto EXIT;
    }
    writel(1 << gpio_no, GPIO_SET);
  
	return 1;
EXIT:
    return 0;
}

int gpioa_set(unsigned int gpioa_no)
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
	return 1;
EXIT:
    return 0;
}

int gpio_clear(unsigned int gpio_no)
{
    unsigned int tmp = 0;
    tmp = readl(GPIO_ALTFUNC_SEL);
    tmp &= altfunc_sel_gpio_mask[gpio_no];
    if (tmp != altfunc_sel_gpio[gpio_no]) {
        goto EXIT;
    }

	if (((readl(GPIO_OE) >> gpio_no) & 0x01) != 1) {
		goto EXIT;
	}
    writel((1 << gpio_no), GPIO_CLEAR);
	
	return 1;
EXIT:
    return 0;
}

int gpioa_clear(unsigned int gpioa_no)
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
	return 1;
EXIT:
    return 0;
}

int gpio_in(unsigned int gpio_no)
{
    unsigned int tmp = 0;
	
	tmp = readl(GPIO_ALTFUNC_SEL);
    tmp &= altfunc_sel_gpio_mask[gpio_no];
    if (tmp != altfunc_sel_gpio[gpio_no]) {
        goto EXIT;
    }
    writel(readl(GPIO_OE) & ~(1 << gpio_no), GPIO_OE);
	return 1;
EXIT:
	return 0;
}

int gpioa_in(unsigned int gpioa_no)
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
    writel(readl(GPIOA_OE) & ~(1 << gpioa_no), GPIOA_OE);
	return 1;
EXIT:
	return 0;
}

int gpio_out(unsigned int gpio_no)
{
    unsigned int tmp = 0;
	
    tmp = readl(GPIO_ALTFUNC_SEL);
    tmp &= altfunc_sel_gpio_mask[gpio_no];
    if (tmp != altfunc_sel_gpio[gpio_no]) {
        goto EXIT;
    }
    writel(readl(GPIO_OE) | (1 << gpio_no), GPIO_OE);
	return 1;
EXIT:
	return 0;
}

int gpioa_out(unsigned int gpioa_no)
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
    writel(readl(GPIOA_OE) | (1 << gpioa_no), GPIOA_OE);
	return 1;
EXIT:
	return 0;
}

int gpio_int_rising(unsigned int gpio_no)
{
    unsigned int tmp = 0;
	
    tmp = readl(GPIO_ALTFUNC_SEL);
    tmp &= altfunc_sel_gpio_mask[gpio_no];
    if (tmp != altfunc_sel_gpio[gpio_no]) {
        goto EXIT;
    }
	if (((readl(GPIO_OE) >> gpio_no) & 0x01) == 1) {
            goto EXIT;
    }
	gpio_in(gpio_no);
	writel(readl(GPIO_RED) | (1 << gpio_no), GPIO_RED);
	writel(readl(GPIO_INT_MASK) & ~(1 << gpio_no), GPIO_INT_MASK);
	return 1;
EXIT:
	return 0;
}

int gpioa_int_rising(unsigned int gpioa_no)
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
	gpioa_in(gpioa_no);
	writel(readl(GPIOA_RED) | (1 << gpioa_no), GPIOA_RED);
	writel(readl(GPIOA_INT_MASK) & ~(1 << gpioa_no), GPIOA_INT_MASK);
	return 1;
EXIT:
	return 0;
}

int gpio_int_falling(unsigned int gpio_no)
{
    unsigned int tmp = 0;
	
    tmp = readl(GPIO_ALTFUNC_SEL);
    tmp &= altfunc_sel_gpio_mask[gpio_no];
    if (tmp != altfunc_sel_gpio[gpio_no]) {
        goto EXIT;
    }
	gpio_in(gpio_no);
    writel(readl(GPIO_FED) | (1 << gpio_no), GPIO_FED);
	writel(readl(GPIO_INT_MASK) & ~(1 << gpio_no), GPIO_INT_MASK);
	return 1;
EXIT:
	return 0;
}

int gpioa_int_falling(unsigned int gpioa_no)
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
	gpioa_in(gpioa_no);
    writel(readl(GPIOA_FED) | (1 << gpioa_no), GPIOA_FED);
	writel(readl(GPIOA_INT_MASK) & ~(1 << gpioa_no), GPIOA_INT_MASK);
	return 1;
EXIT:
	return 0;
}

static void gpio_irq_handler(int irq, void *dev_id, struct pt_regs *regs)
{
    unsigned int red, fed;
	unsigned int red_a, fed_a;

    red = readl(GPIO_RED_STATUS);
    fed = readl(GPIO_FED_STATUS);

    writel(red, GPIO_RED_STATUS);
    writel(fed, GPIO_FED_STATUS);
	
    red_a = readl(GPIOA_RED_STATUS);
    fed_a = readl(GPIOA_FED_STATUS);

    writel(red_a, GPIOA_RED_STATUS);
    writel(fed_a, GPIOA_FED_STATUS);
	
	GPIO_INT_FLAG |= red;
	GPIO_INT_FLAG |= fed;
	
	GPIOA_INT_FLAG |= red_a;
	GPIOA_INT_FLAG |= fed_a;
	
}

unsigned char check_gpio_event (unsigned int* flags)
{
	unsigned char i= 0;
	unsigned char rc= 0;
	*flags = 0;

	for(i= 0; i< 32; i++) {
		if(GPIO_INT_FLAG&(1<<i)) {
			rc= 1;	// break for just one flag
			break;
		}
	}

	if(rc){	// get interrupt flags
		*flags  |= GPIO_INT_FLAG;
	}
	GPIO_INT_FLAG = 0;
	return rc;
}

unsigned char check_gpioa_event (unsigned int* flags)
{
	unsigned char i= 0;
	unsigned char rc= 0;
	*flags = 0;

	for(i= 0; i< 16; i++) {
		if(GPIOA_INT_FLAG&(1<<i)) {
			rc= 1;	// break for just one flag
			break;
		}
	}

	if(rc){	// get interrupt flags
		*flags  |= GPIOA_INT_FLAG;
	}
	GPIOA_INT_FLAG = 0;
	return rc;
}

int gpio_isr_open(void)
{
	int rc=0;
	static struct irqaction gpio;
	gpio.handler = gpio_irq_handler;
    gpio.flags = SA_INTERRUPT;
    gpio.mask = 0;
    gpio.name = "gpioint";
    gpio.next = NULL;
    gpio.dev_id = NULL;
	rc = setup_irq(IRQ_GPIO,&gpio);
	if(rc<0)
	{
		printf("gpio_open setup_irq IRQ_SCALER  ERROR\r\n");
		goto EXIT;
	}
	return 0;
EXIT:
	disable_irq(IRQ_GPIO);
    free_irq(IRQ_GPIO,NULL);
	return -1;
}

void gpio_isr_close(void)
{
	writel(0xffffffff, GPIO_INT_MASK);
	writel(0xffffffff, GPIOA_INT_MASK);
	disable_irq(IRQ_GPIO);
    free_irq(IRQ_GPIO,NULL);
}
