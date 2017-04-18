/**
 *  @file   lsf_service.h
 *  @brief
 *  $Id: lsf_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/3/13  Ken 	New file.
 *
 */


#ifndef LSF_SERVICE_H_
#define LSF_SERVICE_H_

struct lsfsrv_opt_t {
    int media;		/* media */
	int dev;		/* file system */
	int mark;
};

extern void lsf_srv (struct lsfsrv_opt_t *lsfsrv_opt);

#endif /* LSF_SERVICE_H_ */
