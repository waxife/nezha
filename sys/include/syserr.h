/**
 *  @file   syserr.h
 *  @brief  errno define 
 *  $Id: syserr.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/03/05  jedy  New file.
 *
 */

#ifndef __SYSERR_H
#define __SYSERR_H
#include <errno.h>

/* all syserr should >= 2000 */

/* error for jftl and nand flash driver */
#define ENAND           (2000)
#define ENAND_PROGRAM   (ENAND+0)
#define ENAND_ERASE     (ENAND+1)
#define ENAND_ECC       (ENAND+2)


#endif /* __SYSERR_H */
