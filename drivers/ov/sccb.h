/* 
 *  @file   sccb.h (derived from i2c.h)
 *  @brief  SCCB header file
 *
 *  $Id: sccb.h,v 1.2 2014/02/05 08:00:21 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *  @date   2010/09/16  Hugo    New file.
 *
 */

#ifndef __SCCB_H
#define __SCCB_H

#include <gpio.h>

/* GPIO operation functions */
/*Please select another gpio to control sio in T582!!!!!!! */
#define GPIO_SIO_C  (1 << 3)   // GPIO Pin for SIO_C
#define GPIO_SIO_D  (1 << 2)   // GPIO Pin for SIO_D
/***********************************************************/

/* SCCB Device Slave Address */
#define SCCB_ADDR_W 0x42        // OV7670 slave address for write
#define SCCB_ADDR_R 0x43        // OV7670 slave address for read


/*
 * Synopsis     int sccb_write (unsigned char reg, unsigned char val);
 * Description  write value to a register of SCCB slave
 * Parameters   reg - OV camera chip control register address
 *              val - value to set
 * Return       0 for success, or -1 for any error
 */
int sccb_write (unsigned char reg, unsigned char val);

/*
 * Synopsis     int sccb_read (unsigned char reg, unsigned char *val);
 * Description  read value from a register of SCCB slave
 * Parameters   reg - OV camera chip control register address
 *              val - value to get
 * Return       0 for success, or -1 for any error
 */
int sccb_read (unsigned char reg, unsigned char *val);

/*
 * Synopsis     int sccb_init (void);
 * Description  setup sccb communication environment
 * Parameters   none
 * Return       0 for success, or -1 for any error
 */
int sccb_init (void);

#endif /* __SCCB_H */
