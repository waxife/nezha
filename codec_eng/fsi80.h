/**
 *  @file   fsi80.h
 *  @brief
 *  $Id: fsi80.h,v 1.1 2015/07/17 08:19:55 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2015/6/30  Ken 	New file.
 *
 */


#ifndef FSI80_H_
#define FSI80_H_

#include <sys.h>

struct fsi80_opt_t {
	int dev;		/* file system */
	int med;		/* media */
	uint32_t	filesize;
	uint32_t	blksize;
};

int enci80_init (void);
uint32_t fsi80_get_filecrc(void);
/**
 * @func    fsi80
 * @brief   send file list used i80 interface .
 * @param   path			file path
 * 			fs_opt_t       	fs option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int fsi80(const char *path, struct fsi80_opt_t *fs_opt, int (*cmd_callback)(void **arg));

#endif /* FSI80_H_ */
