/**
 *  @file   sh_watchdog.c
 *  @brief  watch dog testing program
 *  $Id: sh_watchdog.c,v 1.4 2014/02/13 11:53:20 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/13  C.N.Yu  New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <shell.h>
#include <mipsregs.h>
#include <sys.h>
#include <serial.h>
#include "sys/include/watchdog.h"

#define DEFAULT_TEST_MODE		0
#define CONFIG_TEST_MODE		1
#define ACCORDING_TEST_MODE		2

command_init (sh_watchdog, "watchdog", "watchdog [minesecond]");

static int sh_watchdog (int argc, char **argv, void **result, void *sys_ctx)
{
	int test_mode = CONFIG_TEST_MODE;
	unsigned long millisec = 5000;
    int c;
    
    if (argc == 2) {
        millisec = atoi(argv[1]);
        test_mode = ACCORDING_TEST_MODE;
        if(millisec < WATCHDOG_MIN_CNT) {
        	printf("The WDT timeout period must more the %ums.\n", WATCHDOG_MIN_CNT);
        	millisec = WATCHDOG_MIN_CNT;
        }
    }

    if(config_watchdog_timeout < WATCHDOG_MIN_CNT) {
    	printf("Do configure the WDT for test.\n");
    	printf("The WDT timeout period is %lums.\n", millisec);
    	watchdog_keepalive(millisec);
    	watchdog_enable();
    	test_mode = DEFAULT_TEST_MODE;
    }

    printf("Start watchdog test\n");
    printf("> Press 's' to stop watchdog alive, 'q' to quit the test.\n");

    while (1) {
    	if(test_mode == CONFIG_TEST_MODE)
    		watchdog_touch();
    	else
    		watchdog_keepalive(millisec);
        if ((c = getb2()) >= 0) {
            printf("'%c'\n", c);
            if (c == 's') {
                int cnt = 0;
                if(test_mode == CONFIG_TEST_MODE)
                	printf("> Reset system after %lu millisecond.\n", config_watchdog_timeout);
                else
                	printf("> Reset system after %lu millisecond.\n", millisec);
                printf("> Press 'k' to keep watchdog alive.\n");
                while ((c = getb2()) != 'k') {
                    sleep(1);
                    cnt++;
                    printf("%d sec. \n", cnt);
                }
            } else if (c == 'q') {
                printf("> Quit watchdog test.\n");
                break;
            }
        }
    }
    if(test_mode != CONFIG_TEST_MODE)
    	watchdog_disable();
    printf("End watchdog test\n");
    
    return 0;
}
