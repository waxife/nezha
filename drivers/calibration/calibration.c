/**
 *  @file   calibration.c
 *  @brief  calibration for touch panel driver
 *  $Id: calibration.c,v 1.18 2015/08/06 03:25:17 dos1236 Exp $
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
#include <registry2.h>
#include "reg_tw.h"

#include <drivers/scaler/scaler.h>
#include <drivers/touch/touch.h>
#include <drivers/calibration/calibration.h>
#include <drivers/osd2/osd2.h>
#include <watchdog.h>


#define diff 	20
#define c_x 	100	// calibration x axis
#define c_y 	60// calibration y axis

#define DWHSZ_LO    0xb0400b40
#define DWHSZ_HI    0xb0400b50
#define DWVSZ_LO    0xb0400b60
#define DWVSZ_HI    0xb0400b70

unsigned short tp_left=0x00;
unsigned short tp_right=0x00;
unsigned short tp_up=0x00;
unsigned short tp_down=0x00;
unsigned short tp_mode = 0x00;
unsigned char  calibration_finish = 0x00;
unsigned short c_w = 0;
unsigned short c_h = 0;
unsigned short diff_threshold = 0x20;


int get_touch_boundary_value(unsigned short* tp_left,unsigned short* tp_right,unsigned short* tp_up,unsigned short* tp_down,unsigned short* tp_mode)
{
	int rc = 0;
	int i = 0;
	int val;

	rc =1;
	if(rc!=0){
		for(i=0;i<=4;i++)
		{
			switch(i)
			{
				case 0:
					rc=_registry2_get(RS_TOUCH_CALI_LEFT,&val,0xff);
					if(rc == 0)
						*tp_left = val;
					else
						goto EXIT;
				break;
				case 1:
					rc=_registry2_get(RS_TOUCH_CALI_RIGHT,&val,0xff);
					if(rc == 0)
						*tp_right = val;
					else
						goto EXIT;
				break;
				case 2:
					rc=_registry2_get(RS_TOUCH_CALI_UP,&val,0xff);
					if(rc == 0)
						*tp_up = val;
					else
						goto EXIT;
				break;
				case 3:
					rc=_registry2_get(RS_TOUCH_CALI_DOWN,&val,0xff);
					if(rc == 0)
						*tp_down = val;
					else
						goto EXIT;
				break;
				case 4:
					rc=_registry2_get(RS_TOUCH_CALI_MODE,&val,0xff);
					if(rc == 0){
						*tp_mode = (val >> 4);
						calibration_finish = val&0x01;
					}else{
						goto EXIT;
					};
					calibration_finish = val&0x01;
				break;
			}
		}
		printf("read left =%d right=%d up=%d down=%d tp_mode=0x%x\n",*tp_left,*tp_right,*tp_up,*tp_down,*tp_mode);
	}
	return 1;
EXIT:
	return 0;
}

void TouchSave(void)
{
	unsigned char rc=0;
	unsigned char i = 0;
	rc = 1;
	if(rc!=0) {
		for(i=0;i<=4;i++) {
			switch(i) {
				case 0:
					_registry2_set(RS_TOUCH_CALI_LEFT,tp_left);
					break;
				case 1:
					_registry2_set(RS_TOUCH_CALI_RIGHT,tp_right);
					break;
				case 2:
					_registry2_set(RS_TOUCH_CALI_UP,tp_up);
					break;
				case 3:
					_registry2_set(RS_TOUCH_CALI_DOWN,tp_down);
					break;
				case 4:
					_registry2_set(RS_TOUCH_CALI_MODE,(tp_mode<<4|0X01));
					break;
			}
		}
		printf("write left =%d right=%d up=%d down=%d tp_mode=0x%x\n",tp_left,tp_right,tp_up,tp_down,tp_mode);
	}
}
unsigned short sub(unsigned short x,unsigned short y)
{
	if(x>y)
		return x-y;
	else
		return y-x;
}

void drawtarget(unsigned char select)
{
	unsigned char x=0;
	unsigned char y=0;

	unsigned char temp;
	unsigned char O_LSB=0;
	unsigned char O_MSB=0;
    int rc = 0;

	rc=get_touch_boundary_value(&tp_left,&tp_right,&tp_up,&tp_down,&tp_mode);
	if((rc != 0) && ((select == CHECK_IFNOT_CALIBRA) || (select == FACTORY_CALIBRA_MODE))
		&& ((calibration_finish&0x01)==1)){
		return;
	} else if ((select == CHECK_IFNOT_DEFAULT) || (select == CHECK_IFNOT_CALIBRA)) {
		tp_left		= DEFAULT_TP_LEFT;
		tp_right	= DEFAULT_TP_RIGHT;
		tp_up		= DEFAULT_TP_UP;
		tp_down		= DEFAULT_TP_DOWN;
		tp_mode		= DEFAULT_TP_MODE;
		c_w 		= DEFAULT_TP_WIDTH;
	    c_h 		= DEFAULT_TP_HIGHT;
	    calibration_finish = 1;
		return;
	} else {	// FORCE_TP_CALIBRA
		calibration_finish = 0;
	}

	osd2_disable();

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
}

int check_correct_position(unsigned long avgx, unsigned long avgy,int target_x,int target_y)
{
    unsigned short temp;
	unsigned int x = (unsigned int)avgx;
	unsigned int y = (unsigned int)avgy;
	int rc = 0;
    switch(tp_mode)
	{
	    case 0x01:
			x = x;
			y = y;
			break;
		case 0x02:
		    x=Calibration_MAX-x;
		    break;
	    case 0x03:
			y=Calibration_MAX-y;
			break;
		case 0x04:
			x=Calibration_MAX-x;
			y=Calibration_MAX-y;
			break;
		case 0x05:
			temp=x;
			x=y;
			y=temp;
			break;
		case 0x06:
			temp=x;
			x=y;
			y=temp;
            y=Calibration_MAX-y;
			break;
        case 0x07:
			temp=x;
			x=y;
			y=temp;
            x=Calibration_MAX-x;
			y=Calibration_MAX-y;
			break;
		case 0x08:
			temp=x;
			x=y;
			y=temp;
            x=Calibration_MAX-x;
			break;
		default:
			break;
    }
	x=c_w-(((x-tp_right)*c_w)/(tp_left-tp_right));
	y=c_h-((y-tp_down)*c_h)/(tp_up-tp_down);
	dbg(0,"x = %d y = %d\n",x,y);
	if(sub(target_x,x)<=diff_threshold &&sub(target_y,y)<=diff_threshold)
		rc = 0;
	else
		rc = -1;
	return rc;
}

void save_default_value(void)
{
		tp_left		= DEFAULT_TP_LEFT;
		tp_right	= DEFAULT_TP_RIGHT;
		tp_up		= DEFAULT_TP_UP;
		tp_down		= DEFAULT_TP_DOWN;
		tp_mode		= DEFAULT_TP_MODE;
		c_w 		= DEFAULT_TP_WIDTH;
	    c_h 		= DEFAULT_TP_HIGHT;
	    calibration_finish = 1;
}

unsigned char calibration (unsigned short int *x,unsigned short int *y)
{
	unsigned short int xa,xb,ya,yc;
	unsigned char tempx=0x00;
	unsigned char tempy=0x00;

	xa=c_x;
	xb=c_w-c_x;
	ya=c_h-c_y;
	yc=c_y;
	tempx= sub(sub(x[0],x[1]),sub(x[2],x[3]));
	tempy= sub(sub(y[0],y[1]),sub(y[2],y[3]));
	
	if(tempx<=diff_threshold&&tempy<=diff_threshold)
	{
	    if(sub(x[0],x[1])>200)
	    {
	        if((x[0]>x[1])&&(y[0]>y[2]))
	        {
			    tp_mode=0x01;

		    }
		    else if((x[0]<x[1])&&(y[0]>y[2]))
		    {
	            x[0]=Calibration_MAX-x[0];
			    x[1]=Calibration_MAX-x[1];
			    x[2]=Calibration_MAX-x[2];
			    x[3]=Calibration_MAX-x[3];
			    tp_mode=0x02;

		    }
		    else if((x[0]>x[1])&&(y[0]<y[2]))
	        {
	            y[0]=Calibration_MAX-y[0];
			    y[1]=Calibration_MAX-y[1];
			    y[2]=Calibration_MAX-y[2];
			    y[3]=Calibration_MAX-y[3];
			    tp_mode=0x03;

		    }
		    else if((x[0]<x[1])&&(y[0]<y[2]))
		    {
	            x[0]=Calibration_MAX-x[0];
			    x[1]=Calibration_MAX-x[1];
			    x[2]=Calibration_MAX-x[2];
			    x[3]=Calibration_MAX-x[3];
	            y[0]=Calibration_MAX-y[0];
			    y[1]=Calibration_MAX-y[1];
			    y[2]=Calibration_MAX-y[2];
			    y[3]=Calibration_MAX-y[3];
			    tp_mode=0x04;

		    }
		}
        else if(sub(y[0],y[1])>200)
        {
            unsigned short temp1=0x00;
            if((y[0]>y[1])&&(x[0]>x[2]))
            {

                temp1=x[0];
		        x[0]=y[0];
		        y[0]=temp1;
		        temp1=x[1];
		        x[1]=y[1];
		        y[1]=temp1;
		        temp1=x[2];
		        x[2]=y[2];
		        y[2]=temp1;
		        temp1=x[3];
		        x[3]=y[3];
		        y[3]=temp1;

		        tp_mode=0x05;

	        }
            else  if((y[0]>y[1])&&(x[0]<x[2]))
            {

                temp1=x[0];
		        x[0]=y[0];
		        y[0]=temp1;
		        temp1=x[1];
		        x[1]=y[1];
		        y[1]=temp1;
		        temp1=x[2];
		        x[2]=y[2];
		        y[2]=temp1;
		        temp1=x[3];
		        x[3]=y[3];
		        y[3]=temp1;

		        y[0]=Calibration_MAX-y[0];
		        y[1]=Calibration_MAX-y[1];
		        y[2]=Calibration_MAX-y[2];
		        y[3]=Calibration_MAX-y[3];
                tp_mode=0x06;

	        }
	        else  if((y[0]<y[1])&&(x[0]<x[2]))
            {

                temp1=x[0];
		        x[0]=y[0];
		        y[0]=temp1;
		        temp1=x[1];
		        x[1]=y[1];
		        y[1]=temp1;
		        temp1=x[2];
		        x[2]=y[2];
		        y[2]=temp1;
		        temp1=x[3];
		        x[3]=y[3];
		        y[3]=temp1;

		        x[0]=Calibration_MAX-x[0];
		        x[1]=Calibration_MAX-x[1];
		        x[2]=Calibration_MAX-x[2];
		        x[3]=Calibration_MAX-x[3];

		        y[0]=Calibration_MAX-y[0];
		        y[1]=Calibration_MAX-y[1];
		        y[2]=Calibration_MAX-y[2];
		        y[3]=Calibration_MAX-y[3];
                tp_mode=0x07;

	        }
	        else  if((y[0]<y[1])&&(x[0]>x[2]))
            {

                temp1=x[0];
		        x[0]=y[0];
		        y[0]=temp1;

		        temp1=x[1];
		        x[1]=y[1];
		        y[1]=temp1;

		        temp1=x[2];
		        x[2]=y[2];
		        y[2]=temp1;

		        temp1=x[3];
		        x[3]=y[3];
		        y[3]=temp1;

		        x[0]=Calibration_MAX-x[0];
		        x[1]=Calibration_MAX-x[1];
		        x[2]=Calibration_MAX-x[2];
		        x[3]=Calibration_MAX-x[3];
		        tp_mode=0x08;

	        }
	    }
		x[0]=(x[0]+x[2])/2;
		x[1]=(x[1]+x[3])/2;
		y[0]=(y[0]+y[1])/2;
		y[2]=(y[2]+y[3])/2;
		tp_left=x[0]+((x[0]-x[1])*c_x)/(xb-xa);
		tp_right=(x[1]>=(((x[0]-x[1])*c_x)/(xb-xa)))?(x[1]-((x[0]-x[1])*c_x)/(xb-xa)):0;
		tp_up=y[0]+((y[0]-y[2])*c_y)/(ya-yc);
		tp_down=(y[2]>=(((y[0]-y[2])*c_y)/(ya-yc)))?(y[2]-((y[0]-y[2])*c_y)/(ya-yc)):0;
		return 1;
	}
	else
	{
		return 0;
	}
}
#define bmp_offset 0x0d

void do_calibration (unsigned char select)
{
	unsigned char rc=0;
	unsigned short x[4]={0};
	unsigned short y[4]={0};
	unsigned char count=0x00;
	unsigned short x_l,x_r,y_u,y_d;
	unsigned short re_x = 0x00;
	unsigned short re_y = 0x00;
	touchd co_value={0,0,0};
	unsigned int tm_out = 0; //timer
	unsigned char osd2_temp = 0;
	unsigned short target_x;
	unsigned short target_y;

    c_w = readb(DWHSZ_HI)<<8 | readb(DWHSZ_LO);
    c_h = readb(DWVSZ_HI)<<8 | readb(DWVSZ_LO);
	osd2_temp = osd2_cfg_read(0x01);// store current vale
	osd2_cfg_wr(0x01, 0x12);// disable more left (setting default value)

	drawtarget(select);

	x_l=c_x;	//200
	x_r=c_w-c_x;	 //600
	y_u=c_y;		//120
	y_d=c_h-c_y;	//360
	//target_x = ((x_l+x_r)/2)+((x_l+x_r)/4);
	//target_y = ((y_u+y_d)/2)+((y_u+y_d)/4);
	target_x = x_l+((c_w/2)-x_l)/2;
	target_y = y_u+((c_h/2)-y_u)/2;

	printf("x_l =%d x_r = %d y_u = %d y_d = %d\n",x_l,x_r,y_u,y_d);
	printf("target_x = %d target_y = %d\n",target_x,target_y);

	if(calibration_finish==1)
		return;

	watchdog_keepalive(30000);

	/* Set BMP H/V Start Position */
	_osd2_bmp_pos((x_l-diff)+bmp_offset, (y_u-diff));
	osd2_cfg_wr(OSD_CTRL, 0x80);
	//osd2_enable();

	tm_out = read_c0_count32() + millisec(20000);

	while(count<=4)
	{
		if(is_time_out(tm_out))
			break;// timeout
		co_value=touch_read();
		if(((co_value.x!=0) || (co_value.y!=0)) && (co_value.status==0))
		{
			dbg(0,"co_value x = %x y = %x\n",co_value.x,co_value.y);
			count++;
			switch(count)
			{
				case 1:
					x[0]=co_value.x;
					y[0]=co_value.y;
					re_x=co_value.x;
					re_y=co_value.y;

					osd2_bmp_star_position((x_r-diff)+bmp_offset,y_u-diff);//for second points
					break;
				case 2:
					if((sub(co_value.x,re_x)>=50)||(sub(co_value.y,re_y)>=50))
					{
						x[1]=co_value.x;
						y[1]=co_value.y;
						re_x=co_value.x;
						re_y=co_value.y;

						osd2_bmp_star_position((x_l-diff)+bmp_offset,y_d-diff);//for third points
					}
					else
					{
						count=1;
					}
					break;
				case 3:
					if((sub(co_value.x,re_x)>=50)||(sub(co_value.y,re_y)>=50))
					{
						x[2]=co_value.x;
						y[2]=co_value.y;
						re_x=co_value.x;
						re_y=co_value.y;

						osd2_bmp_star_position((x_r-diff)+bmp_offset,y_d-diff);//for fourth points
					}
					else
					{
						count=2;
					}
					break;
				case 4:
					if((sub(co_value.x,re_x)>=50)||(sub(co_value.y,re_y)>=50))
					{
						x[3]=co_value.x;
						y[3]=co_value.y;
						rc=calibration(x,y);
						re_x=co_value.x;
						re_y=co_value.y;
						if(rc!=0)
						{
							osd2_bmp_star_position((target_x-diff)+bmp_offset,(target_y-diff));
							while(1){
								co_value=touch_read();
								if(is_time_out(tm_out))
									break;
								if((sub(co_value.x,re_x)>=50)||(sub(co_value.y,re_y)>=50)){
									if(((co_value.x!=0) || (co_value.y!=0)) && (co_value.status==0)){
										if(check_correct_position(co_value.x, co_value.y,target_x,target_y)>=0){
											count=5;
											osd2_disable();
											osd2_bmp_disable();
											calibration_finish=1;
											TouchSave();
										}else{
											count=0;
											osd2_bmp_star_position((x_l-diff)+bmp_offset,(y_u-diff));//first point
										}
										break;
									}
								}
							}
							
						}
						else
						{
							count=0;
							osd2_bmp_star_position((x_l-diff)+bmp_offset,(y_u-diff));//first point
						}
						re_x=co_value.x;
						re_y=co_value.y;
					}
					else
					{
						count=3;
					}
					break;
			}
		}
	}
	if(calibration_finish == 0){
		save_default_value();
		dbg(0,"Calibration load default value\n");
	}else{
		dbg(0,"Calibration Successful\n");
	}
	osd2_cfg_wr(0x01, osd2_temp);//restore value
	osd2_disable();
	osd2_bmp_disable();

}

void calib_diff_threshold(int value)
{
	diff_threshold = value;
	printf("calibration diff threshold = %d",diff_threshold);
}
