/**
 *  @file   ttl_innolux_7_doorphone.h
 *  @brief	innolux 7' for door phone demo
 *  $Id: ttl_innolux_7_doorphone.h,v 1.2 2014/01/22 01:45:19 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/11/18  Ken 	New file.
 *
 */


#ifndef TTL_INNOLUX_7_DOORPHONE_H_
#define TTL_INNOLUX_7_DOORPHONE_H_

#define VIDEO_STD_NTSC  1
#define VIDEO_STD_PAL   2

void ttl_init_reg_tables(const struct reg* p0, const struct reg* p1, const struct reg* p2, const struct reg* p3, const struct reg* p4);
void ttl_set_reg_tables(const struct reg* p0, const struct reg* p1, const struct reg* p2, const struct reg* p3, const struct reg* p4);
void ttl_set_liveview (void);
void ttl_set_playback (void);

#endif /* TTL_INNOLUX_7_DOORPHONE_H_ */
