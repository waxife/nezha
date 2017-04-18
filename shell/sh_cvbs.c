/**
 *  @file   sh_cvbs.c
 *  @brief  CVBS test program
 *  $Id: sh_cvbs.c,v 1.1.1.1 2013/12/18 03:43:40 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/07      New file.
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
#include "sys/include/cvbs.h"


command_init (sh_cvbs_detect_init, "cvbs_detect_init", "cvbs_detect_init");
command_init (sh_cvbs_detect_close, "cvbs_detect_close", "cvbs_detect_close");
command_init (sh_cvbs_detect, "cvbs_detect", "cvbs_detect");
command_init (sh_cvbs_detect_chrmoalock, "cvbs_detect_chrmoalock", "cvbs_detect_chrmoalock");
command_init (sh_cvbs_detect_always, "cvbs_detect_always", "cvbs_detect_always");


static struct cvbs_state  cvbs_st ={0, 0} ;

static int sh_cvbs_detect_init (int argc, char **argv, void **result, void *sys_ctx)
{
    
    cvbs_detect_init(&cvbs_st);
    printf("cvbs_st->auto_freerun_mode %d\n", cvbs_st.auto_freerun_mode);

    return 0;
}

static int sh_cvbs_detect_close (int argc, char **argv, void **result, void *sys_ctx)
{
    
    cvbs_detect_close(&cvbs_st);
    printf("cvbs_st->auto_freerun_mode %d\n", cvbs_st.auto_freerun_mode);
    
    return 0;
}

static int sh_cvbs_detect(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    rc = cvbs_detect(&cvbs_st);
        
    if (rc== AVIN_CVBS_CLOSE)
        printf("cvbs detect no signal %02d\n", rc);
    else if(rc== AVIN_CVBS_NTSC)
        printf("cvbs detect NTSC signal %02d\n", rc);
    else if(rc== AVIN_CVBS_PAL)
        printf("cvbs detect PAL signal %02d\n", rc);
    

    return rc;
}

static int sh_cvbs_detect_always(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;

    cvbs_detect_init(&cvbs_st);
    while(1)
    {
        rc = cvbs_detect(&cvbs_st);
    
        if (rc== AVIN_CVBS_CLOSE)
            printf("cvbs detect no signal %02d\n", rc);
        else if(rc== AVIN_CVBS_NTSC)
            printf("cvbs detect NTSC signal %02d\n", rc);
        else if(rc== AVIN_CVBS_PAL)
            printf("cvbs detect PAL signal %02d\n", rc);
    

        //return rc;
    }
  return rc;
}

static int sh_cvbs_detect_chrmoalock(int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
	unsigned char org_std ;
	unsigned char org_chromalock;
    int chromalock_times;
	//int times ;

	//times =0;
	chromalock_times = 0;
    cvbs_detect_init(&cvbs_st);
	printf("initial chromalock times %2d\n", chromalock_times );
	
    while(1)
    {
        
        org_chromalock = cvbs_st.chromapll_lockcb;
		org_std = cvbs_st.std_mode;
		
        rc = cvbs_detect(&cvbs_st);
        //usleep(100000);
        if((org_chromalock==0) &&  (cvbs_st.chromapll_lockcb ==1) && (org_std !=AVIN_CVBS_CLOSE) &&(cvbs_st.std_mode !=AVIN_CVBS_CLOSE))
        {
			chromalock_times++;
			//usleep(10000);
			printf("org_chromalock %02d cvbs_st.chromapll_lockcb %2d\n", org_chromalock, cvbs_st.chromapll_lockcb );
			printf("org_std %02d now_std %2d\n", org_std, cvbs_st.std_mode );
			printf("now chromalock times %2d\n", chromalock_times );
        }
		//times++;
		//printf("while times %2d\n", times );
	    #if 0
        if (rc== AVIN_CVBS_CLOSE)
            printf("cvbs detect no signal %02d\n", rc);
        else if(rc== AVIN_CVBS_NTSC)
            printf("cvbs detect NTSC signal %02d\n", rc);
        else if(rc== AVIN_CVBS_PAL)
            printf("cvbs detect PAL signal %02d\n", rc);
        #endif
         
        //return rc;
    }
  return rc;
}
