/**
 *  @file   auplay_service.h
 *  @brief
 *  $Id: auplay_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */


#ifndef AUPLAY_SERVICE_H_
#define AUPLAY_SERVICE_H_

struct auplaysrv_opt_t {
    int dev;		/* file system */
    int mark;
};

extern void auplay_srv (struct auplaysrv_opt_t *auplaysrv_opt);


#endif /* AUPLAY_SERVICE_H_ */
