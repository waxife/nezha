/**
 *  @file   iichost.h
 *  @brief  T582 gpio to iic host driver
 *  $Id: gpioToiic.h,v 1.1 2014/07/14 07:25:31 nick1516 Exp $
 *  $Author: nick1516 $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 *
 *  @date   2014/07/03  Nick     New file.
 *
 */



#ifndef _gpio_to_iIC
#define _gpio_to_iIC

#define _gpio_SDA 11    //gpio pin :data
#define _gpio_SCL 15	//gpio pin :clock
#define _ACK  0x00
#define _nACK 0x01
extern void gpio_to_iIC_start();
extern void gpio_to_iIC_stop();
extern unsigned char gpio_to_iIC_wrbyte(unsigned char wrData);
extern unsigned char gpio_to_iIC_rdbyte(unsigned char ack);
extern unsigned char gpio_to_iIC_wrreg(unsigned char cDevAddr, unsigned char cReg,unsigned char *arrData, unsigned char cNum);
extern unsigned char gpio_to_iIC_rdreg(unsigned char cDevAddr, unsigned char cReg,unsigned char *arrData, unsigned char cNum);




extern int get_sensor_value(int *ptemp, int *prh);   



#define _gpio_iIC_SPEED 0xf0 //value:1 (Fastest), 2 (slower), ... N (much slower) 0xf0  

#define _gpio_iIC_TEST 0x12c0

#define _Max_WAIT 0x10 

#ifdef _gpio_iIC_SPEED
	#define _Delay(c) for(c=0; c<_gpio_iIC_SPEED ;c++){}	
#else 
	#define _Delay(c);
#endif

#endif //_gpio_to_iIC


