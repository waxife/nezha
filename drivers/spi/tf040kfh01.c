/**
 *  @file   ili9320.c
 *  @brief  ili9320 interface
 *  $Id: tf040kfh01.c,v 1.1.1.1 2013/12/18 03:43:43 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2009 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/02/24  Ken  New file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <io.h>
#include <debug.h>

#include "spi.h"
#include "tf040kfh01.h"

void tf040kfh_init(void)
{
    printf("tf040kfh01 spi init\n");
    spi_open();
    spi_write(0x05,0x04);
    spi_write(0x00,0x07);
    spi_write(0x01,0xAD);
    spi_write(0x0B,0x80);
    spi_write(0x2F,0x7D);
    spi_write(0x16,0x00);
    spi_write(0x17,0x54);
    spi_write(0x18,0x54);
    spi_write(0x19,0x43);
    spi_write(0x1A,0x54);
    spi_write(0x2B,0x01);
    spi_write(0x0D,0x25);
       
}
