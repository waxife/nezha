/**
 *  @file   sarkey.c
 *  @brief  sarkey interrupt handler 
 *  $Id: sarkey.c,v 1.18 2014/09/03 05:56:39 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.18 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <io.h>
#include <sys.h>
#include <mipsregs.h>
#include <math.h>
#include <cache.h>
#include <ctype.h>
#include <unistd.h>
#include <debug.h>
#include <gpio.h>
#include <serial.h>
#include <nor.h>
#include "reg_tw.h"

#include "sarkey.h"

#include <drivers/scaler/scaler_irq.h>

//SARKEY
#define SAR1_MASK               (REG_SCALER_BASE | PAGE_0 | (0xE7 << 4)) 
#define SAR1_STATUS             (REG_SCALER_BASE | PAGE_0 | (0xE7 << 4))
#define SAR1_SETUP              (REG_SCALER_BASE | PAGE_3 | (0x43 << 4))
#define SAR1_RVALUE             (REG_SCALER_BASE | PAGE_3 | (0x44 << 4)) 
#define SAR1_THRESHOLD          (REG_SCALER_BASE | PAGE_3 | (0x45 << 4))
#define SAR1_LTCH_VAL           (REG_SCALER_BASE | PAGE_3 | (0x47 << 4))

#define SAR2_SETUP              (REG_SCALER_BASE | PAGE_3 | (0x50 << 4))
#define SAR2_THRESHOLD          (REG_SCALER_BASE | PAGE_3 | (0x52 << 4))
#define SAR2_LTCH_VAL           (REG_SCALER_BASE | PAGE_3 | (0x54 << 4))

#define SAR1_MASK_VALUE         0X20    //for sarkey 
#define SAR1_SETUP_VALUE        0xFD
#define SAR1_THRESHOLD_VALUE    0x05
#define SAR1_INTCLEAR_VALUE     0x04

#define SAR2_MASK_VALUE         0X20    //for sarkey 
#define SAR2_SETUP_VALUE        0xFD
#define SAR2_THRESHOLD_VALUE    0x05
#define SAR2_INTCLEAR_VALUE     0x04

#define SAR0_00    0x00
#define SAR0_01    0x40
#define SAR0_1x    0x80
#define SAR1_00    0x00
#define SAR1_01    0x10
#define SAR1_1x    0x20
#define SAR2_00    0x00
#define SAR2_01    0x40
#define SAR2_1X    0X80
#define SAR1_SOURCE_SEL           (REG_SCALER_BASE | PAGE_3 | (0x48 << 4))
#define SAR2_SOURCE_SEL           (REG_SCALER_BASE | PAGE_3 | (0x58 << 4))

#define SAR_MASK             (REG_SCALER_BASE | PAGE_2 | (0xE3 << 4))

#define sarkey_diff 10

#define SAR0  0
#define SAR1  1
#define SAR2  2
  
static unsigned char sarqueue[10]={0};
static unsigned char sfront=9;
static unsigned char srear=9;

static unsigned char sarqueue2[10]={0};
static unsigned char sfront2=9;
static unsigned char srear2=9;

//const unsigned char sarkey_evb_table[6][2]={{0xba,0x01},{0xa4,0x02},{0xd2,0x03},{0x00,0x04},{0x79,0x05},{0x00,0x06}}; /*new key board*/
//const unsigned char sarkey_evb_table[4][2]={{0xba,0x01},{0xa4,0x02},{0xd2,0x03},{0x79,0x05}};

//const unsigned char sarkey_demo_table[7][2]={{0xe7,0x01},{0xc4,0x02},{0x98,0x03},{0x80,0x04},{0x66,0x05},{0x3f,0x06},{0x20,0x07}};
const unsigned char sarkey_evb_table[6][2]={{0xe8,0x01},{0xd5,0x02},{0xaa,0x03},{0x7f,0x04},{0x55,0x05},{0x2a,0x06}};
//const unsigned char sarkey_evt_table[6][2]={{0xd9,0x01},{0xba,0x02},{0x98,0x03},{0x4c,0x04},{0x67,0x05},{0x80,0x06}};
const unsigned char (*sarkey_table)[2]  = sarkey_evb_table;
const unsigned char (*sarkey2_table)[2] = sarkey_evb_table;

#define time_thread 200 //minisecond unit
 
unsigned char sarkey_num = 7;

unsigned char mSarkey1_val=0;
unsigned char mSarkey2_val=0;
unsigned char mSarkey1Count =0;
unsigned char mSarkey2Count =0;
unsigned char m_Keycode1 =0;
unsigned char m_Keycode2 =0;
unsigned char mkey1stroke_ch=0;
unsigned char mkey2stroke_ch=0;
#define _sarkey_repete_count 0x85
unsigned char sarkey2_num = 7;


void sarkey_event(void)
{
	unsigned char keycode;
	unsigned char c = 0;
	
	unsigned long long hold_time;
	static unsigned long long start_time = 0x00;       
	keycode=readb(SAR1_LTCH_VAL);

	printf("keycode sarkey_event %d\r\n", keycode);
	
	for(c=0;c<sarkey_num;c++)
	{
		if((keycode>=(sarkey_table[c][0]-sarkey_diff)) && (keycode <= (sarkey_table[c][0]+sarkey_diff))){
			keycode = sarkey_table[c][1];
			m_Keycode1 = keycode;
			break;
		}
	}
	
	if(keycode >sarkey_num){
		keycode = 0xff;
	}
	
	if(keycode != 0xff){
		if(start_time !=0){
			if(read_c0_count32()>start_time)
				hold_time = read_c0_count32() - start_time;
			else
				hold_time = start_time - read_c0_count32();
			if(hold_time < msec2hz(time_thread)){
				keycode = 0xff;
			}else{
				start_time = 0;
			}
		}else{
			start_time =read_c0_count32();
		}
	}

	if(keycode!=0xff)
	{
		srear=(srear+1)%10;
		if(srear==sfront)
		{
			if(srear==0)
			{
				srear=10-1;//max value
			}
			else
			{
				srear=srear-1;
			}
		}
		else
		{
			sarqueue[srear]=keycode;
		}
	}
}

void sarkey2_event(void)
{
	unsigned char keycode;
	unsigned char c = 0;
	
	unsigned long long hold_time;
	static unsigned long long start_time = 0x00;
	keycode=readb(SAR2_LTCH_VAL);
	
	for(c=0;c<sarkey2_num;c++)
	{
		if((keycode>=(sarkey2_table[c][0]-sarkey_diff)) && (keycode <= (sarkey2_table[c][0]+sarkey_diff))){
			keycode = sarkey2_table[c][1];
			m_Keycode2 = keycode;
			break;
		}
	}
	
	if(keycode >sarkey2_num){
		keycode = 0xff;
	}
	
	if(keycode != 0xff){
		if(start_time !=0){
			if(read_c0_count32()>start_time)
				hold_time = read_c0_count32() - start_time;
			else
				hold_time = start_time - read_c0_count32();
			if(hold_time < msec2hz(time_thread)){
				keycode = 0xff;
			}else{
				start_time = 0;
			}
		}else{
			start_time =read_c0_count32();
		}
	}

	if(keycode!=0xff)
	{
		srear2=(srear2+1)%10;
		if(srear2==sfront2)
		{
			if(srear2==0)
			{
				srear2=10-1;//max value
			}
			else
			{
				srear2=srear2-1;
			}
		}
		else
		{
			sarqueue2[srear2]=keycode;
		}
	}
}

void sarkey_isr(void)
{
	if(readb(0xb0400e70)&0x04){
		sarkey_event();
		writeb(readb(0xb0400e70)|0x24,0xb0400e70);
		writeb(readb(0xb0402e10)|0x02,0xb0402e10);
	}else{
		sarkey2_event();
		writeb(readb(0xb0400e70)|0x28,0xb0400e70);
		writeb(readb(0xb0402e10)|0x02,0xb0402e10);
	}
}

int sar1_read (void)
{
	if(sfront==srear)
	{
		return 0;
	}
	else
	{
		sfront=(sfront+1)%10;
		return sarqueue[sfront];
	}
}

int sar2_read (void)
{
	if(sfront2==srear2)
	{
		return 0;
	}
	else
	{
		sfront2=(sfront2+1)%10;
		return sarqueue2[sfront2];
	}
}

void sarkey_init (void)
{
	writel(SAR1_SETUP_VALUE,SAR1_SETUP);//setup
	writel(SAR1_THRESHOLD_VALUE,SAR1_THRESHOLD);//threshold
	//writel(SAR1_MASK_VALUE,SAR1_MASK);//disable mask
	writel(readb(SAR1_MASK)&(~0xc0),SAR1_MASK);//disable mask 1100 0000 ->0xc0
	
	writel(SAR2_SETUP_VALUE,SAR2_SETUP);//setup
	writel(SAR2_THRESHOLD_VALUE,SAR2_THRESHOLD);//threshold

	/* default for EVB */
	sarkey_init_table(sarkey_evb_table, 6);
	sarkey2_init_table(sarkey_evb_table, 6);
	//sarkey_init_table(sarkey_demo_table, 7);

	enable_scaler_irq(IRQ_SARN_TOGGLE);
}

int sar_sel(unsigned char sar_no,unsigned char pin_no)
{
	if((sar_no>0x02) || (pin_no>0x02) || (sar_no == 0))
		return -1;
	if(sar_no == SAR1)
		writeb((readb(SAR1_LTCH_VAL)&0xc0)|(pin_no<<4), SAR1_SOURCE_SEL);
	else if(sar_no == SAR2)
		writeb((pin_no<<6), SAR2_SOURCE_SEL);
	return 0;
}

void sarkey_close(void)
{
	//SAR1 & touh panel
	writeb((readb(SAR1_MASK) | 0xc0),SAR1_MASK);
	writeb((readb(SAR_MASK) | 0x02),SAR_MASK);
}

void sarkey_clear(void)
{
	sfront=9;
	srear=9;
}
 
void sarkey2_clear(void)
{
	sfront2=9;
	srear2=9;
}

void sarkey_init_table(const unsigned char (*sarkey)[2], unsigned char num)
{
	sarkey_table = sarkey;  
	sarkey_num =num;
}




void sarkey2_init_table(const unsigned char (*sarkey)[2], unsigned char num)
{
	sarkey2_table = sarkey;
	sarkey2_num =num;
}
unsigned char sar_status( )
{
	unsigned char keycode=0;
	unsigned char c = 0; 
	if(mSarkey1_val >>7 ==0x01)	{
		keycode=readb(SAR1_LTCH_VAL);		
		printf("keycode sar_status %d\r\n", keycode);
		if(keycode ==0xff){     //Key_up
			mSarkey1_val =0;
			m_Keycode1 =0;
			mSarkey1Count=0;
			return 0x01;
		}		
		else { 					//repete		
			for(c=0;c<sarkey_num;c++)		{
				if((keycode>=(sarkey_table[c][0]-sarkey_diff)) && (keycode <= (sarkey_table[c][0]+sarkey_diff))){
					keycode = sarkey_table[c][1];			
					break;
				}
			}
			if(keycode >sarkey_num)	{
				keycode = 0xff; 
				return 0x00;
			} 
			if(keycode !=0xff && m_Keycode1 ==keycode) {	
				mSarkey1Count++;			
				if(mSarkey1Count >=0xff){			
					mSarkey1_val++;				
					mSarkey1Count=0;
					if(mSarkey1_val >=_sarkey_repete_count){
						mSarkey1_val &=0x80;					
						return 0x02;
					}
				}
			}			
		}
	}	
	if(mSarkey2_val >>7 ==0x01) {
		keycode=readb(SAR2_LTCH_VAL);
		if(keycode==0xff){      //Key_up
			mSarkey2_val =0;
			m_Keycode2 =0;
			mSarkey2Count=0;
			return 0x01;
		}
		else {					//repete
			for(c=0;c<sarkey_num;c++)	{
				if((keycode>=(sarkey2_table[c][0]-sarkey_diff)) && (keycode <= (sarkey2_table[c][0]+sarkey_diff))){
					keycode = sarkey2_table[c][1];			
					break;
				}
			}
			if(keycode >sarkey_num)	{
				keycode = 0xff; 
				return 0x00;
			}
			if(keycode !=0xff && m_Keycode2 ==keycode) 	{				
				 mSarkey2Count++;				
				 if(mSarkey2Count >=0xff){
				 	mSarkey2_val++;
					mSarkey2Count=0;
					if(mSarkey2_val >=_sarkey_repete_count){
					 	mSarkey2_val &=0x80;
						mSarkey2Count=0;
						return 0x02;
					}
				 }
			}
		}
			
	}
	

 return 0;	 
}

