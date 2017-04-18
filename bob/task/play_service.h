/**
 *  @file   play_service.h
 *  @brief
 *  $Id: play_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/17  Ken 	New file.
 *
 */


#ifndef PLAY_SERVICE_H_
#define PLAY_SERVICE_H_

struct playsrv_opt_t {
    int dev;		/* file system */
    int mark;
};

extern void play_srv (struct playsrv_opt_t *playsrv_opt);


#endif /* PLAY_SERVICE_H_ */
