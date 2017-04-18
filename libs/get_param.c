/**
 *  @file   get_param.c
 *  @brief  get parameters in nor code (offset 0x10~0x100)
 *  $Id: get_param.c,v 1.4 2014/01/27 12:26:24 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/17  yc_shih    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <sys.h>
#include <io.h>
#include <unistd.h>
#include <stdlib.h>

/*  State
    0   // find '='
    1   // check param
    2   // get value, and exit
 * */

/* ************ DEPRECATE ************* */
 
int get_param_in_nor(const char *param, const char *nor_str, char *rvalue, int rvlen)
{   
    int ns = 0;         // next start
    //int ce = -1;        // current "=" pos
    int ps = -1;        // param start
    int pe = -1;        // param end
    int vs = -1;        // value start
    int ve = -1;        // value end
    int plen = 0;
    int findeq = 0;
    int findns = 0;
    int diff = 0;
    int state = 0;
    int i = 0;
    int j = 0;
    char* padr = (char*)nor_str;

    plen = strlen(param);

    while(1)
    {
        diff = 0;
        findeq = 0;
        findns = 0;
        if(state == 0)      // find '='
        {
            for(i = ns; i < rvlen; i++)
            {
                if( *(padr+i) == '=')
                {
                    ps = ns;
                    pe = i-1;
                    findeq = 1;
                    state = 1;
                    break;
                }

                if(*(padr+i) == 0)
                {
                    //printf("Can't find %s\n", param);
                    return -1;
                }

            }

            if(i >= rvlen && findeq == 0)
            {
                //printf("Can't find %s\n", param);
                return -1;
            }


        }

        if(state == 1) //ckeck parameter
        {

            for(i = pe; i < rvlen; i++)
            {
                if(*(padr+i) == 0x0a) // '\n'
                {
                    findns = 1;
                    ns = i+1;
                    break;
                }
                
                if(*(padr+i) == 0)
                {
                    //printf("Can't find %s\n", param);
                    return -1;
                }
            }

            if(plen == (pe - ps + 1))
            {
                // compare
                for(i = ps, j = 0; i <= pe; i++, j++)
                {
                    if( (*(padr+i) >= 65 && *(padr+i) <=90))
                    {
                        if( (*(padr+i) != param[j]) && 
                            (*(padr+i) != param[j]-32))
                            diff++;
                    }
                    else if( (*(padr+i) >= 97 && *(padr+i) <=122) ) 
                    {
                        if( (*(padr+i) != param[j]) &&
                            (*(padr+i) != param[j]+32))
                            diff++;
                    }
                    else if(*(padr+i) != param[j])
                        diff++;
                }

                if(diff == 0)//find
                {
                    vs = pe+2;
                    ve = ns-3;
                    state = 2;
                }
                else//can't find
                {
                    state = 0;
                }
            }
            else
            {
                state = 0;
            }
            
            if(i >= rvlen && findns == 0)
            {
                //printf("Can't find %s\n", param);
                return -1;
            }

        }

        if(state == 2)
        {
            memset(rvalue, 0, rvlen);
            if((ve-vs+1) <= rvlen)
                memcpy(rvalue, (char*)(padr+vs), (ve-vs+1));
            else
                memcpy(rvalue, (char*)(padr+vs), rvlen);
            return 0;
        }

    }

    return -1;
}


