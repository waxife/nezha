/**
 *  @file   sh_sosd.c
 *  @brief  spiosd demo for Kirin environment
 *  $Id: sh_ui.c,v 1.5 2014/01/22 01:45:19 kevin Exp $
 *  $Authr: $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <cache.h>
#include <stdlib.h>
#include <serial.h>
#include "display.h"
#include "debug.h"
#include <sys/include/display.h>

#include "rc.h"
#include "tw_widget_sys.h"
#include "reg_tw.h"
#include "./drivers/spiosd/spiosd.h"
#include "./drivers/spirw/spirw.h"
#include "./drivers/osd2/osd2.h"
#include <drivers/cq/cq.h>
#include "./drivers/scaler/scaler.h"

#include "res.h"
#include "page_A_COVER_png.h"
#include "page_B_COVER_png.h"


#define REG_SCALAR_BASE 0xb040000
#define PAGE0(addr)  (0xb0400000 | (addr << 4))
#define PAGE1(addr)  (0xb0401000 | (addr << 4))
#define PAGE2(addr)  (0xb0402000 | (addr << 4))
#define PAGE3(addr)  (0xb0403000 | (addr << 4))


command_init (sh_ui, "ui", "demo spiosd ui <choose, 0: 582D , 1: 582A >");

static struct tw_spr spr_main_png[10];
static char str[10][30]={"RECORD_2012/05/20","SNAPSHOT_2012/05/20","MESSAGE_2012/05/20","PLAY_2012/05/20","PICVIEW_2012/05/23","SETTING_2012/05/23","TIME_2012/05/23","LANGUAGE_2012/05/23","BACKUP_ 2012/05/23","FORMAT_ 2012/05/23"};
static struct tw_icon icon_main_png[7];
static struct tw_icon icon_slave_png[5];

static unsigned char page = 0;

static void sprite_array_construct()
{
    spr_main_png[0] = page_a_cover_png_record_png;
    spr_main_png[1] = page_a_cover_png_snapshot_png;
    spr_main_png[2] = page_a_cover_png_message_png;
    spr_main_png[3] = page_a_cover_png_play_png;
	spr_main_png[4] = page_a_cover_png_picview_png;
    spr_main_png[5] = page_a_cover_png_settings_png;
    spr_main_png[6] = page_b_cover_png_time_png;
    spr_main_png[7] = page_b_cover_png_langb_png;
    spr_main_png[8] = page_b_cover_png_backup_png;
	spr_main_png[9] = page_b_cover_png_format_png;
};

static void tw_icon_array_construct()
{
	icon_main_png[0]  = page_a_cover_png_green_resource_data_png4_png_1;//check
	icon_main_png[1]  = page_a_cover_png_green_resource_data_png1_png_1;//play
	icon_main_png[2]  = page_a_cover_png_green_resource_data_png5_png_1;//pause
	icon_main_png[3]  = page_a_cover_png_orange_resource_data_png1_png_1;//x
	icon_main_png[4]  = page_a_cover_png_orange_resource_data_png0_png_1;//message
	icon_main_png[5]  = page_a_cover_png_orange_resource_data_png2_png_1;//record
    icon_main_png[6]  = page_a_cover_png_orange_resource_data_png4_png_2;//people
	
	icon_slave_png[0] = page_b_cover_png_green_resource_data_png1_png_2;//play
	icon_slave_png[1] = page_b_cover_png_green_resource_data_png4_png_2;//check
	icon_slave_png[2] = page_b_cover_png_green_resource_data_png5_png_2;//pause
	icon_slave_png[3] = page_b_cover_png_orange_resource_data_png1_png_2;//x
	icon_slave_png[4] = page_b_cover_png_green_resource_data_png0_png_1;//executing
}


static unsigned char bar_index = 0x00;
static unsigned char bar_length = 0x00;
static unsigned char language = 0x01;

static void draw_bar_outline(unsigned char x,unsigned char y,unsigned char length)
{
	unsigned char i = 0;
	bar_length=length;
	_icon_draw_at(page_a_cover_png_right_png.index, (page_a_cover_png_right_png.color)&0xf, x-1, y, 1, 1,0x1bc5, 50, 20, 0, 0);//left
	_icon_draw_at(page_a_cover_png_left_png.index, (page_a_cover_png_left_png.color)&0xf, x+length, y, 1, 1,0x1bc5, 50, 20, 0, 0);//right
	for(i=0;i<length;i++)
	{
		_icon_draw_at(page_a_cover_png_down_png.index, (page_a_cover_png_down_png.color)&0xf, x+i, y-1, 1, 1,0x1bc5, 50, 20, 0, 0);
		_icon_draw_at(page_a_cover_png_up_png.index, (page_a_cover_png_up_png.color)&0xf, x+i, y+1, 1, 1,0x1bc5, 50, 20, 0, 0);
	}
}
static void draw_bar_direct(unsigned char x,unsigned char y,unsigned char direct)
{
	if(direct==0)//left
	{
		if(bar_index>0)
		{
			bar_index--;
			osd2_clear_block(x+bar_index,y, 1, 1, 0x1bc5, 50);
		}
	}
	else if(direct==1)//right
	{
		if(bar_index<bar_length)
		{
			_icon_draw_at(page_a_cover_png_fill_png.index, (page_a_cover_png_fill_png.color)&0xf, x+bar_index, y, 1, 1,0x1bc5, 50, 20, 0, 0);
			bar_index++;
		}
	}
}

static char ascii_num(char *value)
{
	char data=*value;
	if(*value==0x5f)//0x5f = '_'
	{
		data=0x00;
	}
	else if((*value)<=0x3a&&(*value)>=0x2d)//0x2d ~0x39  -./0123456789
	{
		data-=0x2d;//0X39
		data+=0x01;
	}
	else if((*value)<=0x5a&&(*value)>=0x41)//0x41~0x5a ABCD........Z
	{
		data-=0x41;
		data+=0x0F;//0X0E
	}
	else
	{
		data=0x7f;// space
	}
	return data;
}
static void draw_font(char *str,unsigned char x,unsigned char y,unsigned char blinkmode,unsigned char color)
{
	unsigned char i=0;
	unsigned char count=strlen(str);
	unsigned short data = 0x00;
	unsigned char value = 0x00;
	struct tw_icon letter;
	osd2_clear_block(x,y, count, 1, 0x1bc5, 50);
	
	if(page == 0x00)
		letter=page_a_cover_png_letter_eng_arial_0_png;
	else
		letter=page_b_cover_png_letter_eng_arial_0_png_1;
	for(i=0;i<0xff;i++)
	{
		if(str[i]==0)
			break;
		value = ascii_num(&str[i]);
		if(value!=0x7f)
			data = letter.index+value;
		else
			data = 0x00;
		if(color==0x00)
			_icon_draw_at(data, (letter.color)&0xf, x+i, y, 1, 1,0x1bc5, 50, 0, 20, blinkmode);
		else
			_icon_draw_at(data, ((letter.color)>>4)&0xf, x+i, y, 1, 1,0x1bc5, 50, 0, 20, blinkmode);
	}
}

static void draw_icon_at(struct tw_icon *icon,unsigned char x,unsigned char y,unsigned char blinkmode)
{
	_icon_draw_at(icon->index, icon->color, x, y, icon->width, icon->height,0x1bc5, 50, 0, 20, blinkmode);
}

static void handler(char *str,unsigned char x,unsigned char y,unsigned char choice)
{
	unsigned char rb = 0xff;
	unsigned char value = 0x00;
	osd2_clear_menu(0x1bc5,50,20);
	sosd_spr_disable();
	sosd_disable();
	printf("choice=%d\n",choice);
	while(1)
	{
		switch(choice)
		{
			case 0:
				draw_icon_at(&icon_main_png[0],0,0,1);
				draw_font(str,x,y,0,0);
			break;
			case 1:
				draw_icon_at(&icon_main_png[1],0,0,1);
				draw_font(str,x,y,1,1);
			break;
			case 2:
				draw_icon_at(&icon_main_png[2],0,0,1);
				draw_font(str,x,y,0,0);
			break;
			case 3:
				draw_icon_at(&icon_main_png[3],0,0,1);
				draw_font(str,x,y,1,1);
			break;
			case 4:
				draw_icon_at(&icon_main_png[4],0,0,1);
				draw_font(str,x,y,0,0);
			break;
			case 5:
				draw_icon_at(&icon_main_png[5],0,0,1);
				draw_font(str,x,y,1,1);
			break;
			case 6:
				draw_icon_at(&icon_slave_png[0],0,0,1);
				draw_font(str,x,y,0,0);
			break;
			case 7:
				if(language==1)
					draw_icon_at(&icon_slave_png[0],8,11,0);
				else if(language==2)
					draw_icon_at(&icon_slave_png[0],8,13,0);
				draw_font("CHINESE",10,12,1,0);//English 
				draw_font("ENGLISH",10,14,1,0);//Chinese
				while(1)
				{
					printf("Please Enter 1 or 2 to Select Language and  Enter 3 to Exit\n");
					rb = getb();
					if(rb=='1')
					{
						osd2_clear_block(8, 13, 2, 2, 0x1bc5, 50);
						draw_icon_at(&icon_slave_png[0],8,11,0);
						set_iconset_num(1);
						language=1;
					}
					else if(rb=='2')
					{
						osd2_clear_block(8, 11, 2, 2, 0x1bc5, 50);
						draw_icon_at(&icon_slave_png[0],8,13,0);
						set_iconset_num(2);
						language=2;
					}
					else if(rb=='3')
						break;
				}
			break;
			case 8:
				draw_icon_at(&icon_slave_png[4],0,0,1);
				draw_font(str,x,y,0,0);
				break;
			case 9:
				draw_icon_at(&icon_slave_png[4],0,0,1);
				draw_font(str,x,y,0,0);
			break;
			default:
			break;
		}
		osd2_blink(0,2);
		if(rb!='3')
		{
			printf("Enter 2 To Eixt");
			value=getb();
		}
		if(value=='2'||rb=='3')
		{
			osd2_disable();
			if(choice<0x06)
			{
				tw_menu_draw((struct tw_menu *)&page_a_cover_png_osd2_1_menu);
				bar_index = 0;
				draw_bar_outline(10,17,30);
			}
			else
			{
				tw_menu_draw((struct tw_menu *)&page_b_cover_png_osd2_2_menu);
			}
			osd2_menu_location (0, 1, 0x1bc5, 50, 20);
			sosd_enable(1);
			sosd_spr_enable(1);
			osd2_enable();
			break;
		}
	}
}

static void Marquee_demo(unsigned short *letter,unsigned char count,int delay)
{
	unsigned char time = 0x00;
	struct tw_scl_txt twrun;
	tw_menu_reset_index();
	tw_text_draw((struct tw_txt *)&page_b_cover_png_icon_loc_1bp_font_3,letter);//start x= y=0 direct right
	twrun=tw_init_scrol((struct tw_txt *)&page_b_cover_png_icon_loc_1bp_font_3, letter, 0, 1);
	for(time=0;time<count;time++)
	{
		tw_scrolling(&twrun);
		usleep(delay);
	}
}

static void t582d_sosd_control_init(void)
{
    writel(0x80, PAGE3(0xB8));  // ospi_h_freq_clk
    writel(0x03, PAGE3(0xAD));  // uspi_4x, dspi_4x
    writel(0x01, PAGE3(0xB5));  // ospi4x
}


static int sh_ui(int argc, char **argv, void **result, void *sys_ctx)
{
    unsigned char m_select = 0;//main
	unsigned char s_select = 0;//SECOND
    unsigned char rb = 0xff;
	unsigned char count = 0x00;
	unsigned short letter[50]={0x5b8f,0x82af,0x79d1,0x6280,'T','E','R','A','W','I','N','S',0};
	
	int spiosd_h_start = 0xEE;
	int spiosd_v_start = 0x23;
	int spiosd_spr_h_start = 0x55;
	int spiosd_spr_v_start = 0x1E;
	
	static int video_format = VIDEO_STD_NTSC;
	
    scaler_open();
    
	cq_init();
  
    cq_trigger_source (CQ_TRIGGER_SW);
	
	osd2_init();
    
	display_init();
    display_set_cvbs_full(video_format, CH0);

    enable_quad_io();
    resource_init();

    spiosd_init(spiosd_h_start, spiosd_v_start, spiosd_spr_h_start, spiosd_spr_v_start);
    t582d_sosd_control_init();
	

    printf("T582D spiosd main page\r\n");
 

	tw_td_draw((struct tw_td *)&layout_1_a_cover_png);

    sosd_enable(0);

	osd2_disable();
    osd2_ram_clear();

	tw_menu_draw((struct tw_menu *)&page_a_cover_png_osd2_1_menu);

	osd2_menu_location(0, 1, 0x1bc5, 50, 20);
	osd2_enable();
	
	printf("1 -> Move Sprite\n");
	printf("2 -> Enter \n");
	printf("3 -> Change Page\n");
	printf("4 -> Scroll Left");
	printf("5 -> Scroll Right");
	printf("x -> Exit\n");
	printf("Press command: ");
	sprite_array_construct();
	tw_icon_array_construct();
	draw_bar_outline(10,17,30);
	
	while(1)
	{
		rb = getb();
		printf("%c\n", rb);
		switch(rb)
		{
			case '1'://MOVE
				if(page == 0)
				{
					sosd_spr_disable();
					tw_spr_draw(&spr_main_png[m_select]);
					sosd_spr_enable(1);
					m_select++;
					m_select=m_select%6;
				}
				else
				{
					sosd_spr_disable();
					tw_spr_draw(&spr_main_png[s_select+6]);
					sosd_spr_enable(1);
					s_select++;
					s_select=s_select%4;
				}
			break;
			case '2'://enter page
				if(page == 0)
				{
					if(((m_select+5)%6) == 5)//change page
					{
						page++;
						page%=2;
						osd2_disable();
						osd2_clear_menu(0x1bc5,50,20);
						sosd_spr_disable();
						sosd_disable();
						tw_menu_draw((struct tw_menu *)&page_b_cover_png_osd2_2_menu);
						osd2_menu_location (0, 1, 0x1bc5, 50, 20);
						tw_td_draw((struct tw_td *)&layout_2_b_cover_png);
						sosd_enable(1);
						osd2_enable();
						bar_index = 0;
					}
					else
					{
						handler(str[(m_select+5)%6],5,0,((m_select+5)%6));//0~5
					}
				}
				else
				{
					handler(str[(s_select+3)%4+6],5,0,((s_select+3)%4+6));
				}
			break;
			case '3'://switch main to second page
				page++;
				page%=2;
				if(page == 0)
				{
					osd2_disable();
					osd2_clear_menu(0x1bc5,50,20);
					sosd_spr_disable();
					sosd_disable();//
					tw_menu_draw((struct tw_menu *)&page_a_cover_png_osd2_1_menu);
					osd2_menu_location(0, 1, 0x1bc5, 50, 20);
					tw_td_draw((struct tw_td *)&layout_1_a_cover_png);
					sosd_enable(1);//
					osd2_enable();
					bar_index = 0;
					draw_bar_outline(10,17,30);
				}
				else
				{
					osd2_disable();
					osd2_clear_menu(0x1bc5,50,20);
					sosd_spr_disable();
					sosd_disable();//
					tw_menu_draw((struct tw_menu *)&page_b_cover_png_osd2_2_menu);
					osd2_menu_location (0, 1, 0x1bc5, 50, 20);
					tw_td_draw((struct tw_td *)&layout_2_b_cover_png);
					count++;
					sosd_enable(1);
					osd2_enable();
					Marquee_demo(letter,100,100000);
				}
			break;
			case '4':
				draw_bar_direct(10,17,0);
				break;
			case '5':
				draw_bar_direct(10,17,1);
				break;
			case 'x':
					return 0;
			break;
			default:
				break;
		}
	}
    return 0;
}
