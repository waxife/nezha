/**
 *  @file   sh_vr_ctrl.c
 *  @brief  vr test program
 *  $Id: sh_vr_ctrl.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/07  Kevin    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <unistd.h>
#include <sys.h>
#include <ctype.h>
#include <debug.h>
#include "serial.h"
#include "drivers/input/vr_ctrl.h"

#define CHANGE_THRESHOLD 15
command_init (sh_vr_test, "vr_test", "vr_test");

static int k_abs(int a)
{
    if(a>0)
        return a;
    else
        return -a;
}

static int sh_vr_test(int argc, char **argv, void **result, void *sys_ctx)
{
    struct vr_data_t vr_data;
    int c;
    int rc=0;
    int threshold = CHANGE_THRESHOLD;
    printf("VR Test Program.\n");
    printf("Default threadhold = 15.\n");
    printf("Press '-' and '+' to decrease and increase sensitivity.\n");
    
    //Init vr driver and set threadhold.
    vr_open(threshold);
   
    //Check vr value.
    rc = vr_check(&vr_data);
    printf("vr0=%d, vr1=%d, pre_vr0=%d, pre_vr1=%d\n",
            vr_data.just_value0,vr_data.just_value1,vr_data.pre_value0,vr_data.pre_value1);
    while (1) {
        if ((c = getb2()) >= 0)  {
            if (c == 'q') {
                goto EXIT;
            }
            else if((c == '=')||(c=='+')){
                threshold--;
                vr_open(threshold);
                printf("Increase vr sensitivity, threshold = %d.\n",threshold);
                
            }
            else if(c == '-'){
                threshold++;
                vr_open(threshold);
                printf("Decrease vr sensitivity, threshold = %d.\n",threshold);
                
            }
        }
        //Check vr value.
        rc = vr_check(&vr_data);
        if(rc==0){
            if((k_abs(vr_data.just_value0-vr_data.pre_value0)>threshold)||(k_abs(vr_data.just_value1-vr_data.pre_value1)>threshold)){
                printf("vr0=%d, vr1=%d, pre_vr0=%d, pre_vr1=%d\n",
                        vr_data.just_value0,vr_data.just_value1,vr_data.pre_value0,vr_data.pre_value1);

            }
        }
    }
    
EXIT: 
    vr_close();   
    return 0;
}

