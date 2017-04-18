/**
 *  @file   snap_service.h
 *  @brief
 *  $Id: snap_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */


#ifndef SNAP_SERVICE_H_
#define SNAP_SERVICE_H_

struct snapsrv_opt_t {
    int dev;		/* file system */
};

extern void snap_srv (struct snapsrv_opt_t *snapsrv_opt);

#endif /* SNAP_SERVICE_H_ */
