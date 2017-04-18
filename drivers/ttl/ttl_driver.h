/**
 *  @file   ttl_driver.h
 *  @brief	TTL of driver
 *  $Id: ttl_driver.h,v 1.3 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.3 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  kevin 	New file.
 *
 */


#ifndef TTL_INNOLUX_7_DOORPHONE_H_
#define TTL_INNOLUX_7_DOORPHONE_H_

#define VIDEO_STD_NTSC  1
#define VIDEO_STD_PAL   2

#include <iml.h>
void ttl_init_reg_tables(const struct reg* p0,int p0_size_t , const struct reg* p1, int p1_size_t, 
                         const struct reg* p2, int p2_size_t,  const struct reg* p3, int p3_size_t);
void ttl_set_reg_tables(const struct reg* p0,int p0_size_t , const struct reg* p1, int p1_size_t, 
                         const struct reg* p2, int p2_size_t,  const struct reg* p3, int p3_size_t);
void ttl_set_liveview (void);
void ttl_set_playback (void);

#endif /* TTL_INNOLUX_7_DOORPHONE_H_ */
