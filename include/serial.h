/**
 *  @file   serial.h
 *  @brief  header file for serial driver
 *  $Id: serial.h,v 1.4 2016/05/25 08:27:56 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */



#ifndef __SERIAL_H__
#define __SERIAL_H__

#define CLK_REG		(0x1b000000 | KSEG1)

#define SERIAL_REG_BASE	(0x1b000400 | KSEG1)
#define SERIAL_REG_ST	(SERIAL_REG_BASE + 0)
#define SERIAL_REG_DATA	(SERIAL_REG_BASE + 1)
#define SERIAL_REG_RST	(SERIAL_REG_BASE + 2)
#define SERIAL_REG_SRB	(SERIAL_REG_BASE + 3)
#define SERIAL_REG_PL	(SERIAL_REG_BASE + 4)
#define SERIAL_REG_PH	(SERIAL_REG_BASE + 5)
#define SERIAL_REG_CMP	(SERIAL_REG_BASE + 6)
#define SERIAL_REG_GS	(SERIAL_REG_BASE + 7)

#define INBOUND_READY	(1 << 7)
#define ERR_OVERRUN	(1 << 6)
#define ERR_PARITY	(1 << 5)
#define ERR_FRAME	(1 << 4)
#define SERIAL_BREAK	(1 << 3)
#define OUTBOUND_EMPTY	(1 << 2)
#define OUTBOUND_FULL	(1 << 1)
#define CTS_STATUS	(1 << 0)

#define DET_EN		(1 << 7) // Baud Rate Auto Detection Enable

/* UART interrupt APIs, only support recieve data */
enum UART_PORT {
    UART_PORT,
    AFC_UART_PORT
};

enum RCV_THRD {
    RCV_THRD_1 = 0,
    RCV_THRD_QUARTER,
    RCV_THRD_HALF,
    RCV_THRD_2_LESS
};

/* put a character to console */
void putb2 (char byte);
/* put a character to console, translate '\n' to '\r\n'*/
void putb (char byte);
/* block getb, it should wait until a character ready */
int getb (void);
/* non-block getb, if no character ready then return -1 */
int getb2 (void);

/* put a character to afc*/
void afc_putb2(const char c);
/* put a character to afc, translate '\n' to '\r\n'*/
void afc_putb(const char c);
/* non-block getb, if no character ready then return -1 */
int afc_getb2(void);
/* block getb, if no character ready then return -1 */
int afc_getb(void);

int afc_uart_irq_open(enum RCV_THRD thrd);
int afc_uart_irq_close(void);

void afc_enable_CTSRTS(int enable);

int console_exist (void);
int console_detect (void);
int getBaudRate (void);
void setBaudRate (unsigned rate);

int uart_irq_open(enum UART_PORT, enum RCV_THRD);
int uart_irq_close(void);
int uart_irq_readb(unsigned char *ch);

#endif // __SERIAL_H__

