/**
 *  @file   file_manip.h
 *  @brief	API for manipulation files
 *  $Id: file_manip.h,v 1.1 2014/03/14 12:11:36 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  Ken 	New file.
 *
 */


#ifndef FILE_MANIPULATE_H_
#define FILE_MANIPULATE_H_

#define TIME_FILE_NAME_TYPE		// naming by time
//#define NUMBER_FILE_NAME_TYPE		// naming by total number of files

#define MAX_NUM_FILES		999

#define EXT_NAME_LEN        4
#define TIME_N_LEN			(4+2+2+1+2+2+2)	// 15
/* ABCDEFGH_IJKLMN.abc */
#define TIME_NAME_LEN       (TIME_N_LEN + EXT_NAME_LEN)

#define NUMBER_LEN			3
#define PRE_NAME_LEN        4
/* vid_xxx.jpg */ /* pic_xxx.jpg */ /* aud_xxx.wav */
#define NO_NAME_LEN			(PRE_NAME_LEN + NUMBER_LEN + EXT_NAME_LEN)

#ifdef TIME_FILE_NAME_TYPE
#define NAME_LENS	TIME_NAME_LEN
#else
#define NAME_LENS	NO_NAME_LEN
#endif

/**
 * user defined of file attribute, total is 8bits
 * NOTE: every bit of default value is "0",
 * only set to a binary "1" value once (OTP)
 */
#define FATTR_UNUSE0_BIT		(1 << 0)
#define FATTR_UNUSE1_BIT		(1 << 1)
#define FATTR_UNUSE2_BIT		(1 << 2)
#define FATTR_UNUSE3_BIT		(1 << 3)
#define FATTR_UNUSE4_BIT		(1 << 4)
#define FATTR_UNUSE5_BIT		(1 << 5)
#define FATTR_PROTE_BIT			(1 << 6)	/* protected */
#define FATTR_READ_BIT			(1 << 7)	/* read */
#define FATTR_TOTAL_BITS			  8


extern int check_and_fattrib (int fattrib, int mask);
extern int check_or_fattrib (int fattrib, int mask);
extern int mark_fattrib (void* hls, char *fname, int fattrib, int mask);

extern char* get_video_fname (int no);
extern char* get_photo_fname (int no);
extern char* get_audio_fname (int no);

extern int get_latest_fno (void* hls);
extern int check_unread (int fattrib);
extern int get_total_file (void* hls, int* nls, int* unread);
extern int get_curr_file (void* hls, char** pos, int* fattrib);
extern int get_next_file (void* hls, char** pos, int* fattrib);
extern int get_prev_file (void* hls, char** pos, int* fattrib);
extern int delete_curr_file (void* hls);

extern int chk_sd_status (void);
extern int format_sdcared (void);
extern int pic_backup2sd (int media_type, char *fname);
extern int aumsg_backup2sd (int media_type, char *fname);

/**
 * @func    scan_media_files
 * @brief   scan and check number of media file in the file list.
 * @param   mdi		media
 * 			fs		file system
 * @return  0 for successful, minus if any error occurred
 */
extern int scan_media_files (int mdi, int fs, int *nls, int *nread);
/**
 * @func    scan_pic_files
 * @brief   scan and check number of picture file in the file list.
 * @param   fs		file system
 * @return  0 for successful, minus if any error occurred
 */
extern int scan_pic_files (int fs, int *nls, int *nread);
/**
 * @func    scan_aud_files
 * @brief   scan and check number of audio file in the file list.
 * @param   fs		file system
 * @return  0 for successful, minus if any error occurred
 */
extern int scan_aud_files (int fs, int *nls, int *nread);
/**
 * @func    scan_vid_files
 * @brief   scan and check number of video file in the file list.
 * @param
 * @return  0 for successful, minus if any error occurred
 */
extern int scan_vid_files (int *nls, int *nread);

#endif /* FILE_MANIPULATE_H_ */
