#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <io.h>
#include <shell.h>
#include <sys.h>
#include <serial.h>
#include <mipsregs.h>
#include <unistd.h>
#include "irq.h"
#include "interrupt.h"
#include "display.h"
#include <drivers/scaler/scaler.h>
#include <registry.h>
#include <drivers/calibration/calibration.h>
#include <drivers/ir/ir.h>
#include <drivers/sarkey/sarkey.h>
#include <drivers/touch/touch.h>
#include <drivers/cq/cq.h>
#include <drivers/osd2/osd2.h>
#include "reg_tw.h"

command_init (sh_touch_resolution, "touch_resolution","touch_resolution");
command_init (sh_calib_threshold, "calib_threshold","calib_threshold");
command_init (sh_calib, "calib","calib");
command_init (sh_touch, "touch","touch");
command_init (sh_ir, "ir","ir");
command_init (sh_sarkey, "sarkey","sarkey");
command_init (sh_touch_test, "touch_test","touch_test");
command_init (sh_sarkey_test, "sarkey_test","sarkey_test");
command_init (sh_bmp_draw_test, "bmp_draw_test","bmp_draw_test");
command_init (sh_touch_draw_test, "touch_draw_test","touch_draw_test");

#define SAR1_LTCH_VAL 0xb0403470
#define SAR1_RVALUE   0xb0403440
#define SAR2_LTCH_VAL 0xb0403540


#define diff 20
static int sh_bmp_draw_test(int argc, char **argv, void **result, void *sys_ctx)
{
	char ch = 0;
	int x =20; 
	int y =20;
	int bmp_offset = 0;
	
	if(argc ==2){
		bmp_offset=atoi(argv[1]);
	}else{
		bmp_offset=0x0d;//default
	}
	printf("osd2 offset = %d\n",bmp_offset);
	writel(0x87,0xb0400910);
	//draw_target();
	draw_target_test();
	_osd2_bmp_pos((x-diff)+bmp_offset, (y-diff));
	while(1){
		ch = getb2();
		switch (ch){
			case 'w':
				y++;
				_osd2_bmp_pos((x-diff)+bmp_offset, (y-diff));
				printf("x= %d y= %d\n",x,y);
				break;
			case 's':
				y--;
				_osd2_bmp_pos((x-diff)+bmp_offset, (y-diff));
				printf("x= %d y= %d\n",x,y);
				break;
			case 'a':
				x--;
				_osd2_bmp_pos((x-diff)+bmp_offset, (y-diff));
				printf("x= %d y= %d\n",x,y);
				break;
			case 'd':
				x++;
				_osd2_bmp_pos((x-diff)+bmp_offset, (y-diff));
				printf("x= %d y= %d\n",x,y);
				break;
			case 'q':
				goto  EXIT;
				break;
		}
	}		
EXIT:
	return 0;
}

static int sh_touch_resolution(int argc, char **argv, void **result, void *sys_ctx)
{
	int value = 0;
	int ret = 0;
	if(argc ==2){
		value = atoi(argv[1]);
	}else{
		printf("error parameter\n");
		return 0;
	}
	if((value == 1024) || (value == 2048) || (value == 4096)){
		ret = touch_set_resolution(value);
		printf("resolution value = %d \n",value);
	}else{
		printf("Please set 1024 , 2048 or 4096\n");
	}
	return 0;
}

static int sh_calib_threshold(int argc, char **argv, void **result, void *sys_ctx)
{
	int value = 0;
	//int ret = 0;
	if(argc ==2){
		value = atoi(argv[1]);
	}else{
		printf("error parameter\n");
		return 0;
	}
	return 0;
}



static int sh_ir(int argc, char **argv, void **result, void *sys_ctx)
{
	int ir_value = 0x00;
	int ch = 0;
    while(1){
	
		ir_value = IRread();
		if(ir_value!=0){
			printf("ir value=%x\n",ir_value);
		}
        ch = getb2();
        if ((ch == 'q') || (ch == 'Q')) {
			break;
		}
    }
    return 0;	
}

static int sh_sarkey(int argc, char **argv, void **result, void *sys_ctx)
{
	int sar1_value = 0x00;
	int sar2_value = 0x00;
	int ch = 0;
    while(1){
		sar1_value = sar1_read();
		sar2_value = sar2_read();
		if(sar1_value!=0){
			printf("sar1 value=%x\n",sar1_value);
		}
		if(sar2_value!=0){
			printf("sar2 value=%x\n",sar2_value);
		}
        ch = getb2();
        if ((ch == 'q') || (ch == 'Q')) {
			break;
		}
    }
    return 0;	
}

static int sh_calib(int argc, char **argv, void **result, void *sys_ctx)
{
	writel(0x87,0xb0400910);
	osd2_init();		// for UI
	do_calibration(FORCE_TP_CALIBRA); //calibration
    return 0;	
}

static int sh_touch(int argc, char **argv, void **result, void *sys_ctx)
{
    touchd  tv     = {0, 0, 0};
	int ch = 0;
	writel(0x87,0xb0400910);
	osd2_init();		// for UI
	do_calibration(CHECK_IFNOT_CALIBRA); //calibration
    while(1){
        tv = touch_read();
		if(tv.status!=3)
			printf("touch x=%d y=%d sts=%d\n", (int)tv.x, (int)tv.y, (int)tv.status);
        ch = getb2();
        if ((ch == 'q') || (ch == 'Q')) {
			break;
		}
    }
    return 0;
}

static int sh_touch_draw_test(int argc, char **argv, void **result, void *sys_ctx)
{
	touch_targe_test();
    return 0;
}

static int sh_touch_test(int argc, char **argv, void **result, void *sys_ctx)
{
	int bounce = 0;
	int pend_x = 0, pend_y =0; 
	int ch = 0;
	touchd  t_value = {0, 0, 0};
	calibration_finish = 0;
	printf("Touch Panel Test Code()\r\n");
	while(1){
		t_value=touch_read();
		if((t_value.x!=0)||(t_value.y!=0)) {
			if(t_value.status==0){
				printf("x=%d y=%d status=Pen Down\r\n",(int)t_value.x,(int)t_value.y);
				pend_x = (int)t_value.x; 
				pend_y = (int)t_value.y;
			}
			if(t_value.status==1)
				printf("x=%d y=%d status=Move\r\n",(int)t_value.x,(int)t_value.y);
			if(t_value.status==2){
				printf("x=%d y=%d status=Pen Release\r\n",(int)t_value.x,(int)t_value.y);
				pend_x = pend_x-((int)t_value.x);
				pend_y = pend_y-((int)t_value.y);
				bounce = readb(0xb0403740);
				//Clear Bounce
				writel((readb(0xb0403720)|0x10),0xb0403720);
				writel((readb(0xb0403720)&0xEF),0xb0403720);			
				//IC_WritByte(TWIC_P3,0x52,IC_ReadByte(TWIC_P3,0x52)|0x10);
				//IC_WritByte(TWIC_P3,0x52,IC_ReadByte(TWIC_P3,0x52)&0xEF);

				printf("Pen Down/Release Diff x=%d y=%d , Bounce Remove:%d\r\n\r\n\r\n",(int)pend_x,(int)pend_y,bounce);
			}
		}
		ch = getb2();
        if ((ch == 'q') || (ch == 'Q')) {
			break;
		}
	}
	return 0;
}

static int sh_sarkey_test(int argc, char **argv, void **result, void *sys_ctx)
{
	unsigned char keycode1;
	unsigned char keycode2;
	int ch = 0;
    while(1){
		keycode1=readb(SAR1_RVALUE);
		keycode2=readb(SAR2_LTCH_VAL);
		if(keycode1!=0xff){
			printf("sar1 value=%x\n",keycode1);
		}
		if(keycode2!=0xff){
			printf("sar2 value=%x\n",keycode2);
		}
        ch = getb2();
        if ((ch == 'q') || (ch == 'Q')) {
			break;
		}
    }
    return 0;	
}
