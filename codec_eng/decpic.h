/**
 *  @file   decpic.h
 *  @brief  decode picture
 *  $Id: decpic.h,v 1.21 2016/07/27 09:11:07 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.21 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2010/09/09  sherman New file.
 *
 */

#ifndef _DECPIC_H_
#define _DECPIC_H_

#include "enc_common.h"

#define DEC_OS_MAGIC        0x14838976
#define MAX_DEC_ATOMS   8
#define JPEG_BUFSIZE 8192
#define JPEG_ATOM_SIZE 8192

struct dec_os_t {
    uint32_t        magic;
    int             img_w;
    int             img_h;
    int             img_h_real;
    int             frm_cnt;
    struct atom_t   atoms[MAX_DEC_ATOMS];
    uint8_t         v_shut_idx;
    uint8_t         junk[3];
};

struct img_info_t {
    int width;
    int height;
    int length;
    int quality;
    time_t tm;
    char user_info[32];
    int user_info_len;
};


struct dpic_opt_t{
	int dev;
	int effect;
	int duration;
	int threshold;
    int fields;     /* field option (debug only)
                        0: depend on app0_info.f,
                        1: odd field
                        2: odd & even fields */
    int data_length; //For ram jpeg decoder
};

struct pic_info_t {
    int width;
    int height;
    int length;
    int quality;
    time_t tm;
    char user_info[32];
    int user_info_len;
};

struct pic_property {
    int width;
    int height;
    time_t time;

    /* image time field */
    int year;
    int mon;
    int mday;
    int hour;
    int min;
    int sec;

    /* user infomation buffer */
    char user_info[32];
    int user_info_len;
	int length;
};

/* The following return value is defined for copy image from NOR to SD function */
enum IMG_NOR2SD_RC {
    IMG_NOR2SD_INV_IDX = 501,
    IMG_NOR2SD_NO_IMG,
    IMG_NOR2SD_ERR_IMG,
};

/**
 * @func    decpic
 * @brief   decode picture
 * @param   fname           file name
 *          dpic_opt_t      the option of the picture decode
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int decpic(char *fname,struct dpic_opt_t *opt, cmd_callback_t cmd_callback);
/**
 * @func    decpic
 * @brief   decode picture
 * @param   fname           file name
 *          dpic_opt_t      the option of the picture decode
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int decpic2(char *fname,struct dpic_opt_t *opt, cmd_callback_t cmd_callback);
/**
 * @func    decode_status
 * @brief   get decode pic status.
 * @param   status          status data structure is declared in sys/include/sys.h
 * @return  0 success, or return negative error code.
 */
int decpic_status(struct sysc_status_t *status);

/**
 * @func    pic_get_info
 * @brief   get decode pic information.
 * @param   fs : filesystem fname: file name pic_property = the structure of the picture information
 * @return  0 success, or return negative error code.
 */
int pic_get_info(int fs,char *fname,struct pic_property *property);
/**
 * @func    pic_get_info
 * @brief   get decode pic information.
 * @param   fs : filesystem fname: file name pic_property = the structure of the picture information
 * @return  0 success, or return negative error code.
 */
int decpic2_pic_property(int fs,char *fname,struct pic_property *property);

/**
 * @func    pic_nor2sd
 * @brief   pic form norflahs to sdcard
 * @param   src_name ->sourcde name in norflash  dst_name ->destination name in sdcard
 * @return  0 success, or return negative error code.
 */
int pic_nor2sd(char *src_name,char *dst_name);

int pic_sd2nor (char *srcFile, char *dstFile);


/* decpic3 API*/
int decpic3 (char *fname, struct dpic_opt_t *opt, cmd_callback_t cmd_callback);
int decpic3_pic_property (int fs, char *fname, struct pic_property *property);

/* decpic2 decode jpeg in ram*/
int decpic2_from_ram(unsigned char *dbuf,struct dpic_opt_t *opt, cmd_callback_t cmd_callback);

/* decpic2 decode two jpegs in 64K ram*/
int decpic2_from_ram_video(unsigned char *dbuf,struct dpic_opt_t *opt, cmd_callback_t cmd_callback);


#endif /* _DECPIC_H_ */
