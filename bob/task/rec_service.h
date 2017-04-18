/**
 *  @file   rec_service.h
 *  @brief
 *  $Id: rec_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  Ken 	New file.
 *
 */


#ifndef REC_SERVICE_H_
#define REC_SERVICE_H_

struct recsrv_opt_t {
    int dev;		/* file system */
};

extern void rec_srv (struct recsrv_opt_t *recsrv_opt);

#endif /* REC_SERVICE_H_ */
