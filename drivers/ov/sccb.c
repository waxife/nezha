/* 
 *  @file   sccb.c (derived from i2c.c)
 *  @brief  SCCB driver APIs
 *
 *  $Id: sccb.c,v 1.2 2014/02/05 08:00:21 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *  @date   2010/09/16  Hugo    New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <io.h>
#include <errno.h>
#include <debug.h>

#include "sccb.h"

/*
 * GPIO operation functions.
 */

#define SET_SCCB_CLK_HIGH       writel (GPIO_SIO_C, GPIO_SET)
#define SET_SCCB_CLK_LOW        writel (GPIO_SIO_C, GPIO_CLEAR)
#define SET_SCCB_DATA_HIGH      writel (GPIO_SIO_D, GPIO_SET)
#define SET_SCCB_DATA_LOW       writel (GPIO_SIO_D, GPIO_CLEAR)
#define SET_SCCB_DATA_INPUT     writel (readl (GPIO_OE) & ~GPIO_SIO_D, GPIO_OE)
#define SET_SCCB_DATA_OUTPUT    writel (readl (GPIO_OE) | GPIO_SIO_D, GPIO_OE)
#define GET_SCCB_DATA_BIT       ((readl (GPIO_LEVEL) & GPIO_SIO_D) ? 1 : 0)

/*
 * SCCB operation functions.
 */

#define SCCB_DLY                usleep(3)

static void
sccb_transmission_start (void)
{
    SET_SCCB_DATA_OUTPUT;

    /* sccb start bit sequence */
    SET_SCCB_DATA_HIGH;
    SET_SCCB_CLK_HIGH;
    SCCB_DLY;
    SET_SCCB_DATA_LOW;
    SCCB_DLY;
    SET_SCCB_CLK_LOW;
    SCCB_DLY;
}

static void
sccb_transmission_stop (void)
{
    SET_SCCB_DATA_OUTPUT;

    /* sccb stop bit sequence */
    SET_SCCB_CLK_LOW;
    SET_SCCB_DATA_LOW;
    SCCB_DLY;
    SET_SCCB_CLK_HIGH;
    SCCB_DLY;
    SET_SCCB_DATA_HIGH;
    SCCB_DLY;
}

static int
sccb_send_byte (unsigned char byte)
{
    int i;

    /* data bits */
    for (i = 7; i >= 0; i--) {
        if (byte & (1 << i))
            SET_SCCB_DATA_HIGH;
        else
            SET_SCCB_DATA_LOW;
        SCCB_DLY;
        SET_SCCB_CLK_HIGH;
        SCCB_DLY;
        SET_SCCB_CLK_LOW;
        SCCB_DLY;
    }

    /* don't care bit */
    SET_SCCB_DATA_INPUT;
    SCCB_DLY;
    SET_SCCB_CLK_HIGH;
    SCCB_DLY;
    SET_SCCB_CLK_LOW;
    SCCB_DLY;
    SET_SCCB_DATA_OUTPUT;

    return 0;
}

static unsigned char
sccb_recv_byte (void)
{
    int i, byte = 0;

    SET_SCCB_DATA_INPUT;

    /* data bits */
    for (i = 7; i >= 0; i--) {
        SCCB_DLY;
        SET_SCCB_CLK_HIGH;
        SCCB_DLY;
        if (GET_SCCB_DATA_BIT)
            byte |= (1 << i);
        SET_SCCB_CLK_LOW;
        SCCB_DLY;
    }

    /* write N/A bit */
    SET_SCCB_DATA_OUTPUT;
    SET_SCCB_DATA_HIGH;
    SCCB_DLY;
    SET_SCCB_CLK_HIGH;
    SCCB_DLY;
    SET_SCCB_CLK_LOW;
    SCCB_DLY;

    return byte;
}

/* sccb functions end. */

int
sccb_write (unsigned char reg, unsigned char val)
{
    int rc;

    /*
     * 3-Phase Write Transmission Cycle
     */

    /* start of transmission */
    sccb_transmission_start ();

    /* phase 1 - IP address with I/O direction (WRITE) */
    rc = sccb_send_byte (SCCB_ADDR_W);
    if (rc != 0)
        return -1;

    /* phase 2 - Sub-address (register) */
    rc = sccb_send_byte (reg);
    if (rc != 0)
        return -1;

    /* phase 3 - write data */
    rc = sccb_send_byte (val);
    if (rc != 0)
        return -1;

    /* stop of transmission */
    sccb_transmission_stop ();

    return 0;
}

int
sccb_read (unsigned char reg, unsigned char *val)
{
    int rc;

    /*
     * 2-Phase Write Transmission Cycle
     */

    /* start of tranmission */
    sccb_transmission_start ();

    /* phase 1 - IP address with I/O direction (WRITE) */
    rc = sccb_send_byte (SCCB_ADDR_W);
    if (rc != 0)
        return -1;

    /* phase 2 - Sub-address (register) */
    rc = sccb_send_byte (reg);
    if (rc != 0)
        return -1;

    /* stop of tranmission */
    sccb_transmission_stop ();

    /*
     * 2-Phase Read Transmission Cycle
     */

    /* start of tranmission */
    sccb_transmission_start ();

    /* phase 1 - IP address with I/O direction (READ) */
    rc = sccb_send_byte (SCCB_ADDR_R);
    if (rc != 0)
        return -1;

    /* phase 2 - read data */
    *val = sccb_recv_byte ();

    /* stop of transmission */
    sccb_transmission_stop ();

    return 0;
}

int
sccb_init (void)
{
    /* Setup GPIO multiplex */
    //Config pin mux function at VAL_SYSMGM_ALTFUNC_SEL in sysclock.c by Green Tool.
#if 0      
    /* setup output enabled to 1 for default output */
    writel (readl (GPIO_OE) | (GPIO_SIO_C | GPIO_SIO_D), GPIO_OE);

    /* setup gpio level */
    writel ((GPIO_SIO_C | GPIO_SIO_D), GPIO_SET);
#else
    dbg(0,"Please select another gpio to drive sio in T582!\n");
    dbg(0,"T582 SDK default not support SPI now!\n");
#endif	
    return 0;
}
