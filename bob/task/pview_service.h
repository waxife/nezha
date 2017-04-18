/**
 *  @file   pview_service.h
 *  @brief
 *  $Id: pview_service.h,v 1.1 2014/03/14 12:14:23 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */


#ifndef PVIEW_SERVICE_H_
#define PVIEW_SERVICE_H_

struct pviewsrv_opt_t {
    int dev;		/* file system */
    int mark;
};

extern void pview_srv (struct pviewsrv_opt_t *pviewsrv_opt);

#endif /* PVIEW_SERVICE_H_ */
