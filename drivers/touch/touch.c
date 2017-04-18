/**
 *  @file   touch.c
 *  @brief  touch interrupt handler
 *  $Id: touch.c,v 1.19 2015/01/19 02:33:25 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.19 $
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
#include <registry.h>
#include "reg_tw.h"

#include <drivers/scaler/scaler.h>
#include <drivers/scaler/scaler_irq.h>
#include <drivers/touch/touch.h>
#include <drivers/calibration/calibration.h>
#include <drivers/osd2/osd2.h>


//TOUCH

#define SAR1_MASK            (REG_SCALER_BASE | PAGE_0 | (0xE7 << 4))
#define SAR_MASK             (REG_SCALER_BASE | PAGE_2 | (0xE3 << 4))
#define SAR0_INTERRUPT       0xb0400e70
#define SCALER_INT_STS_LOW   0xb0402e00
#define SCALER_INT_STS_HIGH  0xb0402e10
#define SCALER_INT_MASK_LOW  0xb0402e20
#define SCALER_INT_MASK_HIGH 0xb0402e30
#define SAR0_INITIAL_SETTING 0xb0403400
#define SAR0_THRESHOLD_LOW	 0xb0403410
#define SAR0_THRESHOLD_HIGH  0xb0403420
#define SAR0_STATUS          0xb04034c0
#define SAR0_TRIG_STATUS     0xb04034d0
#define SAR0_TOUCH_CLOCK     0xb04034f0
#define SAR0_TPMEASURE_GAP   0xb0403510
#define SAR0_PDDMEASURE_GAP  0xb0403600
#define SAR0_MEASURE_GAP     0xb0403610

#define SAR0_BOUN_THREHX     0xb0403700
#define SAR0_BOUN_THREHY     0xb0403710
#define SAR0_TIMES_XY        0xb0403720
#define SAR0_BOUN_THREH0     0xb0403750

#define SAR0_VALUE_LOWXY     0xb04035d0
#define SAR0_VALUE_HIGHX     0xb04035e0
#define SAR0_VALUE_HIGHY     0xb04035f0

#define TOUCH_BUFF_SIZE	100

//#define Calibration_MAX     1024
//int Calibration_MAX = 1024;

#define DWHSZ_LO    0xb0400b40
#define DWHSZ_HI    0xb0400b50
#define DWVSZ_LO    0xb0400b60
#define DWVSZ_HI    0xb0400b70

static unsigned short txqueue[TOUCH_BUFF_SIZE]={0};
static unsigned short tyqueue[TOUCH_BUFF_SIZE]={0};
static unsigned char tfront=TOUCH_BUFF_SIZE-1;
static unsigned char trear=TOUCH_BUFF_SIZE-1;
static unsigned char tstatus[TOUCH_BUFF_SIZE]={0};
static unsigned short repeat_count=0x00;
unsigned short td_rep_thres= 200;
int Calibration_MAX = 1024;

tcoord  t_coord={0,0};

#define REG_SCALAR_BASE 0xb040000
#define PAGE0(addr)  (0xb0400000 | (addr << 4))
#define PAGE1(addr)  (0xb0401000 | (addr << 4))
#define PAGE2(addr)  (0xb0402000 | (addr << 4))
#define PAGE3(addr)  (0xb0403000 | (addr << 4))
const unsigned char HANSTER_TOUCH_INIT_TBL[]={
0X40,0X6D,
0X41,0X0F,
0X42,0X01,
0X4D,0X03,
0X4F,0X64,
0X51,0XFF,
0X60,0XFF,
0X61,0XFF,
0X70,0X09,
0X71,0X09,
0X72,0X05,
0X75,0X44,
0XFF,0X00,
};

#define bmp_offset 0x0d

void WriteTouchTable(const unsigned char* RegsTable)
{
	unsigned short Index = 0x00;
	while(1){
		if(RegsTable[Index] == 0xFF)
				break;
		writeb(RegsTable[Index+1],PAGE3(RegsTable[Index]));
		Index += 2;
	}
}

void touch_queue(unsigned short avgx,unsigned short avgy,unsigned char status)
{
	if(avgx!=0||avgy!=0){
		trear=(trear+1)%TOUCH_BUFF_SIZE;
		if(trear==tfront){
			trear=TOUCH_BUFF_SIZE-1;
			trear=TOUCH_BUFF_SIZE-1;
			trear=(trear+1)%TOUCH_BUFF_SIZE;
			txqueue[trear]=avgx;
			tyqueue[trear]=avgy;
			tstatus[trear]=status;
		}else{
			txqueue[trear]=avgx;
			tyqueue[trear]=avgy;
			tstatus[trear]=status;
		}
	}
}

void touch_auto_switch(unsigned long *avgx, unsigned long *avgy, unsigned char tp_mode)
{
    unsigned short temp;
	
    switch(tp_mode)
	{
	    case 0x01:
			*avgx=*avgx;
			*avgy=*avgy;
			break;
		case 0x02:
		    *avgx=Calibration_MAX-*avgx;
		    break;
	    case 0x03:
			*avgy=Calibration_MAX-*avgy;
			break;
		case 0x04:
			*avgx=Calibration_MAX-*avgx;
			*avgy=Calibration_MAX-*avgy;
			break;
		case 0x05:
			temp=*avgx;
			*avgx=*avgy;
			*avgy=temp;
			break;
		case 0x06:
			temp=*avgx;
			*avgx=*avgy;
			*avgy=temp;
            *avgy=Calibration_MAX-*avgy;
			break;
        case 0x07:
			temp=*avgx;
			*avgx=*avgy;
			*avgy=temp;
            *avgx=Calibration_MAX-*avgx;
			*avgy=Calibration_MAX-*avgy;
			break;
		case 0x08:
			temp=*avgx;
			*avgx=*avgy;
			*avgy=temp;
            *avgx=Calibration_MAX-*avgx;
			break;
		default:
			break;
    }
}

void doTouch(unsigned char choice)
{
		unsigned long avgx=0;
		unsigned long avgy=0;
		unsigned char status=0;
		
		if((readb(SAR0_STATUS)&0x10)==0X10&&(readb(SAR0_TRIG_STATUS)&0x40)!=0x40){ //pendown
				status=TOUCH_PEN_DOWN;
		}else if(readb(SAR0_STATUS)&0x20){  //release
				status=TOUCH_PEN_REL;
		}else if((readb(SAR0_STATUS)&0x10)==0X10&&(readb(SAR0_TRIG_STATUS)&0x40)==0x40){ //move
				status=TOUCH_PEN_MOVE;
		}
			
		avgx=readb(SAR0_VALUE_LOWXY)&0x0f;//read avgx [7:4] 0xb04065d0
		avgx=(readb(SAR0_VALUE_HIGHX)<<4)|avgx;//read avgx [11:4] 0xb04065e0
		avgy=readb(SAR0_VALUE_LOWXY)>>4;//read avgy [3:0] 0xb04065d0
		avgy=(readb(SAR0_VALUE_HIGHY)<<4)|avgy;//read avgy avgx[11:4] 0xb04065f0
		if(Calibration_MAX == 1024){
			avgx=avgx>>2;
			avgy=avgy>>2;
		}else if(Calibration_MAX == 2048){
			avgx=avgx>>1;
			avgy=avgy>>1;
		}else if(Calibration_MAX == 4096){
			avgx=avgx;
			avgy=avgy;
		}
		
		switch (choice)
		{
			case 0:
				touch_queue(avgx,avgy,status);
				break;
			case 1:
            touch_auto_switch(&avgx, &avgy, tp_mode);
			if((avgx!=0) || (avgy!=0)){

					avgx=c_w-(((avgx-tp_right)*c_w)/(tp_left-tp_right));
					avgy=c_h-((avgy-tp_down)*c_h)/(tp_up-tp_down);


					if(status!=2){
						t_coord.x=avgx;
						t_coord.y=avgy;
					}
					else if(status==2){
						avgx=t_coord.x;
						avgy=t_coord.y;
					}
					touch_queue(avgx,avgy,status);
			}else{
				writeb(0x01, SCALER_INT_STS_HIGH);
				writeb(readb(SAR0_INTERRUPT) | 0x02,SAR0_INTERRUPT);
				return;
			}
			break;
		}
		writeb(0x01, SCALER_INT_STS_HIGH);
		writeb(readb(SAR0_INTERRUPT) | 0x02,SAR0_INTERRUPT);
}

void touch_isr(void)
{
	if(calibration_finish==0)
		doTouch(0);
	else
		doTouch(1);
	repeat_count=0x00;
}

touchd touch_read(void)
{
	touchd result={0,0,0};//Initial
	if(tfront==trear){
		result.x=0;
		result.y=0;
		result.status=TOUCH_NOACK;
		return result;
	}else{
		tfront=(tfront+1)%TOUCH_BUFF_SIZE;
		result.x=txqueue[tfront];
		result.y=tyqueue[tfront];
		result.status=tstatus[tfront];
		return result;
	}
}

void touch_init(void)
{
#if 0
    // p2_e0~3, p0_e7
	writeb(0xce, SAR0_INTERRUPT);//clear sar interrupt and unmask sar 0 interrupt 0xb0400e70
    writeb(0xff, SCALER_INT_MASK_LOW);
    writeb(0xfc, SCALER_INT_MASK_HIGH);
    //writeb(0x03, SCALER_INT_STS_HIGH);
#endif
#if 1
	writeb(readb(SAR0_INTERRUPT)&(~0x20), SAR0_INTERRUPT);//clear sar interrupt and unmask sar 0 interrupt 0xb0400e70
    writeb(0xff, SCALER_INT_MASK_LOW);
	writeb(0xff, SCALER_INT_MASK_HIGH);
    writeb(readb(SCALER_INT_MASK_HIGH)&(~0x01), SCALER_INT_MASK_HIGH);
#endif
    enable_scaler_irq(IRQ_TP_TOGGLE);

	WriteTouchTable(HANSTER_TOUCH_INIT_TBL);
	//touch
	c_w = readb(DWHSZ_HI)<<8 | readb(DWHSZ_LO);
    c_h = readb(DWVSZ_HI)<<8 | readb(DWVSZ_LO);
}

void touch_close(void)
{
	//SAR1 & touh panel
	//writeb(0xE0,SAR1_MASK);
	//writeb((readb(SAR_MASK) | 0x03),SAR_MASK);
	writeb((readb(SAR1_MASK) | 0x20),SAR1_MASK);
	writeb((readb(SAR_MASK) | 0x01),SAR_MASK);
}

void touch_clear(void)
{
	tfront=TOUCH_BUFF_SIZE-1;
	trear=TOUCH_BUFF_SIZE-1;
}

touchd check_repeat(unsigned short threshold)
{
	touchd result={0,0,0};//Initial 
	if(((readb(SAR0_TRIG_STATUS)&0x80)>0)&&((readb(SAR0_TRIG_STATUS)&0x04)!=0x04)){
		repeat_count++;
		if(repeat_count==0xFFFF)
			repeat_count=0x00;
		if(repeat_count>=threshold){
			repeat_count=0x00;
			result.x=txqueue[tfront];
			result.y=tyqueue[tfront];;
		}
	}
	return result;
}

int touch_set_resolution(int value)
{
	int ret = 0;
	if((value == 1024) || (value == 2048) || (value == 4096))
		Calibration_MAX = value;
	else 
		ret = -1;
	printf("touch panel resolution = %d ret = %d\n",Calibration_MAX,ret);
	return ret;
}

#define diff 20

void draw_target_test(void)
{
	unsigned char x=0;
	unsigned char y=0;

	unsigned char temp;
	unsigned char O_LSB=0;
	unsigned char O_MSB=0;
	
	osd2_init();

	osd2_disable();
	
	osd2_cfg_wr(0x01, 0x12);// disable more left (setting default value)

	/* Load Main Color LUT */
	osd2_cfg_wr(0x08, 0x00);	// OSD LUT address start point
	IC_WriteByte(TWIC_P0, 0xA8, 0x09);

	IC_WriteByte(TWIC_P0, 0xA9, 0xFF);	// A
	IC_WriteByte(TWIC_P0, 0xA9, 0x00);	// B
	IC_WriteByte(TWIC_P0, 0xA9, 0x00);	// G
	IC_WriteByte(TWIC_P0, 0xA9, 0x00);	// R

	IC_WriteByte(TWIC_P0, 0xA9, 0x00);	// A
	IC_WriteByte(TWIC_P0, 0xA9, 0x00);	// B
	IC_WriteByte(TWIC_P0, 0xA9, 0xFF);	// G
	IC_WriteByte(TWIC_P0, 0xA9, 0x00);	// R

	/* Set BMP Color LUT Base Address */
	osd2_cfg_wr(0x2C, 0x00);

	/* Set BMP Width */
	osd2_cfg_wr(0x24, 0x0A);
	osd2_cfg_wr(0x25, 0x00);
	/* Set BMP Height */
	osd2_cfg_wr(0x26, 0x28);
	osd2_cfg_wr(0x27, 0x00);
	/* Set BMP Horizintal Start Position */
	osd2_cfg_wr(0x28, 0x15);
	osd2_cfg_wr(0x29, 0x00);
	/* Set BMP Vertical Start Position */
	osd2_cfg_wr(0x2A, 0x00);
	osd2_cfg_wr(0x2B, 0x00);
	/* Set OSD RAM Base Address */
	osd2_set_ram_addr(0x0000);
	/* Load BMP Index to OSDRAM */
	for(y=0; y<40; y++) {
		for(x=0; x<40; x++) {
			if(x==19||y==19)
				temp = 0x01;
			else
				temp = 0x00;

			switch((x%4)) {
				case 0:
					O_MSB=(temp<<4);
					break;
				case 1:
					O_MSB|=temp;
					break;
				case 2:
					O_LSB=(temp<<4);
					break;
				case 3:
					O_LSB|=temp;
					IC_WriteByte(TWIC_P0, 0xAB, O_LSB);
					IC_WriteByte(TWIC_P0, 0xAB, O_MSB);
					break;
			}
		}
	}
	/* Set BMP Data Address in OSDRAM */
	osd2_cfg_wr(0x21, 0x00);
	osd2_cfg_wr(0x22, 0x00);

	/* Set BMP Configuration */
	osd2_cfg_wr(0x0B, 0x00);
	osd2_cfg_wr(0x0E, 0x07);
	osd2_cfg_wr(0x20, 0xC0);
	
	osd2_cfg_wr(OSD_CTRL, 0x80);
}

void touch_targe_test(void)
{
    touchd  tv     = {0, 0, 0};
	int ch = 0;
	int w = 0;
	int h = 0;
	int osd2_temp;
	writel(0x87,0xb0400910);
	osd2_temp = osd2_cfg_read(0x01);// store current vale
	osd2_cfg_wr(0x01, 0x12);// disable more left (setting default value)
	do_calibration(CHECK_IFNOT_CALIBRA);
	draw_target_test();
	_osd2_bmp_pos((diff-diff)+bmp_offset, (diff-diff));
	w = readb(DWHSZ_HI)<<8 | readb(DWHSZ_LO);
    h = readb(DWVSZ_HI)<<8 | readb(DWVSZ_LO);
	printf("c_w =%d c_h =%d\n",w,h);
    while(1){
        tv = touch_read();
		if(tv.status!=3){
			if(tv.x<=w && tv.y<=h){
				printf("touch x=%d y=%d sts=%d\n", (int)tv.x, (int)tv.y, (int)tv.status);
				_osd2_bmp_pos((tv.x-diff)+bmp_offset, (tv.y-diff));
			}else{
				//printf("outof range\n");
				printf("out of range touch x=%d y=%d sts=%d\n", (int)tv.x, (int)tv.y, (int)tv.status);
			}
		}
        ch = getb2();
        if ((ch == 'q') || (ch == 'Q')) {
			osd2_cfg_wr(0x01, osd2_temp);//restore value
			osd2_disable();
			osd2_bmp_disable();
			break;
		}
    }
}
