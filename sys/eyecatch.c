/**
 *  @file   eyecatch.c
 *  @brief  eye-catch effect. fade in fade out.
 *  $Id: eyecatch.c,v 1.8 2014/04/09 07:54:47 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  New file.
 *
 */
#include <stdarg.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys.h>
#include "eyecatch.h"
#include "cvbs.h"
#include "./drivers/cq/cq.h"
#include <stdatomic.h>
#define DBG_HELPER 
#define DBG_LEVEL   0
#include <debug.h>


static int g_fade_out_mode = FD_NONE;
static int g_fade_in_mode = FD_NONE;
static short g_p0_68 = -1;
static short g_p0_6C = -1;
static atomic_1_t key = 0;
void ect_reset(void){
    g_fade_out_mode = FD_NONE;
    g_fade_in_mode = FD_NONE;
    if(g_p0_68 != -1){
        writel(g_p0_68, 0xb0400680);
        writel(0x00, 0xb0400E20);   
        g_p0_68 = -1;    
    } 
    else{
        writel(0x80, 0xb0400680);
        writel(0x00, 0xb0400E20);  
    }
    if(g_p0_6C != -1){
        writel(g_p0_6C, 0xb04006C0);
        writel(0x00, 0xb0400E20);   
        g_p0_6C = -1;    
    } 
    else{
        writel(0x80, 0xb04006C0);
        writel(0x00, 0xb0400E20);  
    }
}


/**
 * @func       cvbs_detect_change
 * @brief      check cvbs status change and get standard.
 * @param     
 * @return     0: no mode change.  1: mode change
 */
int ect_black_screen(int fade_in_mode, int fade_out_mode)
{
    short p0_68 = 0;
    short p0_6C = 0;   
    int error = 0;
    int wi = 60;
    if(atomic_flag_test_and_set_1(&key, 1))
        return 1;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
        
    g_fade_out_mode = fade_out_mode;
    g_fade_in_mode = fade_in_mode;
    dbg(1,"ect_black_screen\n");
    if(g_p0_68!=-1){
        ERROR ("Fade in procedure error!\n");
        error = 1;
    }
    else{
        p0_68 = readl(0xb0400680);
        if(p0_68 == 0){
            ERROR("read P0_68 = 0\n");           
            error = 1;
            goto EXIT;
        }
        else{
            if(g_fade_in_mode != FD_NONE)
                g_p0_68 = p0_68;
        }
    }
    if(g_p0_6C!=-1){
        ERROR ("Fade in procedure error!\n");
        error = 1;
    }
    else{
        p0_6C = readl(0xb04006C0);
        if(p0_6C==0){
            ERROR("read P0_6C = 0\n");
            p0_6C = g_p0_6C = -1;
            error = 1;
            goto EXIT; 
        }
        else{
            if(g_fade_in_mode != FD_NONE)
                g_p0_6C = p0_6C;
        }
    }

    
    
    dbg(1,"Start p0_68 = %d, p0_6C = %d\n", g_p0_68, g_p0_6C);
    switch(fade_in_mode){
        case FD_NONE:
            cq_write_byte(CQ_P0, 0x91,0x87);
            if (cq_flush_vde () < 0)
    		        ERROR ("cq_flush_now ()\n");
        break;
        
        case FD_SLOW:
            while((p0_68>0)||(p0_6C>0)){
                p0_68 -= 2; 
    		    p0_6C -= 2;
                if(p0_68>0){
                    cq_write_byte(CQ_P0, 0x68,p0_68);
                }
                else{
                    cq_write_byte(CQ_P0, 0x68, 0);
                }
                if(p0_6C>0){
                    cq_write_byte(CQ_P0, 0x6C,p0_6C);
                }
                else{
                    cq_write_byte(CQ_P0, 0x6C, 0);
                }
                if (cq_flush_vde () < 0)
    		        ERROR ("cq_flush_now ()\n");
    		    
    		    usleep(10000);

            }
            cq_write_byte(CQ_P0, 0x91,0x87);
            if (cq_flush_vde () < 0)
    		        ERROR ("cq_flush_now ()\n");
        break;
        
        case FD_FAST:
            while((p0_68>0)||(p0_6C>0)){
                p0_68 -= 8; 
    		    p0_6C -= 8;
                if(p0_68>0){
                    cq_write_byte(CQ_P0, 0x68,p0_68);
                }
                else{
                    cq_write_byte(CQ_P0, 0x68, 0);
                }
                if(p0_6C>0){
                    cq_write_byte(CQ_P0, 0x6C,p0_6C);
                }
                else{
                    cq_write_byte(CQ_P0, 0x6C, 0);
                }
                if (cq_flush_vde () < 0)
    		        ERROR ("cq_flush_now ()\n");
    		    
    		    usleep(10000);

            }
            cq_write_byte(CQ_P0, 0x91,0x87);
            if (cq_flush_vde () < 0)
    		        ERROR ("cq_flush_now ()\n");
        break;
        default:
            ERROR ("Unknow effect. ()\n");
            error = -1;
        break;
        
    }
EXIT:
    if(key != 1)
        ERROR ("Function Reentrant ()\n");
    atomic_flag_test_and_set_1(&key, 0);
    return error;
}

/**
 * @func       cvbs_detect_change
 * @brief      check cvbs status change and get standard.
 * @param     
 * @return     0: no mode change.  1: mode change
 */
int ect_black_screen_noblock(void)
{
    short p0_68 = 0;
    short p0_6C = 0;   
    int error = 0;
    int wi = 60;
    if(atomic_flag_test_and_set_1(&key, 1))
        return 1;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
        
    dbg(1,"ect_black_screen_nblock\n");  
    if(g_p0_68==-1){
        //p0_68 = g_p0_68 = readl(0xb0400680);  
        p0_68 = readl(0xb0400680);
        if(p0_68 == 0){
            ERROR("read P0_68 = 0\n");           
            error = 1;
            goto EXIT;
        }
        else{
            if(g_fade_in_mode != FD_NONE)
                g_p0_68 = p0_68;
        }
    }
    if(g_p0_6C==-1){
        //p0_6C = g_p0_6C = readl(0xb04006C0);
        p0_6C = readl(0xb04006C0);
        if(p0_6C==0){
            ERROR("read P0_6C = 0\n");
            p0_6C = g_p0_6C = -1;
            error = 1;
            goto EXIT; 
        }
        else{
            if(g_fade_in_mode != FD_NONE)
                g_p0_6C = p0_6C;
        }

    }
 
    p0_68 = readl(0xb0400680);
    p0_6C = readl(0xb04006C0);
    //printf("Start p0_68 = %d, p0_6C = %d\n", g_p0_68, g_p0_6C);
    switch(g_fade_in_mode){
        case FD_NONE:
            cq_write_byte(CQ_P0, 0x91,0x87);
            error = 2;
            if (cq_flush_vde () < 0)
    		        ERROR ("cq_flush_now ()\n");
        break;
        
        case FD_SLOW:           
            if((p0_68<=0)&&(p0_6C<=0)){
                error = 2;
                cq_write_byte(CQ_P0, 0x91,0x87);
                if (cq_flush_vde () < 0)
        		        ERROR ("cq_flush_now ()\n");
                break;
            }
            p0_68 -= 2; 
		    p0_6C -= 2;    
            if(p0_68>0){
                cq_write_byte(CQ_P0, 0x68,p0_68);
            }
            else{
                cq_write_byte(CQ_P0, 0x68,0);
            }
            if(p0_6C>0){
                cq_write_byte(CQ_P0, 0x6C,p0_6C);
            }
            else{
                cq_write_byte(CQ_P0, 0x6C,0);
            }
            
            if (cq_flush_vde () < 0)
		        ERROR ("cq_flush_now ()\n");
		    
		             
            
    		usleep(10000);  
        break;
        
        case FD_FAST:
            if((p0_68<=0)&&(p0_6C<=0)){
                error = 2;
                cq_write_byte(CQ_P0, 0x91,0x87);
                if (cq_flush_vde () < 0)
        		        ERROR ("cq_flush_now ()\n");
                break;
            }
            p0_68 -= 8; 
		    p0_6C -= 8;    
            if(p0_68>0){
                cq_write_byte(CQ_P0, 0x68,p0_68);
            }
            else{
                cq_write_byte(CQ_P0, 0x68,0);
            }
            if(p0_6C>0){
                cq_write_byte(CQ_P0, 0x6C,p0_6C);
            }
            else{
                cq_write_byte(CQ_P0, 0x6C,0);
            }
            
            if (cq_flush_vde () < 0)
		        ERROR ("cq_flush_now ()\n");
		    
    		usleep(10000);  
        break;
        default:
            ERROR ("Unknow effect. ()\n");
            error = -1;
        break;
        
    }
EXIT:
    if(key != 1)
        ERROR ("Function Reentrant ()\n");
    atomic_flag_test_and_set_1(&key, 0);
    return error;
}
/**
 * @func       cvbs_detect_change
 * @brief      check cvbs status change and get standard.
 * @param     
 * @return     0: no mode change.  1: mode change
 */
int ect_black_screen_noblock_ncq(void)
{
    short p0_68 = 0;
    short p0_6C = 0;   
    int error = 0;
    int wi = 60;
    if(atomic_flag_test_and_set_1(&key, 1))
        return 1;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
        
    dbg(1,"ect_black_screen_nblock_ncq\n"); 
    if(g_p0_68==-1){
        //p0_68 = g_p0_68 = readl(0xb0400680);               
        p0_68 = readl(0xb0400680);
        if(p0_68 == 0){
            ERROR("read P0_68 = 0\n");           
            error = 1;
            goto EXIT;
        }
        else{
            if(g_fade_in_mode != FD_NONE)
                g_p0_68 = p0_68;
        }
    }
    if(g_p0_6C==-1){
        //p0_6C = g_p0_6C = readl(0xb04006C0);
        p0_6C = readl(0xb04006C0);
        if(p0_6C==0){
            ERROR("read P0_6C = 0\n");
            p0_6C = g_p0_6C = -1;
            error = 1;
            goto EXIT; 
        }
        else{
            if(g_fade_in_mode != FD_NONE)
                g_p0_6C = p0_6C;
        }
   
    }
    p0_68 = readl(0xb0400680);
    p0_6C = readl(0xb04006C0);
    //printf("Start p0_68 = %d, p0_6C = %d\n", g_p0_68, g_p0_6C);
    switch(FD_NONE){
        case FD_NONE:
            writel(0x87, 0xb0400910);
        	writel(0x00, 0xb0400E20);
            error = 2;
        break;
        
        case FD_SLOW:           
            if((p0_68<=0)&&(p0_6C<=0)){
                error = 2;
        		writel(0x87, 0xb0400910);
        		writel(0x00, 0xb0400E20);
                break;
            }
            p0_68 -= 2; 
		    p0_6C -= 2;    
            if(p0_68>0){
                writel(p0_68, 0xb0400680);                
            }
            else{
                writel(0, 0xb0400680);
            }
            if(p0_6C>0){
                writel(p0_6C, 0xb04006C0);
            }
            else{
                writel(0, 0xb04006C0);
            }
            
            writel(0x00, 0xb0400E20);      		             
            usleep(30000); 
        break;
        
        case FD_FAST:
            if((p0_68<=0)&&(p0_6C<=0)){
                error = 2;
        		writel(0x87, 0xb0400910);
        		writel(0x00, 0xb0400E20);
                break;
            }
            p0_68 -= 8; 
		    p0_6C -= 8;    
            if(p0_68>0){
                writel(p0_68, 0xb0400680);                
            }
            else{
                writel(0, 0xb0400680);
            }
            if(p0_6C>0){
                writel(p0_6C, 0xb04006C0);
            }
            else{
                writel(0, 0xb04006C0);
            }
            
            writel(0x00, 0xb0400E20);
		    usleep(30000); 
        break;
        default:
            ERROR ("Unknow effect. ()\n");
            error = -1;
        break;
        
    }
EXIT:
    if(key != 1)
        ERROR ("Function Reentrant ()\n");
    atomic_flag_test_and_set_1(&key, 0);
    return error;
}


/**
 * @func       cvbs_get_standard
 * @brief      get cvd standard.
 * @param     
 * @return     cvbs standart
 */
int ect_play_screen(void)
{
    short p0_68 = 0;
    short p0_6C = 0;   
    int error = 0;
    int wi=60;
    if(atomic_flag_test_and_set_1(&key, 1))
        return 1;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
        
    dbg(1, "ect_play_screen\n");
    if(g_p0_68==-1){
        dbg (1,"Fade out procedure error!\n");
        g_fade_out_mode = FD_NONE;
        error = 2;
    }
    if(g_p0_6C==-1){
        dbg (1,"Fade out procedure error!\n");
        g_fade_out_mode = FD_NONE;
        error = 2;
    }
    dbg(1, "Set g_p0_68 = %d, g_p0_6C = %d\n", g_p0_68, g_p0_6C);
    if(!(readl(0xb04000E0)&0x20))
        cvbs2_resync_fpll();
   
    switch(g_fade_out_mode){
        case FD_NONE:
            if(g_p0_68!=-1)
                cq_write_byte(CQ_P0, 0x68,g_p0_68);
            if(g_p0_6C!=-1)
                cq_write_byte(CQ_P0, 0x6C,g_p0_6C);
            cq_write_byte(CQ_P0, 0x91,0x07);
            if (cq_flush_now () < 0)
    		        ERROR ("cq_flush_now ()\n");
            g_p0_68 = -1; g_p0_6C = -1;
        break;
        
        case FD_SLOW:
            cq_write_byte(CQ_P0, 0x91,0x07);
            while((p0_68 < g_p0_68)||(p0_6C < g_p0_6C)){
                if(p0_68 <= g_p0_68){
                    cq_write_byte(CQ_P0, 0x68,p0_68);
                    p0_68 += 2;
                }
                if(p0_6C <= g_p0_6C){
                    cq_write_byte(CQ_P0, 0x6C,p0_6C);
                    p0_6C += 2;
                }
                if (cq_flush_now () < 0)
    		        ERROR ("cq_flush_now ()\n");
    		    usleep(10000);

            }
         
             if(g_p0_68 != -1)
                cq_write_byte(CQ_P0, 0x68, g_p0_68);
            if(g_p0_6C != -1)
                cq_write_byte(CQ_P0, 0x6C, g_p0_6C);
            if (cq_flush_now () < 0)
		        ERROR ("cq_flush_now ()\n");
           
            g_p0_68 = -1; g_p0_6C = -1;
        break;
        
        case FD_FAST:
            cq_write_byte(CQ_P0, 0x91,0x07);
            while((p0_68 < g_p0_68)||(p0_6C < g_p0_6C)){
                if(p0_68 <= g_p0_68){
                    cq_write_byte(CQ_P0, 0x68,p0_68);
                    p0_68 += 8;
                }
                if(p0_6C <= g_p0_6C){
                    cq_write_byte(CQ_P0, 0x6C,p0_6C);
                    p0_6C += 8;
                }
                if (cq_flush_now () < 0)
    		        ERROR ("cq_flush_now ()\n");
    		    usleep(10000);

            }
            if(g_p0_68 != -1)
                cq_write_byte(CQ_P0, 0x68, g_p0_68);
            if(g_p0_6C != -1)
                cq_write_byte(CQ_P0, 0x6C, g_p0_6C);
            if (cq_flush_now () < 0)
		        ERROR ("cq_flush_now ()\n");
            g_p0_68 = -1; g_p0_6C = -1;
        break;
        default:
            ERROR ("Unknow effect. ()\n");
            error = -1;
        break;
        
    }
    if(key != 1)
        ERROR ("Function Reentrant ()\n");
    atomic_flag_test_and_set_1(&key, 0);
    return error;
}
/**
 * @func       cvbs_get_standard
 * @brief      get cvd standard.
 * @param     
 * @return     cvbs standart
 */
int ect_play_screen_noblock(void)
{
    short p0_68 = 0;
    short p0_6C = 0;   
    int error = 0;
    int wi=60;
    if(atomic_flag_test_and_set_1(&key, 1))
        return 1;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
    dbg(1,"ect_play_screen_noblock_\n"); 
    p0_68 = readl(0xb0400680);
    p0_6C = readl(0xb04006C0);
    if(g_p0_68==-1){
        dbg (1, "Fade out procedure error!\n");
        g_fade_out_mode = FD_NONE;
        error = 2;
    }
    if(g_p0_6C==-1){
        dbg (1, "Fade in procedure error!\n");
        g_fade_out_mode = FD_NONE;
        error = 2;
    }
    if(g_p0_6C==0)
        ERROR ("g_p0_6C == 0!\n");
    dbg(2, "Start p0_68 = %d, p0_6C = %d\n", p0_68, p0_6C);
    switch(g_fade_out_mode){
        case FD_NONE:
            if(g_p0_68!=-1)
                cq_write_byte(CQ_P0, 0x68,g_p0_68);
            if(g_p0_6C!=-1)
                cq_write_byte(CQ_P0, 0x6C,g_p0_6C);
           
            cq_write_byte(CQ_P0, 0x91,0x07);            
            
        	if (cq_flush_now () < 0)
        		 ERROR ("cq_flush_now ()\n");
            g_p0_68 = -1; g_p0_6C = -1;
        break;
        
        case FD_SLOW:
            cq_write_byte(CQ_P0, 0x91,0x07);
            if(p0_68 <= g_p0_68)
                p0_68 += 2;
            if(p0_6C <= g_p0_6C)
                p0_6C += 2;
            if(p0_68 <= g_p0_68){
                cq_write_byte(CQ_P0, 0x68,p0_68);
            }
            if(p0_6C <= g_p0_6C){
                cq_write_byte(CQ_P0, 0x6C,p0_6C);
            }     
            
            if((p0_68 > g_p0_68)&&(p0_6C > g_p0_6C)){
                cq_write_byte(CQ_P0, 0x68, g_p0_68);
                cq_write_byte(CQ_P0, 0x6C, g_p0_6C);
                g_p0_68 = -1; g_p0_6C = -1;
            }
           
            if (cq_flush_now () < 0)
		        ERROR ("cq_flush_now ()\n"); 
		    usleep(10000);
        break;
        
        case FD_FAST:
            cq_write_byte(CQ_P0, 0x91,0x07);
            if(p0_68 <= g_p0_68)
                p0_68 += 8;
            if(p0_6C <= g_p0_6C)
                p0_6C += 8;
            if(p0_68 <= g_p0_68){
                cq_write_byte(CQ_P0, 0x68,p0_68);
            }
            if(p0_6C <= g_p0_6C){
                cq_write_byte(CQ_P0, 0x6C,p0_6C);
            }
            
            if((p0_68 > g_p0_68)&&(p0_6C > g_p0_6C)){
                cq_write_byte(CQ_P0, 0x68, g_p0_68);
                cq_write_byte(CQ_P0, 0x6C, g_p0_6C);
                g_p0_68 = -1; g_p0_6C = -1;
            }
            if (cq_flush_now () < 0)
		        ERROR ("cq_flush_now ()\n");
		    usleep(10000);
        break;
        default:
            ERROR ("Unknow effect. ()\n");
            error = -1;
        break;
        
    }
    if(key != 1)
        ERROR ("Function Reentrant ()\n");
    atomic_flag_test_and_set_1(&key, 0);
    return error;
}

/**
 * @func       cvbs_get_standard
 * @brief      get cvd standard.
 * @param     
 * @return     cvbs standart
 */
int ect_play_screen_noblock_ncq(void)
{
    short p0_68 = 0;
    short p0_6C = 0;   
    int error = 0;
    int wi=60;
    if(atomic_flag_test_and_set_1(&key, 1))
        return 1;
    //wait cq busy
    while(wi--){
        if(!cq_busy())
            break;
        usleep(1000); 
        
    }
    if(wi<0)
        ERROR ("CQ Block Scaler Register!\n");
    dbg(1,"ect_play_screen_noblock_ncq\n"); 
    p0_68 = readl(0xb0400680);
    p0_6C = readl(0xb04006C0);
    if(g_p0_68==-1){
        dbg (1, "Fade out procedure error!\n");
        g_fade_out_mode = FD_NONE;
        error = 2;
        
    }
    if(g_p0_6C==-1){
        dbg (1, "Fade out procedure error!\n");
        g_fade_out_mode = FD_NONE;
        error = 2;
      
    }
    if(g_p0_6C==0)
        ERROR ("Something Wrong g_p0_6C == 0!\n");
    dbg(2, "Set g_p0_68 = %d, g_p0_6C = %d\n", g_p0_68, g_p0_6C);
    switch(FD_NONE){
        case FD_NONE:
            
            writel(0x00, 0xb0400E20);
            wi = 20;
            if(g_p0_68!=-1){
                writel(g_p0_68, 0xb0400680);
                p0_68 = readl(0xb0400680);
                while((p0_68 != g_p0_68)&&(wi--)){
                    writel(g_p0_68, 0xb0400680);
                    p0_68 = readl(0xb0400680);
                   // usleep(1000);  //From experiment, it need 8m sec. to fill this register.
                }
                if(wi<0)
                    ERROR ("Fill P0_68 timeout!\n");
            }
            
            wi = 20;
            if(g_p0_6C!=-1){
                writel(g_p0_6C, 0xb04006C0);
                p0_6C = readl(0xb04006C0);
                while((p0_6C != g_p0_6C)&&(wi--)){
                    writel(g_p0_6C, 0xb04006C0);
                    p0_6C = readl(0xb04006C0);
                   // usleep(1000);  //From experiment, it need 8m sec. to fill this register.
                }
                if(wi<0)
                    ERROR ("Fill P0_68 timeout!\n");
            }
     
            writel(0x07, 0xb0400910); 
        	writel(0x00, 0xb0400E20);
            g_p0_68 = -1; g_p0_6C = -1;
        break;
        
        case FD_SLOW:
             writel(0x07, 0xb0400910);
	
            if(p0_68 <= g_p0_68)
                p0_68 += 2;
            if(p0_6C <= g_p0_6C)
                p0_6C += 2;
            if(p0_68 <= g_p0_68){
                writel(p0_68, 0xb0400680);
            }
            if(p0_6C <= g_p0_6C){
                writel(p0_6C, 0xb04006C0);
            }
            
            if((p0_68 > g_p0_68)&&(p0_6C > g_p0_6C)){
                writel(p0_68, 0xb0400680);
                writel(p0_6C, 0xb04006C0);
                g_p0_68 = -1; g_p0_6C = -1;
            }
            writel(0x00, 0xb0400e20);
		    usleep(10000); 
        break;
        
        case FD_FAST:
            
            writel(0x07, 0xb0400910);
	
            if(p0_68 <= g_p0_68)
                p0_68 += 8;
            if(p0_6C <= g_p0_6C)
                p0_6C += 8;
            if(p0_68 <= g_p0_68){
                writel(p0_68, 0xb0400680);
            }
            if(p0_6C <= g_p0_6C){
                writel(p0_6C, 0xb04006C0);
            }
            
            if((p0_68 > g_p0_68)&&(p0_6C > g_p0_6C)){
                writel(p0_68, 0xb0400680);
                writel(p0_6C, 0xb04006C0);
                g_p0_68 = -1; g_p0_6C = -1;
            }
            writel(0x00, 0xb0400e20);
		    usleep(10000); 
        break;
        default:
            ERROR ("Unknow effect. ()\n");
            error = -1;
        break;
        
    }
    if(key != 1)
        ERROR ("Function Reentrant ()\n");
    atomic_flag_test_and_set_1(&key, 0);
    return error;
}

