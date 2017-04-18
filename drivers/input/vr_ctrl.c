/**
 *  @file   vr_ctrl.c
 *  @brief  Cheetah VR Read driver 
 *  $Id: vr_ctrl.c,v 1.4 2014/07/16 03:44:17 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/07  Kevin  New file.
 *
 */

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <debug.h>
#include <io.h>
#include <gpio.h>
#include "vr_ctrl.h"


#define ASWITCH_TIME            50 //= 50 uSec

#define GET_DELAY               (2000000/ASWITCH_TIME)

#define REG_SCALER_BASE         0xb0400000
#define PAGE_0                  0
#define REG_SCALER_INT_STS_MASK (REG_SCALER_BASE | PAGE_0 | (0x34 << 4))
#define REG_SCALER_SAR_MASK     (REG_SCALER_BASE | PAGE_0 | (0xE7 << 4))
#define PAGE_1                  0x1000
#define REG_SAR0_CTRL           (REG_SCALER_BASE | PAGE_1 | (0x70 << 4))
#define REG_SAR0_THRES          (REG_SCALER_BASE | PAGE_1 | (0x72 << 4))
#define REG_SAR_SEL             (REG_SCALER_BASE | PAGE_1 | (0x78 << 4))
#define REG_SAR0_RLTM_VAL       (REG_SCALER_BASE | PAGE_1 | (0x79 << 4))
#define REG_SAR0_LTCH_VAL       (REG_SCALER_BASE | PAGE_1 | (0x7B << 4))

#define INT_STS_MASK_SAR_TOG    0x01
#define SCALER_SAR0_MASK        0x20
#define SCAKER_SAR0_TOG         0x02

#define SAR0_VALUE_MASK         0xF0


/*Please select another gpio to control switch in T582!!!!!!! */
#define GPIO_ASWITCH   (1 << 13)   // GPIO Pin for CS
/**************************************************************/

#define SET_ASWITCH_HIGH     writel (GPIO_ASWITCH, GPIO_SET)
#define SET_ASWITCH_LOW      writel (GPIO_ASWITCH, GPIO_CLEAR)


struct vr_data_t vr_data = {0,0,0,0}; 
static unsigned char g_threshold = 0;
static int pre_clk=0;
static unsigned int star_clk=0;
static unsigned char stage;
static unsigned char pre_v0=0;
static unsigned char pre_v1=0;
    
#define STG_INIT    0
#define STG_GETV0   1   
#define STG_GETV1   2

static int inline k_abs(int a)
{
    if(a>0)
        return a;
    else
        return -a;
}

static int __attribute__((unused))
init_vrdata(void)
{
    unsigned short temp=0;
    int i;       
    //Select amux 0
    SET_ASWITCH_LOW;
    //Wait it stable.
    usleep(1000);     
    temp = 0;
    for(i=0; i<8; i++){
        temp += readl(REG_SAR0_RLTM_VAL);
        usleep(100); 
    }
    temp >>= 3;
    if(temp>255)
        temp = 255;    
    vr_data.just_value0 = temp&0xF8;
    vr_data.pre_value0 = vr_data.just_value0;
    pre_v0 = temp;
    //Select amux 1
    SET_ASWITCH_HIGH;
    //Wait it stable.
    usleep(1000);   
    temp = 0;
    for(i=0; i<8; i++){
        temp += readl(REG_SAR0_RLTM_VAL);
        usleep(100); 
    }
    temp >>= 3;  
    if(temp>255)
        temp = 255;
    vr_data.just_value1 = temp&0xF8;
    vr_data.pre_value1 = vr_data.just_value1;
    pre_v1 = temp;
    star_clk = read_c0_count32();
    
         
    //Select amux 0
    SET_ASWITCH_LOW;
    return 0;
}
/*
 * Synopsis	int vr_open (unsigned char threshold);
 * Description: Init VR Driver.
 * Parameters:	threshold - VR resolution.(mask bit.)
 * Return:	0:Init success. 
 */
int vr_open(unsigned char threshold)
{
#if 0    
    
    //Config pin mux function at VAL_SYSMGM_ALTFUNC_SEL in sysclock.c by Green Tool.
    
    /* Enable GPIO 13. Set it as GPO*/
    writel(readl(GPIO_OE) | GPIO_ASWITCH, GPIO_OE);
    /* select SAR0 source */
    writeb(readb(REG_SAR_SEL) & ~0xC0, REG_SAR_SEL);

    /* Set threshold */
    writeb(0x08, REG_SAR0_THRES);

    /* Power on SAR0 */
    writeb(readb(REG_SAR0_CTRL) & ~0x02, REG_SAR0_CTRL);

    /* not mask SAR0 of interrupt */
    writeb(readb(REG_SCALER_SAR_MASK) & ~0x20, REG_SCALER_SAR_MASK);

    /* clear SAR0 of interrupt status */
    writeb(readb(REG_SCALER_SAR_MASK) & 0x02, REG_SCALER_SAR_MASK);

    g_threshold = threshold;
    init_vrdata();
    pre_clk = read_c0_count32();
    stage = STG_INIT;
#else
    dbg(0,"Please select another gpio to control switch in T582!\n");
    dbg(0,"T582 SDK default not support VR now!\n");
#endif
    return 0;
}
/*
 * Synopsis	int vr_read (struct vr_data_t *p_vr_data);
 * Description: Get VR code.
 * Parameters:	p_vr_data - Return vr code.
 * Return:	0:Get VR code.  -1:Not get VR code.
 */
int vr_read(struct vr_data_t *p_vr_data)
{
    unsigned short temp=0;
    int i;
    int just_clk;
    if(p_vr_data==NULL)
        return -1;
    just_clk = read_c0_count32();
    if(((just_clk - pre_clk)<(sys_cpu_clk/6))&&(just_clk>pre_clk)){
        *p_vr_data = vr_data;
        return -1;
    }
    else{
        pre_clk = just_clk; 
    }
    //Select amux 0
    SET_ASWITCH_LOW;
    //Wait it stable.
    usleep(1000);     
    temp = 0;
    for(i=0; i<8; i++){
        temp += readl(REG_SAR0_RLTM_VAL);
        usleep(100); 
    }
    temp >>= 3;
    if(k_abs(temp-vr_data.pre_value0)>=g_threshold){
        vr_data.pre_value0 = vr_data.just_value0;
        vr_data.just_value0 = temp;
    }
    else{
        vr_data.just_value0 = temp;
    }
    
    //Select amux 1
    SET_ASWITCH_HIGH;
    //Wait it stable.
    usleep(1000);   
    temp = 0;
    for(i=0; i<8; i++){
        temp += readl(REG_SAR0_RLTM_VAL);
        usleep(100); 
    }
    temp >>= 3;  
    if(k_abs(temp-vr_data.pre_value1)>=g_threshold){
        vr_data.pre_value1 = vr_data.just_value1;
        vr_data.just_value1 = temp;
    }
    else{
        vr_data.just_value1 = temp;
    }
    //Select amux 0
    SET_ASWITCH_LOW;
    *p_vr_data = vr_data;
    return 0;
}



/*
 * Synopsis	int vr_check (struct vr_data_t *p_vr_data);
 * Description: Check VR code.
 * Parameters:	p_vr_data - Return vr code.
 * Return:	0:Get VR code.  -1:Not get VR code.
 */
int vr_check(struct vr_data_t *p_vr_data)
{
    static short temp=0;
    short get_data=0;
    static unsigned char get_count=0;
    
    
    if(p_vr_data==NULL)
        return -1;
    
    if(stage == STG_INIT){
        //if(is_time_out(star_clk+millisec(1000))){
        if((int)(read_c0_count32() - (star_clk+(sys_cpu_clk / 2000000))) > 0){            
        
        }
        else{
            *p_vr_data = vr_data;
            return -1;
        }
    }
    else if(stage == STG_GETV0){
        //if(is_time_out(star_clk+millisec(1000))){
        if(get_count==0){
            if((int)(read_c0_count32() - (star_clk+(sys_cpu_clk / GET_DELAY))) > 0){
                
                
            }
            else{
                *p_vr_data = vr_data;
                return -1;
            }
        
        }
        else{
            if((int)(read_c0_count32() - (star_clk+(sys_cpu_clk / 2000000))) > 0){
                
                
            }
            else{
                *p_vr_data = vr_data;
                return -1;
            }
        }
    }
    else if(stage == STG_GETV1){
        //if(is_time_out(star_clk+millisec(1000))){
        if(get_count==0){
            if((int)(read_c0_count32() - (star_clk+(sys_cpu_clk / GET_DELAY))) > 0){
                
            }
            else{
                *p_vr_data = vr_data;
                return -1;
                
            }
        }
        else{
            if((int)(read_c0_count32() - (star_clk+(sys_cpu_clk / 2000000))) > 0){
                
            }
            else{
                *p_vr_data = vr_data;
                return -1;
                
            }
        }
    }
    else{
        return -1;
    }
    

    switch(stage){
        case STG_INIT: 
            //Select amux 0            
            SET_ASWITCH_LOW;
            star_clk = read_c0_count32();
            stage = STG_GETV0;
            break;
        case STG_GETV0:
            get_data = readl(REG_SAR0_RLTM_VAL);
            temp += get_data;
            get_count++;
            if(get_count>=8){
                temp = (temp>>3);
                
                if(k_abs(temp-((short)(pre_v0)))>(g_threshold>>2)){
                    pre_v0 = temp;
                }
                else{
                    temp = pre_v0;
                }
                if(temp>255)
                    temp = 255;
                vr_data.pre_value0 = vr_data.just_value0;
                if(temp<255)
                    vr_data.just_value0 = temp&(~g_threshold);
                else
                    vr_data.just_value0 = temp;
            }
            
            star_clk = read_c0_count32();
            if(get_count>=8){
                stage = STG_GETV1;
                get_count = 0;
                temp = 0;
                //Select amux 1
                SET_ASWITCH_HIGH;
            }
            break;
        case STG_GETV1:
            get_data = readl(REG_SAR0_RLTM_VAL);
            temp += get_data;
            get_count++;
            if(get_count>=8){
                temp = (temp>>3);
                
                if(k_abs(temp-((short)(pre_v1)))>(g_threshold>>2)){
                    pre_v1 = temp;
                }
                else{
                    temp = pre_v1;
                }
                if(temp>255)
                    temp = 255;
                vr_data.pre_value1 = vr_data.just_value1;
                if(temp<255)
                    vr_data.just_value1 = temp&(~g_threshold);
                else
                    vr_data.just_value1 = temp;
            }
                
            
            star_clk = read_c0_count32();
            if(get_count>=8){
                stage = STG_INIT;
                get_count = 0;
                temp = 0;
                //Select amux 0
                SET_ASWITCH_LOW;
            }
            break;    
    }
    

    *p_vr_data = vr_data;
    return 0;
}
int vr_close(void)
{
    return 0;
}

