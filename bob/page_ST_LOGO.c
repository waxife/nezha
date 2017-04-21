#include <unistd.h>
#include <debug.h>
#include <io.h>

#include "tw_widget_sys.h"
#include "OSD/res.h"
#include "drivers/spiosd/spiosd.h"
#include "drivers/osd2/osd2.h"
#include "task/pview_service.h"

#include "OSD/page_ST_LOGO.h"
#include "sys/include/mconfig.h"
#include "drivers/cq/cq.h"
#include "sys/include/watchdog.h"
//#include "sys/include/sd.h"
#include <time.h>
#include "sys/include/norfs.h"
#include "userdata.h"


unsigned char cur_flag = 0; /*record current cursor status*/
//unsigned char dis_flag = 0;
unsigned char quit_flag = 0;
unsigned char br_flag = 0;
unsigned char co_flag = 0;

enum
{
	BRIGHTNESS_D = 0,
	CONTRAST_D
};

unsigned char br_val, co_val;

static int
disp_set_brightness (int argc)
{
    cq_write_byte_issue(CQ_P0, 0x69, argc, CQ_TRIGGER_SW);
	printf("current brightness is %d\r\n", argc);
    return 0;
}

static int
disp_set_contrast (int argc)
{
	cq_write_byte_issue(CQ_P0, 0x68, argc, CQ_TRIGGER_SW);
	printf("current contrast is %d\r\n", argc);
    return 0;
}

/*br_or_co = 0 -> brightness; br_or_co = 1 -> contrast*/
void update_data(unsigned char br_or_co, unsigned char data, unsigned char bit)
{
	switch(data)
	{
		case 0:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_1, 21+bit, 7+2*br_or_co);/*0*/
		break;
		case 1:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_2, 21+bit, 7+2*br_or_co);/*1*/
		break;
		case 2:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_3, 21+bit, 7+2*br_or_co);/*2*/
		break;
		case 3:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_4, 21+bit, 7+2*br_or_co);/*3*/
		break;
		case 4:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_5, 21+bit, 7+2*br_or_co);/*4*/
		break;
		case 5:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_6, 21+bit, 7+2*br_or_co);/*5*/
		break;
		case 6:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_7, 21+bit, 7+2*br_or_co);/*6*/
		break;
		case 7:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_8, 21+bit, 7+2*br_or_co);/*7*/
		break;
		case 8:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_9, 21+bit, 7+2*br_or_co);/*8*/
		break;
		case 9:
		tw_icons_loc((struct tw_icons*)&page_img_rom_font_10, 21+bit, 7+2*br_or_co);/*9*/
		break;
		default:
		dbg(2,"error data\r\n");
		break;
	}
}


/*udpate the brightness and contrast value*/
int update_bc(unsigned char br, unsigned char co)
{
	//unsigned char p_table[1];
	//int temp;
	unsigned char unit_d[3];
	unsigned char i;
	
	//struct tw_icons* h, *t, *u;
	tw_icons_loc((struct tw_icons*)&page_img_rom_font_11, 17, 7);/*bright*/
	tw_icons_loc((struct tw_icons*)&page_img_rom_font_12, 17, 9);/*contrast*/
	tw_icons_loc((struct tw_icons*)&page_img_rom_font_13, 17, 11);/*exit*/

	//addr_search(br, h, t, u);
	unit_d[2] = br%10;
	unit_d[1] = br/10%10;
	unit_d[0] = br/100%10;
	//dbg(2, "h = %d, t = %d, u = %d\r\n", unit_d[0],unit_d[1],unit_d[2] );

	for(i=0;i<3;i++)
	update_data(BRIGHTNESS_D, unit_d[i], i);

	unit_d[2] = co%10;
	unit_d[1] = co/10%10;
	unit_d[0] = co/100%10;

	//dbg(2, "h = %d, t = %d, u = %d\r\n", unit_d[0],unit_d[1],unit_d[2] );

	for(i=0;i<3;i++)
	update_data(CONTRAST_D, unit_d[i], i);

	osd2_update();
	
	return 0;
}



unsigned char page_ST_LOGO_process (TW_EVENT* event)
{

	//unsigned char twsrv_id = 0, twsrv_key = 0;
	//int rc;
	//char a,b;
	//struct tm tm1;
	struct pviewsrv_opt_t pview_obj;
	pview_obj.dev = FATFS;
	pview_obj.mark = 0;

	switch(event->type)
	{
		case TW_EVENT_TYPE_ENTER_SYSTEM:
			dbg(2, ">>>>> Enter Menu Page\n\r");
			//dbg(2, "page_img_rom_font_1 address = %x\r\n", &page_img_rom_font_1);
			watchdog_disable();
		break;

		case TW_EVENT_TYPE_KEY_DOWN:
			
			dbg(2, ">>>>> key down happen\n\r");
			dbg(2, ">>>>> key value = %d\n\r", event->keystroke.ch);

			switch(event->keystroke.ch)
			{
				case 3: /*start snap service  key up*/
						if(!cur_flag){
							tw_menu_draw((struct tw_menu*)&page_img_osd2_0_menu);
							osd2_clear_menu(0x1bc5,50,20);
							osd2_enable();

							snap_srv(&pview_obj);

							tw_icons_loc((struct tw_icons*)&page_img_rom_font_131, 5, 1);/*display star*/
							tw_icons_loc((struct tw_icons*)&page_img_rom_font_131, 6, 1);/*display star*/
							tw_icons_loc((struct tw_icons*)&page_img_rom_font_131, 7, 1);/*display star*/

							pview_srv((void *)&pview_obj);

							display_set_cvbs_full(2,0);
							cvbs2_bluescreen_on(0, 0, 0, 0);
							
							osd2_disable();
							
							dbg(2, "snap service done\r\n");
						}
						if (br_flag){
							if(br_val < 255)
							br_val++;
							else
							br_val = 255;
							disp_set_brightness(br_val); /*brightness from 0 to 255*/
							dbg(2, "br +\r\n");
							update_bc(br_val,co_val);
							user_datas[E_BRIGHTNESS] = br_val;
							dbg(2, "brightness set -> %d\r\n", user_datas[E_BRIGHTNESS]);
							save_userdata(E_BRIGHTNESS);
						}
						if (co_flag){
							if(co_val < 255)
							co_val++;
							else
							co_val = 255;
							disp_set_contrast(co_val);/*contrast from 0 to 255*/
							dbg(2, "co +\r\n");
							update_bc(br_val,co_val);
							user_datas[E_CONTRAST] = co_val;
							dbg(2, "contrast set -> %d\r\n", user_datas[E_CONTRAST]);
							save_userdata(E_CONTRAST);
						}
						if((cur_flag==1)&&(quit_flag==1))/*when cursor at "exit", press snap key, then format coard*/
						{
							format_sdcared();
							dbg(2, "format SD done\r\n");
							//cq_write_byte_issue(CQ_P0, 0x91, 0xB8, CQ_TRIGGER_SW);
						}
	
					break;
				case 1: /*Review*/
						if(!cur_flag){
							tw_menu_draw((struct tw_menu*)&page_img_osd2_0_menu);
							osd2_clear_menu(0x1bc5,50,20);
							osd2_enable();
							tw_icons_loc((struct tw_icons*)&page_img_rom_font_131, 5, 1);/*display star*/
							tw_icons_loc((struct tw_icons*)&page_img_rom_font_131, 6, 1);/*display star*/
							tw_icons_loc((struct tw_icons*)&page_img_rom_font_131, 7, 1);/*display star*/
							
							pview_srv((void *)&pview_obj);

							osd2_disable();
							/*to liveview*/
							//display_init();
							//display_bklight_on();
							display_set_cvbs_full(2,0);
							cvbs2_bluescreen_on(0, 0, 0, 0);
							dbg(2, "liveview display again\r\n");
						}
						if((cur_flag==1)&&(quit_flag==1))
						{
							cur_flag = 0;
							dbg(2,"exit osd, \r\n");
							osd2_disable();
						}
						
					break;
				case 2: /*key down*/
					
					if (br_flag){
						if(br_val > 0)
						br_val--;
						disp_set_brightness(br_val);
						dbg(2, "br -\r\n");
						update_bc(br_val,co_val);
						user_datas[E_BRIGHTNESS] = br_val;
						dbg(2, "brightness set -> %d\r\n", user_datas[E_BRIGHTNESS]);
						save_userdata(E_BRIGHTNESS);
					}
					if (co_flag){
						if(co_val > 0)
						co_val--;
						disp_set_contrast(co_val);
						dbg(2, "co -\r\n");
						update_bc(br_val,co_val);
						user_datas[E_CONTRAST] = co_val;
						dbg(2, "contrast set -> %d\r\n", user_datas[E_CONTRAST]);
						save_userdata(E_CONTRAST);
					}

					break;

				case 5:/*Menu & confirm key*/
					dbg(2, ">>>>>>ddddd cur_flag = %d >>>>>>\r\n", cur_flag);

					if(!cur_flag){
						dbg(2, ">>>>>> OSD display >>>>>>\r\n");
						tw_menu_draw((struct tw_menu*)&page_img_osd2_0_menu);
						osd2_clear_menu(0x1bc5,50,20);
						osd2_enable();
						update_bc(br_val,co_val);
						cur_flag++;
						
					}
					else
					{
						switch(cur_flag)
						{
							case 1:/*hightlight brightness*/
							quit_flag = 0;
							br_flag = 1;
							tw_clear_icons((struct tw_icons*)&page_img_rom_font_131);
							page_img_rom_font_131.x = 15;
							page_img_rom_font_131.y = 7;
							tw_icons_draw((struct tw_icons*)&page_img_rom_font_131);/**at brightness*/
							cur_flag++;
							break;
							case 2:/*highlight contrast*/
							br_flag = 0;
							co_flag = 1;
							tw_clear_icons((struct tw_icons*)&page_img_rom_font_131);
							page_img_rom_font_131.x = 15;
							page_img_rom_font_131.y = 9;
							tw_icons_draw((struct tw_icons*)&page_img_rom_font_131);
							update_bc(br_val,co_val);
							cur_flag++;
							break;
							case 3:/*highlight exit*/
							co_flag = 0;
							tw_clear_icons((struct tw_icons*)&page_img_rom_font_131);
							page_img_rom_font_131.x = 15;
							page_img_rom_font_131.y = 11;
							tw_icons_draw((struct tw_icons*)&page_img_rom_font_131);
							update_bc(br_val,co_val);
							cur_flag = 1;
							quit_flag = 1;
							break;
							default:
							break;
						}
					}
		
					break;
				case 4:/*reserved*/

				//bb_nor_write(20, 30);
				break;	
				case 6:/*start format SD card process*/

				//bb_nor_read(&a, &b);
				//dbg(2, "a = %d, b = %d\r\n", a, b);
				//break;
				//rtc_get_time(&tm1);
				//dbg(2, "tm_mday = %d\r\n", tm1.tm_mday);
#if 1
					//rc = chk_sd_status();
					//if(rc == SDC_MOUNTED) {
						//WARN("prepare formating SD card!\n");
						format_sdcared();
						DBG_PRINT("format SD done\r\n");
						cq_write_byte_issue(CQ_P0, 0x91, 0xB8, CQ_TRIGGER_SW);
					//}
#endif
				default:
					break;
			}


#if 0
			if(event->keystroke.ch==5)
			{
				tw_menu_draw((struct tw_menu*)&page_img_osd2_0_menu);
				osd2_clear_menu(0x1bc5,50,20);
				osd2_enable();	

				tw_icons_loc((struct tw_icons*)&page_img_rom_font_11, 17, 7);/*bright*/
				tw_icons_loc((struct tw_icons*)&page_img_rom_font_12, 17, 9);/*contrast*/
				tw_icons_loc((struct tw_icons*)&page_img_rom_font_13, 17, 11);/*exit*/

				osd2_update();

				dbg(2, ">>>>> osd2_update done\n\r");
			}
			else
			{
				osd2_disable();
			}
#endif
		break;

		case TW_EVENT_TYPE_QUIT_SYSTEM:
			dbg(2, ">>>>> QUIT Menu Page\n\r");

			/* todo: Write Your Code Here */

		break;

		default:	/* put to nodify if no process this event */
			return TW_RETURN_NO_PROCESS;


	}
	return TW_RETURN_NONE;
}
