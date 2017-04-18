/**
 *  @file   gpio.h
 *  @brief  regiters defined for gpio IP
 *  $Id: gpio.h,v 1.5 2014/06/23 09:09:57 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/26  jedy    New file.
 *
 */

#ifndef __GPIO_H
#define __GPIO_H

#include <sys.h>

#define     GPIO_LEVEL          (GPIO_BASE + 0x00)
#define     GPIO_OE             (GPIO_BASE + 0x04)
#define     GPIO_SET            (GPIO_BASE + 0x08)
#define     GPIO_CLEAR          (GPIO_BASE + 0x0c)
#define     GPIO_RED            (GPIO_BASE + 0x10)
#define     GPIO_FED            (GPIO_BASE + 0x14)
#define     GPIO_RED_STATUS     (GPIO_BASE + 0x18)
#define     GPIO_FED_STATUS     (GPIO_BASE + 0x1c)
#define     GPIO_INT_MASK       (GPIO_BASE + 0x24)
#define     GPIO_ALTFUNC_SEL    (GPIO_BASE + 0x28)
#define     GPIO_HW_TRAP_FF     (GPIO_BASE + 0x2c)
#define     GPIO_SP_0           (GPIO_BASE + 0x30)
#define     GPIO_SP_1           (GPIO_BASE + 0x34)
#define     GPIO_SP_2           (GPIO_BASE + 0x38)
#define     GPIO_CARD_SEL       (GPIO_BASE + 0x3c)

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

#define     HW_NOT_I2S_SEL      (1<<4)              /* 1 : GPIO */
#define     HW_NOT_UART_SEL     (1<<3)              /* 1 : GPIO */
#define     HW_NOT_EJTAG_SEL    (1<<2)              /* 1 : GPIO */
#define     HW_SPI_NOR_SEL      (1<<1)              /* 1 : SPI NOR select */
#define     HW_NOR_SEL          (1<<0)              /* 1 : NOR select */

#define     CARD_SEL_CF         3
#define     CARD_SEL_SD         2
#define     CARD_SEL_MS         1
#define     CARD_SEL_XD         0


#define     XD_CARDDECT         (1 << 31)
#define     MS_CARDDECT         (1 << 30)
#define     SD_CARDDECT         (1 << 12)
#define     CF_CARDDECT         (1 << 28)

#define     SD_POWER            (1 << 27)
#define     MS_POWER            (1 << 27)
#define     CF_POWER            (1 << 26)

/**
 * @func      gpio_read
 * @brief     gpio read status
 * @param     gpio_no         
 * @return    0 low level, 1 high level.
 */
int gpio_read(unsigned int gpio_no);
/**
 * @func      gpio_set
 * @brief     gpio set as one
 * @param     gpio_no         
 * @return    1 successed, 0 fail.
 */
int gpio_set(unsigned int gpio_no);
/**
 * @func      gpio_clear
 * @brief     gpio set as zero
 * @param     gpio_no         
 * @return    1 successed, 0 fail.
 */
int gpio_clear(unsigned int gpio_no);
/**
 * @func      gpio_in
 * @brief     gpio set as input
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpio_in(unsigned int gpio_no);
/**
 * @func      gpio_out
 * @brief     gpio set as output
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpio_out(unsigned int gpio_no);

/**
 * @func      gpio_int_rising
 * @brief     gpio set as rising edge
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpio_int_rising(unsigned int gpio_no);
/**
 * @func      gpio_int_falling
 * @brief     gpio set as falling edge
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpio_int_falling(unsigned int gpio_no);
/**
 * @func      gpio_isr_open
 * @brief     gpio_isr_open interrupt open
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpio_isr_open(void);
/**
 * @func      gpio_isr_close
 * @brief     gpio_isr_open interrupt close
 * @param     none         
 * @return    1 successful 0 fail
 */
void gpio_isr_close(void);
/**
 * @func      check_gpio_event
 * @brief     gpio_check_event get return flag
 * @param     flags       
 * @return    1 successful 0 fail
 */
unsigned char check_gpio_event (unsigned int* flags);

//gpioa
/**
 * @func      gpio_read
 * @brief     gpio read status
 * @param     gpio_no         
 * @return    0 low level, 1 high level.
 */
int gpioa_read(unsigned int gpioa_no);
/**
 * @func      gpioa_set
 * @brief     gpioa set as one
 * @param     gpioa_no         
 * @return    1 successed, 0 fail.
 */
int gpioa_set(unsigned int gpioa_no);
/**
 * @func      gpioa_clear
 * @brief     gpioa set as zero
 * @param     gpioa_no         
 * @return    1 successed, 0 fail.
 */
int gpioa_clear(unsigned int gpioa_no);
/**
 * @func      gpioa_in
 * @brief     gpioa set as input
 * @param     gpioa_no         
 * @return    1 successful 0 fail
 */
int gpioa_in(unsigned int gpioa_no);
/**
 * @func      gpioa_out
 * @brief     gpioa set as output
 * @param     gpioa_no         
 * @return    1 successful 0 fail
 */
int gpioa_out(unsigned int gpioa_no);
/**
 * @func      gpio_int_rising
 * @brief     gpio set as rising edge
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpioa_int_rising(unsigned int gpioa_no);
/**
 * @func      gpio_int_falling
 * @brief     gpio set as falling edge
 * @param     gpio_no         
 * @return    1 successful 0 fail
 */
int gpioa_int_falling(unsigned int gpioa_no);
/**
 * @func      check_gpio_event
 * @brief     gpio_check_event get return flag
 * @param     flags       
 * @return    1 successful 0 fail
 */
unsigned char check_gpioa_event (unsigned int* flags);


#endif /* __GPIO_H */
