/**
 *  @file   sh_get_param.c
 *  @brief  get parameters in nor code (offset 0x10~0x100)
 *  $Id: sh_get_param.c,v 1.1.1.1 2013/12/18 03:43:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/13  yc    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <unistd.h>
#include <stdlib.h>

#define NOR_PARAM       0xa0000010
#define NOR_PARAM_SOFF  0x10
#define NOR_PARAM_EOFF  0x100
#define PARAM_RANGE     (NOR_PARAM_EOFF - NOR_PARAM_SOFF)
command_init(sh_get_param, "get_param", "get parameters in Nor code");

/*  State
    0   // find '='
    1   // check param
    2   // get value, and exit
 * */

int sh_get_param(int argc, char **argv, void **result, void *sys_ctx)
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
    char* padr = (char*)NOR_PARAM;
    char  pvalue[64];

    if(argc != 2)
        goto EXIT;

    plen = strlen(argv[1]);

    //printf("dbg %s, len = %d\n", argv[1], plen);

    while(1)
    {
        diff = 0;
        findeq = 0;
        findns = 0;
        if(state == 0) // find =
        {
            //printf("dbg state = 0\n");
            for(i = ns; i < PARAM_RANGE; i++)
            {
                //printf("dbg [%d] = %c\n", i, *(padr+i));
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
                    printf("Can't find %s\n", argv[1]);
                    return 0;
                }

            }

            if(i >= PARAM_RANGE && findeq == 0)
            {
                printf("Can't find %s\n", argv[1]);
                return 0;
            }


        }

        if(state == 1) //ckeck parameter
        {

            //printf("dbg state = 1, pe = %d\n", pe);
            for(i = pe; i < PARAM_RANGE; i++)
            {
                if(*(padr+i) == 0x0a) // '\n'
                {
                    findns = 1;
                    ns = i+1;
                    break;
                }
                
                if(*(padr+i) == 0)
                {
                    printf("Can't find %s\n", argv[1]);
                    return 0;
                }
            }

            if(plen == (pe - ps + 1))
            {
                // compare
                for(i = ps, j = 0; i <= pe; i++, j++)
                {
                    if( (*(padr+i) >= 65 && *(padr+i) <=90))
                    {
                        if( (*(padr+i) != argv[1][j]) && 
                            (*(padr+i) != argv[1][j]-32))
                            diff++;
                    }
                    else if( (*(padr+i) >= 97 && *(padr+i) <=122) ) 
                    {
                        if( (*(padr+i) != argv[1][j]) &&
                            (*(padr+i) != argv[1][j]+32))
                            diff++;
                    }
                    else if(*(padr+i) != argv[1][j])
                        diff++;
                }

                if(diff == 0)//find value, set value start, value end
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
            
            if(i >= PARAM_RANGE && findns == 0)
            {
                printf("Can't find %s\n", argv[1]);
                return 0;
            }

        }

        if(state == 2)
        {
            //printf("dbg state = 2\n");
            memset(pvalue, 0, 64);
            if((ve-vs+1) <= 64)
                memcpy(pvalue, (char*)(padr+vs), (ve-vs+1));
            else
                memcpy(pvalue, (char*)(padr+vs), 64);
            printf("%s: %s\n", argv[1], pvalue);
            return 0;
        }

    }

    printf("Get Parameters\n");
    printf("%s", (char*) NOR_PARAM);

    return 0;

EXIT:

    print_usage(sh_get_param);
    return -1;
}


