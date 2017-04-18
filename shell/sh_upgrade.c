/**
 *  @file   sh_upgrade.c
 *  @brief  load upgrade code form flash to memory and run it
 *  $Id: sh_upgrade.c,v 1.13 2015/08/05 10:34:39 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.13 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/09/04  hugo    New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <shell.h>
#include <string.h>
#include <sys.h>
#include <serial.h>
#include <watchdog.h>
#include <time.h>
#include <upgrade.h>
#include "sd_upgrade.h"

command_init (sh_upgrade, "upgrade", "upgrade <time_stamp_mark>");
command_init (sh_upgrade2, "upgrade2", "upgrade2");

static int sh_upgrade (int argc, char **argv, void **result, void *sys_ctx)
{	
	if (argc == 2) {
		int stamp = 0;
	    int rc = get_val(argv[1], &stamp);
	    if (rc < 0)
	        goto EXIT;

	    sd_auto_upgrade_with_time_stamp(stamp);
    }
    else {
        goto EXIT;
    }
    
    return 0;

EXIT:
    print_usage (sh_upgrade);
    return -1;
}

static int sh_upgrade2 (int argc, char **argv, void **result, void *sys_ctx)
{	
    unsigned int old_ver, old_build_date, old_build_num;
    unsigned int new_ver, new_build_date, new_build_num;
    unsigned char c = 0;
    int rc = 0;

    rc = sys_firmware_ver(&old_ver, &old_build_date, &old_build_num);
    if (rc < 0) {
        printf("Failed to get current version.\n");
        goto EXIT;
    }

    rc = sd_upgrade_bin_info(&new_ver, &new_build_date, &new_build_num);
    if (rc < 0) {
        printf("Failed to get new firmware version.\n");
        goto EXIT;
    }

    if (new_ver < old_ver || new_build_date < old_build_date || new_build_num <= old_build_num) {
        printf("[WARN] Firmware version\n");
        printf("  - Old : %4X-%08X-%X\n", old_ver, old_build_date, old_build_num);
        printf("  - New : %4X-%08X-%X\n", new_ver, new_build_date, new_build_num);
        printf("upgrade? [y/n]: ");
        while (1) {
            watchdog_touch();
            c = getb2();
            if (c == 'y' || c == 'Y') {
                sd_auto_upgrade_with_time_stamp(time());
            } else if (c == 'n' || c == 'N') {
                break;
            }
        }
    } else {
        sd_auto_upgrade_with_time_stamp(time());
    }

    return 0;

EXIT:
    return -1;
}
