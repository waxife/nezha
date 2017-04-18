/**
 *  @file   strings.h
 *  @brief  POSIX like standard string header
 *  $Id: strings.h,v 1.1 2014/06/17 03:11:53 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */
 
#ifndef __STRINGS_H__
#define __STRINGS_H__

#define bcopy(src, dst, n)  memcpy(dst, src, n)
#define bzero(src, n)       memset(src, 0, n) 

#endif /* __STRINGS_H__ */
