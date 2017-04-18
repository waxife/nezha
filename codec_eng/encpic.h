/**
 *  @file   encpic.c
 *  @brief  encode picture
 *  $Id: encpic.h,v 1.14 2014/08/04 10:33:23 dos1236 Exp $
 *  $Author: dos1236 $
 *  $Revision: 1.14 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2010/09/09  sherman New file.
 *
 */

#ifndef _ENCPIC_H_
#define _ENCPIC_H_

#include "enc_common.h"

#define ENC_OS_MAGIC    0x14838976
#define MAX_ENC_ATOMS   8

#define JPEG_BUFSIZE 8192
#define JPEG_ATOM_SIZE 8192
#define JPEG2_BUFSIZE 7168
#define JPEG2_ATOM_SIZE 7168
#define MT_SIZE 3240
#define MT_RESERV 8192


#define defualut_qualitry_threshold 10

//motion
#define MD_CTRL_BASE_ADDR                        0xBD400000
#define MD_SYS_REG                              (MD_CTRL_BASE_ADDR + 0x00)
#define MD_CONFIG_REG                           (MD_CTRL_BASE_ADDR + 0x04)
#define MD_STATUS_REG                           (MD_CTRL_BASE_ADDR + 0x08)
#define MD_INT_MASK_REG                         (MD_CTRL_BASE_ADDR + 0x10)
#define MD_INT_STATUS_REG                       (MD_CTRL_BASE_ADDR + 0x14)

#define MD_ENABLE                               1
#define MD_DISABLE                              0

#define FMT_Y                                   (0 << 18)
#define FMT_Cb                                  (1 << 18)
#define FMT_Cr                                  (2 << 18)
#define FMT_YYCbCr                              (3 << 18)

#define INT_FINISH                              1

struct epic_opt_t{
	int width;
	int height;
	int standard;
	int quality;
	int size;
	int dev;
	int field_type;
	int quality_threshold;
	char *user_info;
    int user_info_len;
};

/**
 * @func    recfile
 * @brief   record video file.
 * @param   fname           file name
 *          epic_opt_t       record option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int encpic(char *fname,struct epic_opt_t *opt, cmd_callback_t cmd_callback);
/**
 * @func    recfile
 * @brief   record video file.
 * @param   fname           file name
 *          epic_opt_t       record option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int encpic2(char *fname,struct epic_opt_t *opt, cmd_callback_t cmd_callback);
/**
 * @func    recfile
 * @brief   record video file.
 * @param   fname           file name
 *          epic_opt_t       record option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int encpic3(char *fname, struct epic_opt_t *rec_opt, cmd_callback_t cmd_callback);
/**
 * @func    decode_status
 * @brief   get decode pic status.
 * @param   status          status data structure is declared in sys/include/sys.h
 * @return  0 success, or return negative error code.
 */
int encpic_status(struct sysc_status_t *status);

#endif /* _ENCPIC_H_ */
