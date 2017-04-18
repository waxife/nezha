/**
 *  @file   photo.h
 *  @brief  header file of photo encode/decode driver
 *  $Id: photo.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/10/02  hugo     New file.
 *
 */
#ifndef __PHOTO_H
#define __PHOTO_H

#define PH_MODE_UNKNOWN     0
#define PH_MODE_WRONLY      1
#define PH_MODE_RDONLY      2

struct photo_ctx {
};

/******************************************************************************
 * Photo Driver API
 ******************************************************************************/

int photo_open (struct photo_ctx *ph, int mode);
int photo_close (struct photo_ctx *ph);
int photo_read_nonblock (struct photo_ctx *ph, void *buffer, int length);
int photo_read (struct photo_ctx *ph, void *buffer, int length);
int photo_write_nonblock (struct photo_ctx *ph, void *buffer, int length);
int photo_write (struct photo_ctx *ph, void *buffer, int length);
int photo_ioctl (struct photo_ctx *ph, int request, void *arg);

#endif /* __PHOTO_H */
