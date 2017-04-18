/* 
 *  @file   volume.c 
 *  @brief  volume driver APIs
 *
 *  $Id: volume.c,v 1.1 2014/02/27 07:53:12 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *  @date   2014/02/27  Kevin    New file.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys.h>
#include <io.h>
#include <errno.h>
#include <debug.h>

#include "volume.h"
static int vol_set_now = 0;

const unsigned char vol_table[VOL_LEVELS] = {0x0A, 0x0C, 0x0F, 0x13, 0x18, 0x1D, 0xE8, 0xEA,
                                             0xED, 0xF0, 0xF4, 0xFA, 0x4B, 0x50, 0x5D, 0x3F}; //0.8v~3v
/*
 * @func       vol_up
 * @brief      volume up
 * @param     
 * @return     volumee
*/                                  

int vol_up(void){
    assert(vol_set_now>=0);
    vol_set_now++;
    if(vol_set_now<0){
        vol_set_now = 0;
    }
    if(vol_set_now>=VOL_LEVELS){
        vol_set_now = VOL_LEVELS-1;
    }
    //Set Volume
    writel(vol_table[vol_set_now], PWM_TCREG);
   
    return vol_set_now;
}

/*
 * @func       vol_down
 * @brief      volume down
 * @param     
 * @return     volume
*/                                  

int vol_down(void){
    assert(vol_set_now>=0);
    vol_set_now--;
    if(vol_set_now<0){
        vol_set_now = 0;
    }
    if(vol_set_now>=VOL_LEVELS){
        vol_set_now = VOL_LEVELS-1;
    }
    //Set Volume
    writel(vol_table[vol_set_now], PWM_TCREG);
 
    return vol_set_now;
    
}

/*
 * @func       vol_set
 * @brief      Set volume
 * @param      volume
 * @return     volume
*/   
int vol_set(int volume){
    assert(volume>=0);
    if(volume<0){
        ERROR("Wrong volume valume! %d\n", volume);
        volume = 0;
    }
    if(volume>=VOL_LEVELS){
        ERROR("Wrong volume valume! %d\n", volume);
        volume = VOL_LEVELS-1;
    }
    //Set Volume
    writel(vol_table[volume], PWM_TCREG);
    vol_set_now = volume;
    return vol_set_now;
}

/*
 * @func       vol_reset
 * @brief      reset volume to middle.
 * @param     
 * @return     volume
*/   
int vol_reset(void){
    //Enable PWM and set to Alternative Mode with 13.1843kHz
    writel(0x34, PWM_GCREG);
    
    //Set Volume
    writel(vol_table[VOL_LEVELS>>1], PWM_TCREG);
    vol_set_now = VOL_LEVELS>>1;
    return vol_set_now;
}

/*
 * @func       vol_init
 * @brief      Init volume control.(enable PWM)
 * @param      Set Volume
 * @return     volume
*/   
int vol_init(int volume){
    
    assert(volume>=0);
    if(volume<0){
        ERROR("Wrong volume valume! %d\n", volume);
        volume = 0;
    }
    if(volume>=VOL_LEVELS){
        ERROR("Wrong volume valume! %d\n", volume);
        volume = VOL_LEVELS-1;
    }
    //Enable PWM and set to Alternative Mode with 13.1843kHz
    writel(0x34, PWM_GCREG);
    
    //Set Volume
    writel(vol_table[volume], PWM_TCREG);
    vol_set_now = volume;
    return vol_set_now;
}

/*
 * @func       vol_close
 * @brief      disable PWM
 * @param     
 * @return     0
*/   
int vol_close(void){
        
    //Disable PWM 
    writel(0x00, PWM_GCREG);
    return 0;
}
