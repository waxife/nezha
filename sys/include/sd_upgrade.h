/**
 *  @file   sd_upgrade.h
 *  @brief  header of sd_upgrade.c
 *  $Id: sd_upgrade.h,v 1.6 2015/08/05 10:34:05 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.6 $
 *
 *  Copyright (c) 20014 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/17  lym    New file.
 *
 */
#ifndef SD_UPGRADE_H_
#define SD_UPGRADE_H_

int sd_auto_upgrade (char* time_str);
int sd_auto_upgrade_with_time_stamp (unsigned long time_stamp_mark);

int sd_upgrade (char *filename, unsigned char action);
unsigned long get_sd_upgrade_stamp();

/* return 1 if the time_stamp_mark is equal with the value of RTC SW Information Register.
 * return 0 if the values are not equal. */
char is_sd_upgrade_stamp_equal(unsigned long time_stamp_mark);

int check_upgrade_status();

int sd_upgrade_bin_info (unsigned int *ver, unsigned int *build_date, unsigned int *build_num);

#endif /*SD_UPGRADE_H_*/
