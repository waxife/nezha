/**
 *  @file   aumsg_service.h
 *  @brief
 *  $Id: aumsg_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */


#ifndef AUMSG_SERVICE_H_
#define AUMSG_SERVICE_H_

struct aumsgsrv_opt_t {
    int dev;		/* file system */
};

extern void aumsg_srv (struct aumsgsrv_opt_t *aumsgsrv_opt);


#endif /* AUMSG_SERVICE_H_ */
