/*
 *  @file	cvbs.c
 *  @brief	Cheetah CVBS driver
 *	$Id: cvbs.c,v 1.1.1.1 2013/12/18 03:43:42 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *	Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *			2011/05/07    New file
 *
 */
 
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <debug.h>
#include <io.h>
#include <sys/include/cvbs.h>
#include "./drivers/cq/cq.h"

#define HS_REG_MSB 0xB0400590
#define VS_REG_LSB 0xB04005A0
#define VS_REG_MSB 0xB04005B0

void cvbs_bluescreen_on(unsigned char on)
{
//    if(readl(0xb04000e0)&0x20)
//        return;
//    
    if(on){
       // printf("bluescreen on\n");
       /* set bluescreen color */
//        writel(0x1D, 0xb04009d0);
//        writel(0xF0, 0xb04009e0);
//        writel(0x6C, 0xb04009f0);
//        writel(0x87, 0xb0400910);
//        writel(0x12, 0xb0400C20);
//        writel(0x00, 0xb0400500);
//        writel(0x00, 0xB0400e20);
//        writel(0x10, 0xB0400e20);
        cq_write_byte(CQ_P0, 0x9d, 0x1D);
        cq_write_byte(CQ_P0, 0x9E, 0xF0);
        cq_write_byte(CQ_P0, 0x9F, 0x6C);
        cq_write_byte(CQ_P0, 0x91, 0x87);
        cq_write_byte(CQ_P0, 0xC2, 0x12);
		cq_write_byte_issue(CQ_P0, 0x50, 0x00, CQ_TRIGGER_SW);
    }
    else{
      //  printf("bluescreen off\n");
        //writel(0x07, 0xb0400910);
        //writel(0x00, 0xb0400C20);
        //writel(0x11, 0xB0400e20);
        cq_write_byte(CQ_P0, 0x91, 0x07);
		cq_write_byte_issue(CQ_P0, 0xC2, 0x00, CQ_TRIGGER_VSYNC);
    }
    
}

inline void cvbs_wait_sig_stable()
{
    unsigned short wTemp=100;
    while(wTemp--)
	{
		if((readl(0xb04025A0)&0x06)==0x06)
			break;
		usleep(10000);
	}
}
void cvbs_detect_color(void){
    if((readl(0xB04025A0)&0x08)==0x00)
        //writel(readl(0xB0402200)|0x01,0xB0402200);
    	cq_write_byte_issue(CQ_P2, 0x20, (readb(0xB0402200) | 0x01), CQ_TRIGGER_SW);
    else
        //writel(readl(0xB0402200)&0xFE,0xB0402200);
    	cq_write_byte_issue(CQ_P2, 0x20, (readb(0xB0402200) & 0xFE), CQ_TRIGGER_SW);
}
inline unsigned short cvbs_get_vs_period(void)
{
    unsigned short vs_period,temp;
    unsigned char i;
    unsigned char count;
    cvbs_bluescreen_on(1);
    //writel(readl(0xB0402600) | 0x03, 0xB0402600);
    cq_write_byte_issue(CQ_P2, 0x60, (readb(0xB0402600) | 0x03), CQ_TRIGGER_SW);
   // cvbs_wait_sig_stable();
    temp=0;
    count = 0;
    for(i=0;i<30;i++){
        vs_period = readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);
        if((temp==(vs_period+1))||(temp==(vs_period-1))||(temp==(vs_period))){
            count++;
            if(count>=5)
                break;
        }
        else{
            count = 0;
            temp = vs_period;
        }
        //printf("i=%d, count= %d, vs_period=%d, paldetec=%x\n",i,count,vs_period,readl(0xB04025C0));
        usleep(100000);    
    }
    printf("i=%d, count= %d, vs_period=%d, paldetec=%x\n",i,count,vs_period,readl(0xB04025C0));
    if((vs_period<315)&&(vs_period>310)){
       
            vs_period = 312;
    }
    else{
        
        if(readl(0xB04025C0)&0x04)
            vs_period = 312;
        else
            vs_period = 262;
        
    }
     
    //writel(readl(0xB0402600) & 0xFC, 0xB0402600);
    cq_write_byte_issue(CQ_P2, 0x60, (readb(0xB0402600) & 0xFC), CQ_TRIGGER_SW);
    return vs_period;
}
int cvbs_detect_reset(struct cvbs_state *cvbs_st)
{   
    cvbs_st->reset_times++;
#if 0
    //printf("cvbs_detect_reset\n");
	writel(0x12, 0xB0400C20); 
	cvbs_wait_sig_stable();
    /*Detect Signal in and reset comb filter*/
    writel(readl(0xB04025F0) | 0x01, 0xB04025F0);
    /*wait about 10ms*/
    usleep(10000);
    writel(readl(0xB04025F0) & 0xFE, 0xB04025F0);  
	/*wait about 10ms*/
    //usleep(10000);
	writel(0x00, 0xB0400C20); 
	writel(0x00, 0xB0400500);
    writel(0x01, 0xB0400500);	
    writel(0x11,0xB0400E20);
#else
	cvbs_wait_sig_stable();
    /*Detect Signal in and reset comb filter*/
	cq_write_byte_issue(CQ_P2, 0x5F, (readb(0xB04025F0) | 0x01), CQ_TRIGGER_SW);
    /*wait about 10ms*/
    usleep(10000);
    cq_write_byte_issue(CQ_P2, 0x5F, (readb(0xB04025F0) & 0xFE), CQ_TRIGGER_SW);
	/*wait about 10ms*/
    //usleep(10000);
    cq_write_byte_issue(CQ_P0, 0x50, 0x00, CQ_TRIGGER_SW);
    cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);

#endif
    cvbs_wait_sig_stable();
	return 0;
}

int cvbs_detect_init(struct cvbs_state *cvbs_st)
{
    //unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
    unsigned short vs_period;
	cvbs_st->chromapll_lockcb_times =0;
	cvbs_st->one_line_loss_times = 0;
	cvbs_st->reset_times = 0;
	cvbs_st->cvd_h_loss_times = 0;
	cvbs_st->cvd_v_loss_times = 0;
	/*add black screen*/
	cq_write_byte_issue(CQ_P2, 0x27, (readb(0xB0402270) | 0x30), CQ_TRIGGER_SW);
	//writel(readl(0xB0402270) | 0x30, 0xB0402270);
	/*add chroma lock determine time  */  
	cq_write_byte_issue(CQ_P2, 0xA3, 0x61, CQ_TRIGGER_SW);
	//writel(0x61, 0xB0402A30);
	
	
	
	/*initial no signal*/ 
    if ( readl(0xB04025A0)& 0x01  )
	{
	    cvbs_st->std_mode =  AVIN_CVBS_CLOSE;
	  	cvbs_st->auto_freerun_mode = 0;

		/*record initial data if no signal*/
		cvbs_st->chromapll_lockcb = 3;
		
		//cvbs_bluescreen_on(1);
	}
	else
	{	
	    #if 0       
        /*CVD auto mode detection open*/
        writel(readl(0xB0402600) | 0x01, 0xB0402600);  
	    #endif
	    //printf("cvbs_detect_init\n");
	    cvbs_detect_reset(cvbs_st);  
	    /*initial set  NTSC/PAL*/  
	    cvbs_st->std_mode =  AVIN_CVBS_PAL;
	    cvbs_st->auto_freerun_mode = 1;
		/*detect ChromaPLL_lLock*/
		cvbs_st->chromapll_lockcb = (readl(0xB04025A0)&0x08)>>3;	
       // printf("readl(0xB0402610) = %x\n",readl(0xB0402610));
        /*check avin type NTSC/PAL*/
        vs_period = cvbs_get_vs_period();//readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);	
       //  printf("vs_period= %x\n",vs_period);
		//if( (readl(0xB0402610)&0x10) && readl(HS_REG_MSB)<=HS_REG_MSB_THRESHOLD) 
		//if( (readl(0xB0402610)&0x20) && (vs_period<0x0110)) 
#if 0
		if( (vs_period<0x0110)) 
        {	 
            /*Initial Auto Free Run*/	
            writel(0x02, 0xB0400500);
		    /*for NTSC*/
		    writel(0x05, 0xB04005A0);
            writel(0x01, 0xB04005B0);   
            /*Enable Auto Free Run*/ 
            writel(0x01, 0xB0400500);	
            writel(0x11, 0xB0400E20);
			
            cvbs_st->std_mode =  AVIN_CVBS_NTSC;
        }     
        //else if( (readl(0xB0402610)&0x20)  && readl(HS_REG_MSB)>HS_REG_MSB_THRESHOLD) 
        else if( (vs_period>0x0110)) 	
        {	 
        	/*Initial Auto Free Run*/	
            writel(0x02, 0xB0400500);
		    /*for PAL*/
		    writel(0x37, 0xB04005A0);
            writel(0x01, 0xB04005B0);   
            /*Enable Auto Free Run*/ 
            writel(0x01, 0xB0400500);	  
            writel(0x11, 0xB0400E20);
			
    	    cvbs_st->std_mode =  AVIN_CVBS_PAL;           
        }	 		
#else
		if( (vs_period<0x0110))
        {
            /*Initial Auto Free Run*/
			cq_write_byte(CQ_P0, 0x50, 0x02);
		    /*for NTSC*/
			cq_write_byte(CQ_P0, 0x5A, 0x05);
			cq_write_byte(CQ_P0, 0x5B, 0x01);
            /*Enable Auto Free Run*/
			cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);

            cvbs_st->std_mode =  AVIN_CVBS_NTSC;
        }
        //else if( (readl(0xB0402610)&0x20)  && readl(HS_REG_MSB)>HS_REG_MSB_THRESHOLD)
        else if( (vs_period>0x0110))
        {
        	/*Initial Auto Free Run*/
        	cq_write_byte(CQ_P0, 0x50, 0x02);
		    /*for PAL*/
        	cq_write_byte(CQ_P0, 0x5A, 0x37);
			cq_write_byte(CQ_P0, 0x5B, 0x01);
            /*Enable Auto Free Run*/
			cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);

    	    cvbs_st->std_mode =  AVIN_CVBS_PAL;
        }
#endif
        //cvbs_bluescreen_on(0);
	 }
	
	return 0;
}

int cvbs_detect_close(struct cvbs_state *cvbs_st)
{   
    /*close auto free run*/  
	cq_write_byte_issue(CQ_P0, 0x50, (readb(0xB0400500) & 0x7E), CQ_TRIGGER_SW);
	//writel(readl(0xB0400500) & 0x7E, 0xB0400500);
    cvbs_st->auto_freerun_mode = 0;
    cvbs_st->std_mode = AVIN_CVBS_CLOSE;
	cvbs_st->chromapll_lockcb = 3;
	cvbs_st->chromapll_lockcb_times =0;
	//cvbs_bluescreen_on(1);
    #if 0
    /*CVD auto mode detection close*/
    writel(readl(0xB0402600) & 0xFE, 0xB0402600);
    #endif

	return 0;
}
	


inline int cvbs_detect_freerun_open(struct cvbs_state *cvbs_st)
{   
    unsigned short pic_view;
    pic_view = readl(0xb04000e0)&0x20;
//    printf("cvbs_detect_freerun_open ->pic_view=%d\n",pic_view);
    if(pic_view){
        return 0;
    }
	/*have signal*/ 
#if 0
    if ( !(readl(0xB04025A0)& 0x01))
    {
       
		if(cvbs_st->std_mode == AVIN_CVBS_NTSC) 
        {	 
            /*Initial Auto Free Run*/	
            writel(0x02, 0xB0400500);
		    /*for NTSC*/
		    writel(0x05, 0xB04005A0);
            writel(0x01, 0xB04005B0);   
            /*Enable Auto Free Run*/ 
            writel(0x01, 0xB0400500);	
            writel(0x11, 0xB0400E20);
			
        }     
        else if(cvbs_st->std_mode == AVIN_CVBS_PAL) 	
        {	 
        	/*Initial Auto Free Run*/	
            writel(0x02, 0xB0400500);
		    /*for PAL*/
		    writel(0x37, 0xB04005A0);
            writel(0x01, 0xB04005B0);   
            /*Enable Auto Free Run*/ 
            writel(0x01, 0xB0400500);	  
            writel(0x11, 0xB0400E20);
			        
        }	 		
	    cvbs_st->auto_freerun_mode = 1;
    }
#endif
    if ( !(readl(0xB04025A0)& 0x01))
    {
    	if(cvbs_st->std_mode == AVIN_CVBS_NTSC)
		{
			/*Initial Auto Free Run*/
			cq_write_byte(CQ_P0, 0x50, 0x02);
			/*for NTSC*/
			cq_write_byte(CQ_P0, 0x5A, 0x05);
			cq_write_byte(CQ_P0, 0x5B, 0x01);
			/*Enable Auto Free Run*/
			cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);
		}
        else if(cvbs_st->std_mode == AVIN_CVBS_PAL)
        {
			/*Initial Auto Free Run*/
			cq_write_byte(CQ_P0, 0x50, 0x02);
			/*for PAL*/
			cq_write_byte(CQ_P0, 0x5A, 0x37);
			cq_write_byte(CQ_P0, 0x5B, 0x01);
			/*Enable Auto Free Run*/
			cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);

			cvbs_st->std_mode =  AVIN_CVBS_PAL;
        }
    	cvbs_st->auto_freerun_mode = 1;
    }
    return 0;
}
	
int cvbs_detect_freerun_close(struct cvbs_state *cvbs_st)
{   
//	printf("cvbs_detect_freerun_close----------------\n");
    /*close auto free run*/  
    //writel(readl(0xB0400500) & 0x7E, 0xB0400500);  
	cq_write_byte_issue(CQ_P0, 0x50, 0x00, CQ_TRIGGER_SW);
	//writel(0x00, 0xB0400500);
    //writel(0x11, 0xB0400E20);
    cvbs_st->auto_freerun_mode = 0;
    cvbs_st->chromapll_lockcb = 3;
	
	return 0;
}

int cvbs_detect_return_chromalock_times(struct cvbs_state *cvbs_st)
{
    return cvbs_st->chromapll_lockcb_times;
}

int cvbs_detect_without_init(struct cvbs_state *cvbs_st)
{ 
    unsigned char org_std ;
	unsigned char org_chromapll_lockcb;
   // unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
	unsigned short vs_period;
    org_std = cvbs_st->std_mode;      
    org_chromapll_lockcb = 	cvbs_st->chromapll_lockcb;	
     
    /*now no signal & otg_std no signal*/
    if ( (readl(0xB04025A0)& 0x01 ) && (org_std == AVIN_CVBS_CLOSE) )
    {        
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
       
        return AVIN_CVBS_CLOSE;
    }
    /*now no signal & otg_std have signal*/
    else if ( (readl(0xB04025A0)& 0x01)  && (org_std != AVIN_CVBS_CLOSE) )
    {
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        //cvbs_bluescreen_on(1);
        return AVIN_CVBS_CLOSE;
    }
    /*now have signal & otg_std no signal*/
    //else if ( (!(readl(0xB04025A0)& 0x01) ) && (org_std == AVIN_CVBS_CLOSE) )
    else if ( ((readl(0xB04025A0)& 0x06) ) && (org_std == AVIN_CVBS_CLOSE) )
    {		
	    /*record ChromaPLL_lLock*/
		cvbs_st->chromapll_lockcb = (readl(0xB04025A0)&0x08)>>3;		
        	
        /*detect NTSC or PAL*/
        vs_period = cvbs_get_vs_period();//readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);
        //if( (readl(0xB0402610)&0x10) && readl(HS_REG_MSB)<=HS_REG_MSB_THRESHOLD) 
        if( (readl(0xB0402610)&0x20) && (vs_period<0x0110)) 
        {	 
            cvbs_st->std_mode =  AVIN_CVBS_NTSC;
            cvbs_detect_reset(cvbs_st);    
            //cvbs_bluescreen_on(0);            	    
            return AVIN_CVBS_NTSC;
        }     
		//else if( (readl(0xB0402610)&0x20)  && readl(HS_REG_MSB)>HS_REG_MSB_THRESHOLD) 	
		else if( (readl(0xB0402610)&0x10)  && (vs_period>0x0110)) 	
        {	 
    	    cvbs_st->std_mode =  AVIN_CVBS_PAL;           
            cvbs_detect_reset(cvbs_st);	
            //cvbs_bluescreen_on(0);    
            return AVIN_CVBS_PAL; 	
        }	
		else
			return cvbs_st->std_mode;
    }	
    /*now have signal & otg_std have signal*/
    else if ( (!(readl(0xB04025A0)& 0x01) )&&  (org_std != AVIN_CVBS_CLOSE))
    {		
		/*record ChromaPLL_lLock*/
		cvbs_st->chromapll_lockcb = (readl(0xB04025A0)&0x08)>>3;		
		
		if( !org_chromapll_lockcb  &&  cvbs_st->chromapll_lockcb)
		{
			cvbs_detect_reset(cvbs_st);
			//cvbs_bluescreen_on(0);
			cvbs_st->chromapll_lockcb_times++;
		}
		
    	/*detect NTSC or PAL*/  
    	vs_period = cvbs_get_vs_period();//readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);	 
		//if( (readl(0xB0402610)&0x10) && readl(HS_REG_MSB)<=HS_REG_MSB_THRESHOLD) 
		if( (readl(0xB0402610)&0x20) && (vs_period<0x0110)) 
        { 	 
            cvbs_st->std_mode =  AVIN_CVBS_NTSC;
            if(cvbs_st->std_mode != org_std )
            {
                //cvbs_bluescreen_on(1);
                cvbs_detect_reset(cvbs_st); 
                //cvbs_bluescreen_on(0); 			
            }  	      
            return AVIN_CVBS_NTSC;
        }     
		//else if( (readl(0xB0402610)&0x20)  && readl(HS_REG_MSB)>HS_REG_MSB_THRESHOLD) 
		else if( (readl(0xB0402610)&0x10)  && (vs_period>0x0110)) 
        {	 
    	    cvbs_st->std_mode =  AVIN_CVBS_PAL;
            if(cvbs_st->std_mode != org_std )
            {
                //cvbs_bluescreen_on(1);
                cvbs_detect_reset(cvbs_st);  
                //cvbs_bluescreen_on(0); 				
            }    	  	      
            return AVIN_CVBS_PAL; 	
        }	 
		else
			return cvbs_st->std_mode;
    }	

    return 0;
}

int cvbs_detect(struct cvbs_state *cvbs_st)
{ 
    unsigned char org_std ;
    static unsigned short h_loss_times;
	unsigned char org_chromapll_lockcb;
    //unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
    unsigned short vs_period;
    unsigned char loss_one_line;
	unsigned char video_std;
    org_std = cvbs_st->std_mode;      
    org_chromapll_lockcb = 	cvbs_st->chromapll_lockcb;	
    video_std = readl(0xB04025A0);

    loss_one_line = readl(0xB0400500)& 0x04;
    if(loss_one_line)
        cvbs_st->one_line_loss_times++;
    if((video_std&0x02)==0)
        cvbs_st->cvd_h_loss_times++;
    if((video_std&0x04)==0)
        cvbs_st->cvd_v_loss_times++;
//    printf("readl(0xB04025A0)=%x\n",video_std);
//    printf("readl(0xB0400500)=%x\n",loss_one_line);   
//    printf("cvbs_st->auto_freerun_mode=%x\n",cvbs_st->auto_freerun_mode);
    /*now no signal & otg_std no signal*/
    if ( ((video_std& 0x01)||(loss_one_line)) && (org_std == AVIN_CVBS_CLOSE) )
    {        
        //printf("return AVIN_CVBS_CLOSE1\n");
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        return AVIN_CVBS_CLOSE;
    }
    /*now no signal & otg_std have signal*/
    else if ( ((video_std& 0x01)||(loss_one_line))  && (org_std != AVIN_CVBS_CLOSE) )
    {
        //cvbs_bluescreen_on(1);
       // printf("return AVIN_CVBS_CLOSE2 (readl(0xB0400500) = %d\n",readl(0xB0400500));
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        return AVIN_CVBS_CLOSE;
    }
    /*now have signal & otg_std no signal*/
    //else if ( (!(readl(0xB04025A0)& 0x01) ) && (org_std == AVIN_CVBS_CLOSE) )
    else if ( ((video_std& 0x06)==0x06 ) && (org_std == AVIN_CVBS_CLOSE) )
    {   
        h_loss_times = 0;
	    /*record ChromaPLL_lLock*/
		cvbs_st->chromapll_lockcb = (readl(0xB04025A0)&0x08)>>3;
        vs_period = cvbs_get_vs_period();//readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);	
        /*detect NTSC or PAL*/
        //if( (readl(0xB0402610)&0x10) && readl(HS_REG_MSB)<=VS_PERIOD) 
        if( (vs_period<0x0110)) 
        {
            printf("N sig in\n");
            cvbs_st->std_mode =  AVIN_CVBS_NTSC;
            cvbs_detect_reset(cvbs_st);
            
            if (cvbs_st->auto_freerun_mode == 0)
        	{	
                cvbs_detect_freerun_open(cvbs_st);
            }      
            //cvbs_bluescreen_on(0);       	    
            return AVIN_CVBS_NTSC;
        }     
		//else if( (readl(0xB0402610)&0x20)  && readl(HS_REG_MSB)>HS_REG_MSB_THRESHOLD) 	
		else if( (vs_period>0x0110)) 	
        {	 
            printf("P sig in\n");
    	    cvbs_st->std_mode =  AVIN_CVBS_PAL;           
            cvbs_detect_reset(cvbs_st);  
            if (cvbs_st->auto_freerun_mode == 0)
        	{	
                cvbs_detect_freerun_open(cvbs_st);
            } 
            //cvbs_bluescreen_on(0);       
            return AVIN_CVBS_PAL; 	
        }	
		else{
			cvbs_st->std_mode = AVIN_CVBS_CLOSE;
            return AVIN_CVBS_CLOSE;
		}
       
    }	
    /*now have signal & otg_std have signal*/
    else if ( ((video_std& 0x06)==0x06 )&&  (org_std != AVIN_CVBS_CLOSE))
    {
       // printf("now have signal & otg_std have signal\n");
       	//if ((cvbs_st->auto_freerun_mode == 0)&&(readl(0xB04000e0&0x20)==0))
    	if ((cvbs_st->auto_freerun_mode == 0))
    	{	
            cvbs_detect_freerun_open(cvbs_st);
        }
		
		/*record ChromaPLL_Lock*/
		cvbs_st->chromapll_lockcb = (readl(0xB04025A0)&0x08)>>3;		
		
		if( !org_chromapll_lockcb &&  cvbs_st->chromapll_lockcb )
		{
		    //printf("org_chromapll_lockcb= %d\n",org_chromapll_lockcb);
		    //printf("cvbs_st->chromapll_lockcb= %d\n",cvbs_st->chromapll_lockcb);
//			cvbs_detect_reset(cvbs_st);
			cvbs_st->chromapll_lockcb_times++;		
		}
		//cvbs_detect_color();
#if 0		
    	/*detect NTSC or PAL*/   
    	vs_period = cvbs_get_vs_period();//readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);
    	video_std = readl(0xB0402610);
		//if( (readl(0xB0402610)&0x10) && readl(HS_REG_MSB)<=HS_REG_MSB_THRESHOLD) 
		//if( (video_std&0x20) && (vs_period<0x0110)) 
		if( (vs_period<0x0110)) 
        { 	 
            cvbs_st->std_mode =  AVIN_CVBS_NTSC;
            if(cvbs_st->std_mode != org_std )
            {
                //printf("1.cvbs_st->std_mode = %d\n",cvbs_st->std_mode);
                //cvbs_bluescreen_on(1);
                cvbs_detect_reset(cvbs_st);  
                //cvbs_bluescreen_on(0);				
            }    	
            return AVIN_CVBS_NTSC;
        }     
		//else if( (readl(0xB0402610)&0x20)  && readl(HS_REG_MSB)>HS_REG_MSB_THRESHOLD) 
		//else if( (video_std&0x10)  && (vs_period>0x0110)) 	
		else if( (vs_period>0x0110)) 	
        {	 
    	    cvbs_st->std_mode =  AVIN_CVBS_PAL;
            if(cvbs_st->std_mode != org_std )
            {
                //printf("2.cvbs_st->std_mode = %d\n",cvbs_st->std_mode);
                //cvbs_bluescreen_on(1);
                cvbs_detect_reset(cvbs_st);
                //cvbs_bluescreen_on(0);   
            }    	  
            return AVIN_CVBS_PAL; 	
        }	 
		else{
		    //printf("video_std=%x\n",video_std);
		    //printf("vs_period = %d\n",vs_period);
            return cvbs_st->std_mode;
		}
#endif		
		return cvbs_st->std_mode;
    }	
//    else if(((video_std&0x02)==0)&&  (org_std != AVIN_CVBS_CLOSE)){

    	//cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        //loss_one_line = readl(0xB0400500)& 0x04;
//        h_loss_times++;
//        if(h_loss_times>1600){
//            h_loss_times = 0;
//            cvbs_detect_reset(cvbs_st);
//        }
//        return cvbs_st->std_mode;
   	
//    }
    else{
        return cvbs_st->std_mode;
    }
} 
