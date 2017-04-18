/**
 *  @file   main.c
 *  @brief
 *  $Id: main.c,v 1.2 2016/07/27 09:08:47 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/17  Ken 	New file.
 *
 */

#include <config.h>
#include <debug.h>
#include <io.h>
#include "mconfig.h"
#include "ls.h"
#include "display.h"
#include "tw_widget_sys.h"
#include "rc.h"
#include <sys.h>

#include "./drivers/spirw/spirw.h"
#include "./drivers/spiosd/spiosd.h"
#include "./drivers/osd2/osd2.h"
#include "./drivers/calibration/calibration.h"
#include "./drivers/sarkey/sarkey.h"
#include "./drivers/i80/i80.h"
#include "./drivers/cq/cq.h"

#include <stdio.h>
#include <string.h>
#include <mipsregs.h>
#include <serial.h>
#include <codec.h>
#include <event.h>
#include <cache.h>
#include <debug.h>
#include <ctype.h>
#include <cvbs.h>
#include <codec_eng/decpic.h>
#include <codec_eng/encpic.h>
#include <codec_eng/jreg.h>
#include <mconfig.h>
#include <ls.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <heap.h>

#include "irq.h"
#include "interrupt.h"

#include <serial.h>
#define Bluetooth
extern void data_buf(char *c);
extern int buf_switch(void);
static int Switch=0 ;
unsigned char BT_offset_length =0;

void  init_BT(void)
{
		afc_serial_init_bt(921600);
		afc_uart_irq_open_bt(RCV_THRD_HALF);

}

char *dbuf=0;

static int snap_dec_cmd_callback (void **arg)
{
    int ch = 0;
    int cmd = 0;
	 	int rc=0;
  	int offset_length=0;
 	 	char offset=0;
  	unsigned long length=0;	
   	
		ch = readb(0xb0403000);
        	switch ((ch)) {
        		 case 1:
        		 		writeb(0x00,0xb0403000);									 						 		 	  	  
				 					 Switch=buf_switch();			 	 
					 					Switch+=32*1024;		 					
		 			 					Switch%=64*1024;
																	 						
								for(rc=0;rc<70;rc++){					
							 	 		if(dbuf[rc+Switch]==0xaa){
							 	 				length=dbuf[rc+Switch+1];						 	 		
							 	 					length<<=8;
							 		      length|= dbuf[rc+Switch+3];						 		      
							 		      	length<<=8;
							 		      length|= dbuf[rc+Switch+5];		
							 		      offset_length=rc;				    		   			
														writeb(length>>8,0xb0403030);
														writeb(length&0xff,0xb0403040);	
												}								 		 						 		      					 		   											
							 		 }	
					 		 		    				 			  					 		   			 		      						 	 																				
									writeb((Switch+offset_length-BT_offset_length)>>8,0xb0403010);
									writeb((Switch+offset_length-BT_offset_length)&0xff,0xb0403020);	
								  	cmd = CMD_STEP;	
                break;
            	case 'T':
                /* next */
                printf ("snap_status: CMD_NEXT\n");
                cmd = CMD_NEXT;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
            	case 'P':
                /* next */
                printf ("snap_status: CMD_PREVIOUS\n");
                cmd = CMD_PREVIOUS;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
							case 'E':
                /* error */
                printf ("snap_status: CMD_ERROR\n");
                cmd = CMD_STOP_ERR;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
                
        	}
    return cmd;
}
static void disp_osd_init (void)
{
	/* enable NOR flash of Quad mode for SPIOSD */
	enable_quad_io();
	spiosd_init(0x116, 0x0A, 0x96, 0x0D);	// initialize and setting SPIOSD position of offset

	/* initialize for OSD of resource */
	resource_init();

	/* initialize for OSD2 */
	osd2_init();
}
int __mips32__ __attribute__((section(".inflate")))
 main_drive(void *sys_ctx)
{
 	int rc=0;

  char offset=0;
  unsigned long length=0;	
  
	struct dpic_opt_t opt;
	char msgbuf[ERR_MSG_LEN];
	dbuf = heap_alloc(64*1024);
	
	display_init();
	display_bklight_on();
	display_set_play_full(VIDEO_STD_NTSC);	
	watchdog_disable();
	init_BT();	
	data_buf(dbuf);				
	memset(&opt, 0, sizeof(struct dpic_opt_t));
#if 1
		writeb(0x6F,0xb0403400);
		writeb(0xFF,0xb0403430);
		writeb(0xFF,0xb0403500);
#endif

		while(1){		
				if(readb(0xb0403000)==0x01){				
					 writeb(0x00,0xb0403000);			
				 						 		 	  	  
				 	 Switch=buf_switch();			 	 
					 Switch+=32*1024;		 					
		 			 Switch%=64*1024;
  																	 						
						for(rc=0;rc<70;rc++){					
							 	 		if(dbuf[rc+Switch]==0xaa){
							 	 				length=dbuf[rc+Switch+1];						 	 		
							 	 					length<<=8;
							 		      length|= dbuf[rc+Switch+3];						 		      
							 		      	length<<=8;
							 		      length|= dbuf[rc+Switch+5];
//							 		        printf("3->%d,\n", length);
							 		      	opt.data_length=length;
							 		      	offset=rc;
							 		      	BT_offset_length=rc;											 		 						 		      					 		   											
							 		    }					 			  					 		   			 		      						 	 														
							}											 			
//						if((date_length-offset)==(length+6)){	
							  writel(readl(I80_R00) | 0x01, I80_R00);//i80_on																																	 	  						 	  																																												 												
								rc = decpic2_from_ram_video(dbuf+Switch+(offset+6), &opt, snap_dec_cmd_callback);																			
										if (rc == CMD_STOP_ERR) {
									    printf("encpic return error command %d.\n", rc);
									    rc = get_err(msgbuf);
											 if (rc) {											 								 	
											    printf("get error %d, message : %s\n", rc, msgbuf);
											 }   
										}
//							}		 										  		
					}	
					  		
				if(readb(0xb0403000)==0x08)
					  		goto EXIT; 
	 	}
EXIT: 
	heap_release(dbuf);	
	return 0;		
}


