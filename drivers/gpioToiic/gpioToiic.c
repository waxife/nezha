/**
 *  @file   iichost.h
 *  @brief  T582 gpio to iic host driver
 *  $Id: gpioToiic.c,v 1.2 2014/07/18 02:01:06 nick1516 Exp $
 *  $Author: nick1516 $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 *
 *  @date   2014/07/03  Nick     New file.
 *
 */




#include <stdio.h>
#include <config.h>
#include <debug.h>
#include <io.h>
#include <sys.h>
#include <gpio.h>
#include <gpioToiic.h>




void gpio_to_iIC_start()
{

#ifdef _gpio_iIC_SPEED
	int c;
#endif
	gpioa_out(_gpio_SDA);
	gpio_out(_gpio_SCL);

	
	gpioa_set(_gpio_SDA);
	gpio_set(_gpio_SCL);	
	_Delay(c);
	gpioa_clear(_gpio_SDA);
	_Delay(c);
	_Delay(c);	
	gpio_clear(_gpio_SCL);
	_Delay(c);	
}

void gpio_to_iIC_stop()
{
#ifdef _gpio_iIC_SPEED
	int c;
#endif	
	gpioa_out(_gpio_SDA);

	gpio_clear(_gpio_SCL);
	gpioa_clear(_gpio_SDA);	
	_Delay(c);
	gpio_set(_gpio_SCL);
	_Delay(c);
	gpioa_set(_gpio_SDA);	
}
unsigned char gpio_to_iIC_wrbyte(unsigned char wrData)
{
#ifdef _gpio_iIC_SPEED
	int c;
#endif
	int i;
	unsigned char ack;
	unsigned char mask =0x80;
	for(i=0;i<8;i++){
		gpio_clear(_gpio_SCL);
		
		if(mask&wrData) 
			gpioa_set(_gpio_SDA);  //write data 
		else 
			gpioa_clear(_gpio_SDA);		
		wrData<<=1;
		_Delay(c);		
		gpio_set(_gpio_SCL);
		_Delay(c);
		gpio_clear(_gpio_SCL);
	}
	gpioa_set(_gpio_SDA);	
	gpioa_in(_gpio_SDA);	
	ack = gpioa_read(_gpio_SDA);	
	_Delay(c);
	gpio_set(_gpio_SCL);
	_Delay(c);
	_Delay(c);
	gpio_clear(_gpio_SCL);
	_Delay(c);
	gpioa_out(_gpio_SDA);	

	return ack;

}
unsigned char gpio_to_iIC_rdbyte(unsigned char ack)
{
#ifdef _gpio_iIC_SPEED
	int c;
#endif
	int i;	
	unsigned char rdData=0;

	gpioa_in(_gpio_SDA);	

	for(i =7;i>=0;i--){
		_Delay(c);
		gpio_set(_gpio_SCL);
		_Delay(c);
		  
		if(gpioa_read(_gpio_SDA)) {
			rdData |=(1<<i);
		}		
		gpio_clear(_gpio_SCL);
		_Delay(c);
	}
	gpioa_out(_gpio_SDA);
	gpio_clear(_gpio_SCL);

	

	_Delay(c);

	if(ack)		gpioa_set(_gpio_SDA);
	else		gpioa_clear(_gpio_SDA);




	_Delay(c);	
	gpio_set(_gpio_SCL);

	_Delay(c);
	gpio_clear(_gpio_SCL);
	//_Delay(c);
	gpioa_set(_gpio_SDA);
	return rdData;

}
unsigned char gpio_to_iIC_wrreg(unsigned char cDevAddr, unsigned char cReg, 
						  			 unsigned char *arrData, unsigned char cNum)
{
	int i;
	gpio_to_iIC_start();
	if(gpio_to_iIC_wrbyte(cDevAddr)) return 0;

	if(gpio_to_iIC_wrbyte(cReg)) return 0;

	for(i=0;i<cNum;i++){
		if(gpio_to_iIC_wrbyte(arrData[i])) {
			return 0;
			}
		}

	gpio_to_iIC_stop();
	return 1;	

}

unsigned char gpio_to_iIC_rdreg(unsigned char cDevAddr, unsigned char cReg, 
						   			unsigned char *arrData, unsigned char cNum)
{
#ifdef _gpio_iIC_SPEED
	int c;
#endif
	unsigned char i;
	int rc =1;

	gpio_to_iIC_start();
	if(gpio_to_iIC_wrbyte(cDevAddr)){
		rc =0x02;
		goto _EXIT;
	}

	if(gpio_to_iIC_wrbyte(cReg)){
		rc =0x03;
		goto _EXIT;
	}
	_Delay(c);
	gpio_to_iIC_start();

	if(gpio_to_iIC_wrbyte(cDevAddr |0x01)){
		rc =0x04;
		goto _EXIT;
	}

	if(cNum<=0){
		rc=0x05;
		goto _EXIT;
	}

	if(cNum >1){
		for(i=0;i<cNum-1;i++){
			arrData[i] = gpio_to_iIC_rdbyte(_ACK);
		}
	}

	arrData[cNum-1] = gpio_to_iIC_rdbyte(_nACK);
	
_EXIT:
	gpio_to_iIC_stop();
	return rc;
}

























