/**
 *  @file   device.h
 *  @brief  device operation
 *  $Id: device.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/03/13  Hugo    New file.
 *
 */
#ifndef __DEVICE_H
#define __DEVICE_H

/* device driver operation API */
typedef struct drive_op {
    int (*init_card)(void *hcard);
    int (*release_card)(void *hcard);
    int (*device_size)(void *hcard);
    int (*read_sector)(void *hcard, int StartSector, int SectorCount, int SectorSize, char *Buf);
    int (*write_sector)(void *hcard, int StartSector, int SectorCount, int SectorSize, char *Buf);
    int (*erase_sector)(void *hcard, int StartSector, int SectorCount, int SectorSize);
} drive_op_t;

#endif /* __DEVICE_H */
