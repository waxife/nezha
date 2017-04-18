/**
 *  @file   playi80.h
 *  @brief  header file for send stream to i80
 *  $Id: playi80.h,v 1.1 2015/07/15 13:55:26 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2015 Terawins Inc. All rights reserved.
 * 
 *
 */
#ifndef __PLAYI80_H
#define __PLAYI80_H

#include "playfile.h"

int playi80 (const char *filename, struct play_opt_t *opt, callback_fn cmd);
int playi80_get_time (time_t *time);
int playi80_status (struct sysc_status_t *status);

#endif /* __PLAYI80_H */
