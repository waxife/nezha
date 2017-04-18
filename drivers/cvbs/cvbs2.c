/*
 *  @file	cvbs.c
 *  @brief	Cheetah CVBS driver
 *	$Id: cvbs2.c,v 1.21 2015/02/04 08:30:03 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.21 $
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
#include <io.h>
#include <sys/include/cvbs.h>
#include "./drivers/cq/cq.h"

#define DBG_LEVEL   0
#define DBG_HELPER
#include <debug.h>

static unsigned short vs_period=0;
static unsigned char get_count=0;

#define HS_REG_MSB 0xB0400590
#define VS_REG_LSB 0xB04005A0
#define VS_REG_MSB 0xB04005B0
//Blue : Y=0xF0, U=0x1D, V=0x6C
//Black: Y=0x00, U=0x80, V=0x80
//
#if 0
void cvbs2_bluescreen_on(unsigned char on, unsigned char y, unsigned char u, unsigned char v)
{
    int wi = 60;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
    if(on){
        dbg(2,"bluescreen on\n");
       /* set bluescreen color */
        
        cq_write_byte(CQ_P0, 0x9d, y);
        cq_write_byte(CQ_P0, 0x9E, u);
        cq_write_byte(CQ_P0, 0x9F, v);        
        cq_write_byte(CQ_P0, 0x91, 0x87);
        if (cq_flush_now () < 0)
    		ERROR ("cq_flush_now ()\n");
    }
    else{
        //printf("bluescreen off\n");
        if((readl(0xb0400910)&0x80)==0x00){
            ERROR("Open panel error. Bluescreen is off!!!\n");
            if(((readb(0xb04000e0)&0x20)==0)&&((readb(0xb04013A0)&0x04)==0x04)){
                writeb(0x87, 0xB0400910);
                writeb(0x00, 0xB0400E20);
            }
            else
                return;
        }
        cvbs2_detect_reset();
        /*Enable Free run to overcome the flicker when fpll enable.*/ 
        cq_write_byte(CQ_P0, 0xC2, 0x12);
        cq_write_byte(CQ_P1, 0x3A, 0x00);
        if (cq_flush_vde () < 0)
    		ERROR ("cq_flush_now ()\n");
    	/*Disable free run and let fpll to generate output vsync.*/
    	cq_write_byte(CQ_P0, 0xC2, 0x00);
    	if (cq_flush_vde () < 0)
    		ERROR ("cq_flush_now ()\n");
        cq_write_byte(CQ_P1, 0x3A, 0xE0);
        if (cq_flush_vde () < 0)
    		ERROR ("cq_flush_now ()\n");
    	usleep(200000);	/* Wait FPLL stable. Fpll can't auto black screen at first time.*/
    	
    	cq_write_byte(CQ_P0, 0x91, 0x07);
        if (cq_flush_vde () < 0)
    		ERROR ("cq_flush_now ()\n");
    }
    
}
#else
void cvbs2_bluescreen_on(unsigned char on, unsigned char y, unsigned char u, unsigned char v)
{
    int wi = 60;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
        
    if(on){
        dbg(2,"bluescreen on\n");
       /* set bluescreen color */
        
            writeb(y, 0xB04009d0);
            writeb(u, 0xB04009e0);
            writeb(v, 0xB04009f0);
            writeb(0x87, 0xB0400910);
            writeb(0x00, 0xB0400E20);
    }
    else{
        //printf("bluescreen off\n");
        if((readl(0xb0400910)&0x80)==0x00){
            ERROR("Open panel error. Bluescreen is off!!!\n");
            if(((readb(0xb04000e0)&0x20)==0)&&((readb(0xb04013A0)&0x04)==0x04)){
                writeb(0x87, 0xB0400910);
                writeb(0x00, 0xB0400E20);
            }
            else
                return;
        }
        cvbs2_detect_reset();
        /*Enable Free run to overcome the flicker when fpll enable.*/ 
       
         writeb(0x12, 0xB0400C20);
         writeb(0x00, 0xB04013A0);
         usleep(10000);
    	/*Disable free run and let fpll to generate output vsync.*/

        writeb(0x00, 0xB0400C20);
        writeb(0xE0, 0xB04013A0);
    	usleep(300000);	/* Wait FPLL stable. Fpll can't auto black screen at first time.*/
    	
    	//cq_write_byte(CQ_P0, 0x91, 0x07);
        //if (cq_flush_vde () < 0)
    	//	ERROR ("cq_flush_now ()\n");
        writeb(0x07, 0xB0400910);
        writeb(0x00, 0xB0400E20);
    }
    
}
#endif
void cvbs2_resync_fpll(void)
{   

    //printf("bluescreen off\n");
    if((readl(0xb0400910)&0x80)==0x00){
        ERROR("Open panel error. Bluescreen is off!!!\n");
        if(((readb(0xb04000e0)&0x20)==0)&&((readb(0xb04013A0)&0x04)==0x04)){
            writeb(0x87, 0xB0400910);
            writeb(0x00, 0xB0400E20);
        }
        else
            return;
    }
    cvbs2_detect_reset();
    /*Enable Free run to overcome the flicker when fpll enable.*/ 
    cq_write_byte(CQ_P0, 0xC2, 0x12);
    cq_write_byte(CQ_P1, 0x3A, 0x00);
    if (cq_flush_vde () < 0)
		ERROR ("cq_flush_now ()\n");
	/*Disable free run and let fpll to generate output vsync.*/
	cq_write_byte(CQ_P0, 0xC2, 0x00);
	if (cq_flush_vde () < 0)
		ERROR ("cq_flush_now ()\n");
    cq_write_byte(CQ_P1, 0x3A, 0xE0);
    if (cq_flush_vde () < 0)
		ERROR ("cq_flush_now ()\n");
	usleep(200000);	/* Wait FPLL stable. Fpll can't auto black screen at first time.*/
    	
    	    
}

/**
 * @func       cvbs2_fpll_init
 * @brief      init fpll
 * @param     
 * @return   
 */
void cvbs2_fpll_init(void)
{
	writel(0x12, 0xB0400C20);
	writel(0x33, 0xB04013B0);
	writel(0xFF, 0xB04013E0);
	writel(0x00, 0xB04013F0);
	writel(0xe0, 0xB04013A0);
}

inline void cvbs2_wait_sig_stable()
{
    unsigned short wTemp=30;
    while(wTemp--)
	{
		if((readl(0xb04023A0)&0x06)==0x06)
			break;
		usleep(10000);
	}
	//printf("wTemp = %d\n",wTemp);
}

inline unsigned short cvbs2_get_vs_period_wait(void)
{
    unsigned short vs_period,temp;
    unsigned char i;
    unsigned char count;
    temp=0;
    count = 0;
    for(i=0;i<20;i++){
        vs_period = readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);
        if((temp==(vs_period+1))||(temp==(vs_period-1))||(temp==(vs_period))){
            count++;
            if(count>=5)
                break;
        }
        else{
            count = 0;
            temp = vs_period;
            if(readl(0xB0400500)&0x04)
                break;
        }
        //printf("i=%d, count= %d, vs_period=%d, paldetec=%x\n",i,count,vs_period,readl(0xB04025C0));
        usleep(30000);    
    }
//    printf("i=%d, count= %d, vs_period=%d, paldetec=%x\n",i,count,vs_period,readl(0xB04025C0));

    return vs_period;
}

inline unsigned char cvbs2_get_vs_period(unsigned short *vtotal)
{
    
    unsigned short temp;
    static unsigned int justosd_end_time = 0;
    
    
    if(get_count==0){
        justosd_end_time = read_c0_count32() + millisec(30);
        vs_period = readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);
        get_count++;
        return 0;
    }
    else{
        if(is_time_out(justosd_end_time)){
            temp = readl(VS_REG_LSB)|(readl(VS_REG_MSB)<<8);
            if(temp>500)
                temp>>=1;
            //printf("temp = %d, get_count = %d\n",temp, get_count);
            if((temp==(vs_period+1))||(temp==(vs_period-1))||(temp==(vs_period))){
                get_count++;
                if(get_count>=5){
                    get_count = 0;
                    *vtotal = vs_period;
                    return 1;
                }
            }
            else{
                get_count = 0;
                vs_period=temp;
            }
        }
        else{
            return 0;
        }
    }
    return 0;
}


inline unsigned int cvbs2_get_video_type(unsigned char std)
{
    unsigned char color_cubcarrire;
    unsigned short vtotal;
	unsigned char rc;
    color_cubcarrire = readl(0xB0402410)&0x30;
    rc = cvbs2_get_vs_period(&vtotal);
#if 0   
    if((vtotal>=261)&&(vtotal<=264)&&(color_cubcarrire==0x10)){
       // printf("Detect NTSC\n");
        return AVIN_CVBS_NTSC;
    }
    else if((vtotal>=261)&&(vtotal<=264)&&(color_cubcarrire==0x20)){
       // printf("Detect NTSC443\n");
        return AVIN_CVBS_NTSC443;
    }
    else if((vtotal>=311)&&(vtotal<=314)&&(color_cubcarrire==0x20)){
       // printf("Detect PAL\n");
        return AVIN_CVBS_PAL;
    }
    else if((vtotal>=311)&&(vtotal<=314)&&(color_cubcarrire==0x10)){
       // printf("Detect PALCN\n");
        return AVIN_CVBS_PALCN;
    }
    else{
       // printf("vtotal=%d, color_cubcarrire=%x\n",vtotal,color_cubcarrire);
        return AVIN_CVBS_UNKNOW;
    }
#endif
    if (rc == 1)
	{
    	if((vtotal>=259)&&(vtotal<=264)){
    	    //printf("Detect NTSC\n");
    	    return AVIN_CVBS_NTSC;
    	}

    	else if((vtotal>=310)&&(vtotal<=314)){
    	    //printf("Detect PAL\n");
    	    return AVIN_CVBS_PAL;
    	}
    	if((vtotal>=520)&&(vtotal<=528)){
    	    //printf("Detect NTSC\n");
    	    return AVIN_CVBS_NTSC;
    	}
    	else if((vtotal>=620)&&(vtotal<=628)){
    	    //printf("Detect PAL\n");
    	    return AVIN_CVBS_PAL;
    	}
    	else{
    	    //printf("vtotal=%d, color_cubcarrire=%x\n",vtotal,color_cubcarrire);
    	    return AVIN_CVBS_UNKNOW;
    	}
	}
	else
		return std;
    
}

inline int cvbs2_get_stable_video_type(void)
{
    unsigned char color_cubcarrire;
    unsigned short vtotal;
    color_cubcarrire = readl(0xB0402410)&0x30;
    vtotal = cvbs2_get_vs_period_wait();
 
	if((vtotal>=259)&&(vtotal<=264)){
	    //printf("Detect NTSC\n");
	    return AVIN_CVBS_NTSC;
	}
	else if((vtotal>=310)&&(vtotal<=314)){
	    //printf("Detect PAL\n");
	    return AVIN_CVBS_PAL;
	}
	if((vtotal>=520)&&(vtotal<=528)){
	    //printf("Detect NTSC\n");
	    return AVIN_CVBS_NTSC;
	}
	else if((vtotal>=620)&&(vtotal<=628)){
	    //printf("Detect PAL\n");
	    return AVIN_CVBS_PAL;

	}
	else{
	    //printf("vtotal=%d, color_cubcarrire=%x\n",vtotal,color_cubcarrire);
	    return AVIN_CVBS_UNKNOW;
	}
	
    
}


int cvbs2_detect_reset(void)
{   
#if 1
    //printf("cvbs_detect_reset\n");

	
    /*Detect Signal in and reset comb filter*/
    writel(readl(0xB04023F0) | 0x01, 0xB04023F0);
    /*wait about 10ms*/
    usleep(10000);
    writel(readl(0xB04023F0) & 0xFE, 0xB04023F0);  
    cvbs2_wait_sig_stable();
#else
	cvbs_wait_sig_stable();
    /*Detect Signal in and reset comb filter*/
	cq_write_byte_issue(CQ_P2, 0x3F, (readb(0xB04023F0) | 0x01), CQ_TRIGGER_SW);
    /*wait about 10ms*/
    usleep(10000);
    cq_write_byte_issue(CQ_P2, 0x3F, (readb(0xB04023F0) & 0xFE), CQ_TRIGGER_SW);
	/*wait about 10ms*/
    //usleep(10000);
    cq_write_byte_issue(CQ_P0, 0x50, 0x00, CQ_TRIGGER_SW);
    cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);

#endif
	return 0;
}

int cvbs2_detect_init(struct cvbs_state *cvbs_st)
{
	int i=0;
	cvbs_st->one_line_loss_times = 0;
	cvbs_st->reset_times = 0;
	cvbs_st->cvd_h_loss_times = 0;
	cvbs_st->cvd_v_loss_times = 0;
	/*add black screen*/
	writel(readl(0xB0402070) | 0x30, 0xB0402070);
	/*add chroma lock determine time  */  
    writel(0x61, 0xB0402830);
	/*Init Auto mode detection registers for 33.8688 crystal.*/
	writel(0x83, 0xB04024C0);
	writel(0xE0, 0xB04024D0);
	writel(0xF8, 0xB04024E0);
	writel(0x3E, 0xB04024F0);
	writel(0x6F, 0xB0402500);
	writel(0x13, 0xB0402510);
	writel(0x09, 0xB0402520);
	writel(0xA7, 0xB0402530);
	writel(0x89, 0xB0402540);
	writel(0x93, 0xB0402550);
	writel(0x97, 0xB0402560);
	writel(0xB0, 0xB0402570);
	
	/*Fix CVD UV order mistake!!! Don't Remove!!!*/
	writel(readl(0xB0402400) | 0x10, 0xB0402400);
	/*********************************************/
	
	/*Work around cvbs nor color shift and low color burst no color issue.*/
	writel(0x8F, 0xB04020D0);
	writel(0x33, 0xB04022C0);
    writel(readl(0xB0402830)&0xFE, 0xB0402830);	
	/***********************************************************************/
	
	/*initial no signal*/ 
    if ( readl(0xB04023A0)& 0x01  )
	{
	    cvbs_st->std_mode =  AVIN_CVBS_CLOSE;
	  	cvbs_st->auto_freerun_mode = 0;
	}
	else
	{	
	    
	    cvbs2_detect_reset();  
	    /*initial set  NTSC/PAL*/  
	    while(i<30){
	        cvbs_st->std_mode =  cvbs2_get_video_type(cvbs_st->std_mode);
	        i++;
	        if(cvbs_st->std_mode != AVIN_CVBS_UNKNOW)
	            break;
	        if( readl(0xB04023A0)& 0x01  )
        	{
        	    cvbs_st->std_mode =  AVIN_CVBS_CLOSE;
        	  	cvbs_st->auto_freerun_mode = 0;
        	  	break;
        	}    
	        usleep(50000);
	    }
	    
	 }
	/*Enable Free Run Mode.*/
//	writel(0x12, 0xB0400C20);
//    writel(0x00, 0xB0400e20);
//    writel(0x10, 0xB0400e20);
    cq_write_byte_issue(CQ_P0, 0xC2, 0x12, CQ_TRIGGER_SW);
	return 0;
}

int cvbs2_detect_close(struct cvbs_state *cvbs_st)
{   
    /*close auto free run*/  
	cq_write_byte_issue(CQ_P0, 0x50, (readb(0xB0400500) & 0x7E), CQ_TRIGGER_SW);
    cvbs_st->auto_freerun_mode = 0;
    cvbs_st->std_mode = AVIN_CVBS_CLOSE;
	cvbs_st->chromapll_lockcb_times =0;

	return 0;
}
	


inline void cvbs2_detect_freerun_open(struct cvbs_state *cvbs_st)
{   
    unsigned short pic_view;
    pic_view = readl(0xb04000e0)&0x20;
  //  printf("cvbs_detect_freerun_open ->pic_view=%d\n",pic_view);
   
    if(pic_view){
        return;
    }
	/*have signal*/ 
    if(!(readl(0xB04023A0)&0x01))
    {
        pic_view = cvbs2_get_vs_period_wait();
  //      printf("Get Line =%d----------------------\n",pic_view);
		if((pic_view>=261)&&(pic_view<=264)) 
        {	 
  //          printf("Set Free Run 261=%d\n",pic_view);
#if 0
			/*Initial Auto Free Run*/
            writel(0x02, 0xB0400500);
		    /*for NTSC*/
		    writel(0x05, 0xB04005A0);
            writel(0x01, 0xB04005B0);   
            /*Enable Auto Free Run*/ 
            writel(0x01, 0xB0400500);	
            writel(0x11, 0xB0400E20);
#else
			/*Initial Auto Free Run*/
			cq_write_byte(CQ_P0, 0x50, 0x02);
			/*for NTSC*/
			cq_write_byte(CQ_P0, 0x5A, 0x05);
			cq_write_byte(CQ_P0, 0x5B, 0x01);
			/*Enable Auto Free Run*/
			cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);
#endif
        }     
        else if((pic_view>=311)&&(pic_view<=314)) 	
        {	 
  //          printf("Set Free Run 311=%d\n",pic_view);
        	/*Initial Auto Free Run*/	
#if 0
        	writel(0x02, 0xB0400500);
		    /*for PAL*/
		    writel(0x37, 0xB04005A0);
            writel(0x01, 0xB04005B0);   
            /*Enable Auto Free Run*/ 
            writel(0x01, 0xB0400500);	  
            writel(0x11, 0xB0400E20);
#else
			/*Initial Auto Free Run*/
			cq_write_byte(CQ_P0, 0x50, 0x02);
			/*for PAL*/
			cq_write_byte(CQ_P0, 0x5A, 0x37);
			cq_write_byte(CQ_P0, 0x5B, 0x01);
			/*Enable Auto Free Run*/
			cq_write_byte_issue(CQ_P0, 0x50, 0x01, CQ_TRIGGER_SW);
#endif
        }	 		
	    cvbs_st->auto_freerun_mode = 1;
    }

}
	
int cvbs2_detect_freerun_close(struct cvbs_state *cvbs_st)
{   
	//printf("cvbs_detect_freerun_close----------------\n");
    /*close auto free run*/   
//    writel(0x00, 0xB0400500);
//    writel(0x11, 0xB0400E20);
	cq_write_byte_issue(CQ_P0, 0x50, 0x00, CQ_TRIGGER_SW);
    cvbs_st->auto_freerun_mode = 0;
	
	return 0;
}


void ADC_auto_gain_control (void)
{

    //printf("ADC_auto_gain_control \n");
    //printf("ADC Vmode P0 02 =0x%x\n",readl(0xB0400020));  
    //printf("ADC Gain Readback P0 08 =0x%x\n",readl(0xB0400080));  	

    if((readl(0xB0400090)&0x01)!=0x01)
    {
        writel(readl(0xB0400090)|0x01,0xB0400090);   
		//printf("ADC_auto_gain enable  P0 09 =0x%x\n",readl(0xB0400090));  
    }
	
    /*vmode:1 0.6V ~ 1.3V*/
    if((readl(0xB0400020)&0x08))
    {
        if(readl(0xB0400080)<0x50)
        { 
            writel(readl(0xB0400020)&0xF7,0xB0400020);
			//printf("Vmode 1 change to 0\n");
        }
    }
	/*vmode:0 0.5V ~ 1.0V*/
	else
	{
	    if(readl(0xB0400080)>0xF0)
        {
            writel(readl(0xB0400020)|0x08,0xB0400020);
			//printf("Vmode 0 change to 1\n");
        }
	}
}
#if 0
void cvbs2_fine_tune_dto(void)
{
	unsigned char i;
	unsigned char count = 0;
	unsigned char r_74, r_75;
	for(i=0;i<100;i++)
	{
		r_74 = readl(0xB0402740);//IC_ReadByte(TWIC_P2, 0x74);
		r_75 = readl(0xB0402750);//IC_ReadByte(TWIC_P2, 0x75);
		if((r_74==readl(0xB0402180))&&(r_75==readl(0xB0402190)))
		{ 
			count++;
			//twdDelay(100);	  // Delay  for DTO Stable
            usleep(1000);
		}
		else
		{
			count = 0;
			writel(r_74,0xB0402180);//IC_WritByte(TWIC_P2, 0x18, r_74);
			writel(r_75,0xB0402190);//IC_WritByte(TWIC_P2, 0x19, r_75); 
			writel(0x01,0xB04023F0);//IC_WritByte(TWIC_P2, 0x3F, 0x01);
            usleep(100);
			writel(0x00,0xB04023F0);//IC_WritByte(TWIC_P2, 0x3F, 0x00);
			//twdDelay(500);	 // Delay  for DTO Stable
            usleep(5000);
		}
		if(count > 20){
			count = 0;
			while(!(readl(0xB0402410)&0x08)){
				if(count++>=20)
					break;
				//twdDelay(100);	 // Delay  for CVD Stable
                usleep(1000);
			}
			break;
		}
		
	}
	//twdDelay(3500); // Delay  for DTO Stable
    usleep(35000);
}

#else

void cvbs2_hdto2cdto(unsigned char pal) {
	unsigned char i, j, idx=255, count = 0;
	unsigned long hDTO;
	unsigned short cDTO;
	unsigned short M,N;

	//IC_WritByte(TWIC_P2, 0x3F, 0x01);
    writel(0x01, 0xB04023F0);
	//IC_WritByte(TWIC_P2, 0x3F, 0x00);		
    writel(0x00, 0xB04023F0);
    //usleep(50000);
    
	for (i = 0; i < 10; i++) {
	        
        for(j=0; j<50; j++){
			hDTO = 	readl(0xB0402700)<<8;
            hDTO += readl(0xB0402710);	 
			if(idx == (hDTO - 0x1fa0)) {
				if((count++)>=5)
					break;
			}
			else{
				idx = hDTO - 0x1fa0;
			}
			usleep(20000);
		}
		
		if (idx < 0xc0) {
			if(pal){
				M = 38216; N = 29091;
			
			}
			else{
				M = 315;   N = 297;
			
			}
			cDTO = (hDTO*M)/N;	
            //printf("hDTO: %x, cDTO: %x\n", hDTO, cDTO);
			//IC_WritByte(TWIC_P2, 0x18, cDTO>>8);
            writel(cDTO>>8, 0xB0402180);
			//IC_WritByte(TWIC_P2, 0x19, cDTO&0xff);
            writel(cDTO&0xFF, 0xB0402190);
			writel(0x01, 0xB04023F0);
            //IC_WritByte(TWIC_P2, 0x3F, 0x00);		
            writel(0x00, 0xB04023F0);
			usleep(50000);//twdDelay(500);  // Delay  for CVD Stable  
			break;
		}
	}
}



void cvbs2_fine_tune_dto(void){
	unsigned char count, i=10;		  
    
    if(readl(0xB0402000)&0x02){
        cvbs2_hdto2cdto(1);
    }
    else{
        cvbs2_hdto2cdto(0);
    }
    
    count = readl(0xB0402750);
    while(i--){
		if(count==0xFF)
			count = readl(0xB0402750);
		else
			break;
	}
	//IC_WritByte(TWIC_P2, 0x19, IC_ReadByte(TWIC_P2, 0x75)); 
    writel(count, 0xB0402190);
	//IC_WritByte(TWIC_P2, 0x3F, 0x01);
    writel(0x01, 0xB04023F0);
	//IC_WritByte(TWIC_P2, 0x3F, 0x00);		
    writel(0x00, 0xB04023F0);
	
	count = 0;
	while(!(readl(0xB04023A0)&0x08)){
		if(count++>=10)
			break;	
		usleep(50000);//twdDelay(500);  // Delay  for CVD Stable  
        	
	}				
}
void cvbs2_cdto_check(void){
    unsigned char cDTO, i=10;
 
    cDTO = readl(0xB0402750);
    while(i--){
		if(cDTO==0xFF)
			cDTO = readl(0xB0402750);
		else
			break;
	}
    if(cDTO != readl(0xB0402190))
        cvbs2_fine_tune_dto();
}
void cvbs2_fix_cvd_nocolor(void){
	/*****************************************
		Important Note!  Register Must Setting
		P2_0D[7:6] = 10b
		P2_83[0] = 0b

	******************************************/
	 
	if((readl(0xB04023B0)&(1<<1))||(!(readl(0xB04023C0)&(1<<0))&&(readl(0xB0402000)&0x02)))
	{
		writel(0x00, 0xB04028E0);
		writel(0x00, 0xB04028F0);
	}
	else
	{
        writel(0x64, 0xB04028E0);
		writel(0x80, 0xB04028F0);
	}
	
}
#endif
int cvbs2_detect(struct cvbs_state *cvbs_st)
{ 
    unsigned char org_std ;
    static unsigned short sig_detc_times;
    //unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
   
	unsigned char video_std;	
    org_std = cvbs_st->std_mode;      
    video_std = readl(0xB04023A0);


    if((video_std&0x04)==0)
        cvbs_st->cvd_v_loss_times++;
    //printf("readl(0xB04023A0)=%x\n",video_std);
    //printf("readl(0xB0400500)=%x\n",readl(0xB0400500));   
    //printf("cvbs_st->auto_freerun_mode=%x\n",cvbs_st->auto_freerun_mode);
    //printf("readl(0xB0400910)=%x\n",readl(0xB0400910));  
	 
    /*now no signal */
    if ((video_std&0x06)==0x00)
    {        
        //printf("return AVIN_CVBS_CLOSE1\n");
        vs_period=0;
        get_count=0;
        //cvbs2_detect_freerun_close(cvbs_st);
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        return AVIN_CVBS_CLOSE;
    }
     /*now have signal & otg_std no signal*/
    //else if ( (!(readl(0xB04025A0)& 0x01) ) && (org_std == AVIN_CVBS_CLOSE) )
    else if ( ((video_std& 0x06)==0x06 ) && (org_std == AVIN_CVBS_CLOSE) )
    {   
        //printf("Detect sig loss. \n");
        sig_detc_times = 0;
        
        /*detect NTSC or PAL*/
        cvbs_st->std_mode = cvbs2_get_video_type(cvbs_st->std_mode);

		/*ADC auto Gain Control*/ 
		//ADC_auto_gain_control();
		
		return cvbs_st->std_mode;
       
    }	
    /*now have signal & otg_std have signal*/
    else if ( ((video_std& 0x06)==0x06 )&&  (org_std != AVIN_CVBS_CLOSE))
    {
        //printf("cvbs_st->auto_freerun_mode = %d\n",cvbs_st->auto_freerun_mode);
        
		sig_detc_times++;
       	//if ((cvbs_st->auto_freerun_mode == 0)&&(readl(0xB04000e0&0x20)==0))
//        cvbs2_cdto_check();
        cvbs2_fix_cvd_nocolor();
		cvbs_st->std_mode = cvbs2_get_video_type(cvbs_st->std_mode);
		//printf("cvbs_st->std_mode = %d, cvbs_st->auto_freerun_mode = %d\n",cvbs_st->std_mode,cvbs_st->auto_freerun_mode);
		
		/*ADC auto Gain Control*/ 
		//ADC_auto_gain_control();
				
		return cvbs_st->std_mode;
    }	
    else{
        
        return cvbs_st->std_mode;
    }
} int cvbs2_detect_wait(struct cvbs_state *cvbs_st)
{ 
    unsigned char org_std ;
    static unsigned short sig_detc_times;
    //unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
   
	unsigned char video_std;	
    org_std = cvbs_st->std_mode;      
    video_std = readl(0xB04023A0);


    if((video_std&0x04)==0)
        cvbs_st->cvd_v_loss_times++;
    //printf("readl(0xB04023A0)=%x\n",video_std);
    //printf("readl(0xB0400500)=%x\n",readl(0xB0400500));   
    //printf("cvbs_st->auto_freerun_mode=%x\n",cvbs_st->auto_freerun_mode);
    //printf("readl(0xB0400910)=%x\n",readl(0xB0400910));  
	 
    /*now no signal */
    if ((video_std&0x06)==0x00)
    {        
        //printf("return AVIN_CVBS_CLOSE1\n");
        vs_period=0;
        get_count=0;
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        return AVIN_CVBS_CLOSE;
    }
     /*now have signal & otg_std no signal*/
    else if ( ((video_std& 0x06)==0x06 ) && (org_std == AVIN_CVBS_CLOSE) )
    {   
        //printf("Detect sig loss. \n");
        sig_detc_times = 0;
        
        /*detect NTSC or PAL*/
        cvbs_st->std_mode = cvbs2_get_stable_video_type();
		
		return cvbs_st->std_mode;
       
    }	
    /*now have signal & otg_std have signal*/
    else if ( ((video_std& 0x06)==0x06 )&&  (org_std != AVIN_CVBS_CLOSE))
    {
        //printf("cvbs_st->auto_freerun_mode = %d\n",cvbs_st->auto_freerun_mode);
		sig_detc_times++;
		cvbs_st->std_mode = cvbs2_get_stable_video_type();
		//printf("cvbs_st->std_mode = %d, cvbs_st->auto_freerun_mode = %d\n",cvbs_st->std_mode,cvbs_st->auto_freerun_mode);
		
				
		return cvbs_st->std_mode;
    }	
    else{
        return cvbs_st->std_mode;
    }
} 


int cvbs2_detect656(struct cvbs_state *cvbs_st)
{ 
    unsigned char org_std ;
    static unsigned short sig_detc_times;
    //unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
   
	unsigned char video_std;	
    org_std = cvbs_st->std_mode;      
    video_std = readl(0xB0402E00);


    if((video_std&0x07))
        cvbs_st->cvd_v_loss_times++;
    writel(video_std&0x07, 0xB0402E00);
    //printf("readl(0xB0402E00)=%x\n",video_std);
    //printf("readl(0xB0400500)=%x\n",readl(0xB0400500));   
    //printf("cvbs_st->auto_freerun_mode=%x\n",cvbs_st->auto_freerun_mode);
    //printf("readl(0xB0400910)=%x\n",readl(0xB0400910));  
	 
    /*now no signal */
    if ((video_std&0x03))
    {        
        //printf("return AVIN_CVBS_CLOSE1\n");
        vs_period=0;
        get_count=0;
        //cvbs2_detect_freerun_close(cvbs_st);
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        //Reset 656 v decode for signal detect.
        writel(0x10, 0xB04001B0);
        writel(0x00, 0xB04001C0);
        return AVIN_CVBS_CLOSE;
    }
     /*now have signal & otg_std no signal*/
    //else if ( (!(readl(0xB04025A0)& 0x01) ) && (org_std == AVIN_CVBS_CLOSE) )
    else if ( ((video_std& 0x03)==0x00 ) && (org_std == AVIN_CVBS_CLOSE) )
    {   
        //printf("Detect sig loss. \n");
        sig_detc_times = 0;
        
        /*detect NTSC or PAL*/
        cvbs_st->std_mode = cvbs2_get_video_type(cvbs_st->std_mode);

		/*ADC auto Gain Control*/ 
		//ADC_auto_gain_control();
		
		return cvbs_st->std_mode;
       
    }	
    /*now have signal & otg_std have signal*/
    else if ( ((video_std& 0x03)==0x00 )&&  (org_std != AVIN_CVBS_CLOSE))
    {
        //printf("cvbs_st->auto_freerun_mode = %d\n",cvbs_st->auto_freerun_mode);
		sig_detc_times++;
       	//if ((cvbs_st->auto_freerun_mode == 0)&&(readl(0xB04000e0&0x20)==0))

		cvbs_st->std_mode = cvbs2_get_video_type(cvbs_st->std_mode);
		//printf("cvbs_st->std_mode = %d, cvbs_st->auto_freerun_mode = %d\n",cvbs_st->std_mode,cvbs_st->auto_freerun_mode);
		
		/*ADC auto Gain Control*/ 
		//ADC_auto_gain_control();
				
		return cvbs_st->std_mode;
    }	
    else{
        return cvbs_st->std_mode;
    }
} 

int cvbs2_detect656_wait(struct cvbs_state *cvbs_st)
{ 
    unsigned char org_std ;
    static unsigned short sig_detc_times;
    //unsigned long HS_REG_MSB_THRESHOLD = 0xD7;
   
	unsigned char video_std;	
    org_std = cvbs_st->std_mode;      
    video_std = readl(0xB0402E00);


    if((video_std&0x07))
        cvbs_st->cvd_v_loss_times++;
    writel(video_std&0x07, 0xB0402E00);
    //printf("readl(0xB0402E00)=%x\n",video_std);
    //printf("readl(0xB0400500)=%x\n",readl(0xB0400500));   
    //printf("cvbs_st->auto_freerun_mode=%x\n",cvbs_st->auto_freerun_mode);
    //printf("readl(0xB0400910)=%x\n",readl(0xB0400910));  
	 
    /*now no signal */
    if ((video_std&0x03))
    {        
        //printf("return AVIN_CVBS_CLOSE1\n");
        vs_period=0;
        get_count=0;
        cvbs_st->std_mode = AVIN_CVBS_CLOSE;
        //Reset 656 v decode for signal detect.
        writel(0x10, 0xB04001B0);
        writel(0x00, 0xB04001C0);
        return AVIN_CVBS_CLOSE;
    }
     /*now have signal & otg_std no signal*/
    else if ( ((video_std& 0x03)==0x00 ) && (org_std == AVIN_CVBS_CLOSE) )
    {   
        //printf("Detect sig loss. \n");
        sig_detc_times = 0;
        
        /*detect NTSC or PAL*/
        cvbs_st->std_mode = cvbs2_get_stable_video_type();
		
		return cvbs_st->std_mode;
       
    }	
    /*now have signal & otg_std have signal*/
    else if ( ((video_std& 0x03)==0x00 )&&  (org_std != AVIN_CVBS_CLOSE))
    {
        //printf("cvbs_st->auto_freerun_mode = %d\n",cvbs_st->auto_freerun_mode);
		sig_detc_times++;
		cvbs_st->std_mode = cvbs2_get_stable_video_type();
		//printf("cvbs_st->std_mode = %d, cvbs_st->auto_freerun_mode = %d\n",cvbs_st->std_mode,cvbs_st->auto_freerun_mode);
		
				
		return cvbs_st->std_mode;
    }	
    else{
        return cvbs_st->std_mode;
    }
} 
