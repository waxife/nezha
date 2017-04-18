/* 
 *  @file   sccb.c (derived from i2c.c)
 *  @brief  SCCB driver APIs
 *
 *  $Id: spi.c,v 1.2 2014/02/05 08:00:21 kevin Exp $
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

#include "spi.h"

/*
 * GPIO operation functions.
 */

#define SET_SPI_SCL_HIGH     writel (GPIO_SCL, GPIO_SET)
#define SET_SPI_SCL_LOW      writel (GPIO_SCL, GPIO_CLEAR)
#define SET_SPI_SDA_HIGH     writel (GPIO_SDA, GPIO_SET)
#define SET_SPI_SDA_LOW      writel (GPIO_SDA, GPIO_CLEAR)
#define SET_SPI_CS_HIGH      writel (GPIO_CS, GPIO_SET)
#define SET_SPI_CS_LOW       writel (GPIO_CS, GPIO_CLEAR)
#define SET_SPI_SCL_OUTPUT   writel (readl (GPIO_OE) | GPIO_SCL, GPIO_OE)
#define SET_SPI_SDA_OUTPUT   writel (readl (GPIO_OE) | GPIO_SDA, GPIO_OE)
#define SET_SPI_CS_OUTPUT    writel (readl (GPIO_OE) | GPIO_CS , GPIO_OE)

#define SPI_TIME_DELAY      10 
#define spi_latch_delay(i, time) {for (i=0; i<time; i++);}


static void spi_write_8bit(unsigned char bData)
{
	int i, j;
	for (i=0; i<8; i++)
	{
	    SET_SPI_SCL_LOW;
		if (bData & 0x80) 
		    SET_SPI_SDA_HIGH;
		else 
		    SET_SPI_SDA_LOW;
		spi_latch_delay(j, SPI_TIME_DELAY);
		SET_SPI_SCL_HIGH;
		spi_latch_delay(j, SPI_TIME_DELAY);
		
		bData <<= 1;
	}
}

void spi_write(unsigned int bAdd, unsigned int bData)
{
    int j;
	/* enable Chip select, neg active */
	SET_SPI_CS_LOW;
    spi_latch_delay(j, SPI_TIME_DELAY);
	// write address	
	spi_write_8bit((unsigned char)bAdd);
	// write value
	spi_write_8bit((unsigned char)bData);

	// release CS/SCL/SDA
	SET_SPI_CS_HIGH;
	SET_SPI_SCL_HIGH;
	SET_SPI_SDA_LOW;
	
}

void spi_open(void)
{

    /*Set pin mux*/
    //Config pin mux function at VAL_SYSMGM_ALTFUNC_SEL in sysclock.c by Green Tool.
#if 0    
	/* Config GPIO mode: output mode */
    SET_SPI_SCL_OUTPUT;
    SET_SPI_SDA_OUTPUT;
    SET_SPI_CS_OUTPUT; 

	// park SPI_CS on high level (chip_sel disable) & clear SPI_SCL/SPI_SDA
	SET_SPI_CS_HIGH;
	SET_SPI_SCL_HIGH;
	SET_SPI_SDA_LOW;
#else
    dbg(0,"Please select another gpio to drive spi in T582!\n");
    dbg(0,"T582 SDK default not support SPI now!\n");
#endif	
}

