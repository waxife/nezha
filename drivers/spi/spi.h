/* 
 *  @file   sccb.h (derived from i2c.h)
 *  @brief  SCCB header file
 *
 *  $Id: spi.h,v 1.2 2014/02/05 08:00:21 kevin Exp $
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
/*Please select another gpio to control spi in T582!!!!!!! */
#define GPIO_SCL  (1 << 3)   // GPIO Pin for SCL
#define GPIO_SDA  (1 << 2)   // GPIO Pin for SDA
#define GPIO_CS   (1 << 14)   // GPIO Pin for CS
/***********************************************************/

void spi_write(unsigned int bAdd, unsigned int bData);
void spi_open(void);

#endif /* __SCCB_H */
