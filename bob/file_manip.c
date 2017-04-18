/**
 *  @file   file_manip.c
 *  @brief	API for manipulation files
 *  $Id: file_manip.c,v 1.2 2014/03/27 07:35:31 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/18  Ken 	New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <watchdog.h>
#define DBG_LEVEL   2
#include <debug.h>
#include <codec.h>
#include <codec_eng/aumsg.h>
#include <norfs.h>
#include <fat32.h>
#include <mconfig.h>
#include <time.h>
#include <codec_eng/decpic.h>
#include "ls.h"

#include "./drivers/mvfile/mvfile.h"
#include "file_manip.h"

/*
 * The format of a file in the file names buffer
 */
static char v_ext_fn[EXT_NAME_LEN + 1] = ".avi";
static char p_ext_fn[EXT_NAME_LEN + 1] = ".jpg";
static char a_ext_fn[EXT_NAME_LEN + 1] = ".wav";
static char fnames[NAME_LENS + 1] = { '\0' };
#ifdef NUMBER_FILE_NAME_TYPE
static char v_pre_fn[PRE_NAME_LEN + 1] = "vid_";
static char p_pre_fn[PRE_NAME_LEN + 1] = "pic_";
static char a_pre_fn[PRE_NAME_LEN + 1] = "aud_";
#endif

/**
 * @func    get_video_fname
 * @brief   get a new filename of video
 * @param   no		number of file name entries
 * 					(don't care when naming by time)
 * @return  point of string buffer (filename)
 */
char* get_video_fname (int no)
{
#ifdef TIME_FILE_NAME_TYPE
	struct tm time;
	char tname[TIME_N_LEN + 1] = { '\0' };
	no = no;
	/* clear filename buffer */
	memset(fnames, '\0', NAME_LENS + 1);
	/* get time by RTC and transform to string */
	rtc_get_time(&time);
	snprintf(tname, TIME_N_LEN, "%04d%02d%02d_%02d%02d%02d",
		time.tm_year + 1970, time.tm_mon + 1, time.tm_mday, 
        time.tm_hour, time.tm_min, time.tm_sec);
	/* combine a new filename */
	snprintf(fnames, NAME_LENS, "%s%s", tname, v_ext_fn);
#else
	/* clear filename buffer */
	memset(fnames, '\0', NAME_LENS + 1);
	/* combine a new filename */
	snprintf(fnames, NAME_LENS, "%s%03d%s", v_pre_fn, no, v_ext_fn);
#endif

	INFO("get a new filename: %s\n", fnames);
	return fnames;
}

/**
 * @func    get_photo_fname
 * @brief   get a new filename of photo
 * @param   no		number of file name entries
 * 					(don't care when naming by time)
 * @return  point of string buffer (filename)
 */
char* get_photo_fname (int no)
{
#ifdef TIME_FILE_NAME_TYPE
	struct tm time;
	char tname[TIME_N_LEN + 1] = { '\0' };
	no = no;
	/* clear filename buffer */
	memset(fnames, '\0', NAME_LENS + 1);
	/* get time by RTC and transform to string */
	rtc_get_time(&time);
	snprintf(tname, TIME_N_LEN, "%04d%02d%02d_%02d%02d%02d",
		time.tm_year+ 1970, time.tm_mon+1, time.tm_mday, 
        time.tm_hour, time.tm_min, time.tm_sec);
	/* combine a new filename */
	snprintf(fnames, NAME_LENS, "%s%s", tname, p_ext_fn);
#else
	/* clear filename buffer */
	memset(fnames, '\0', NAME_LENS + 1);
	/* combine a new filename */
	snprintf(fnames, NAME_LENS, "%s%03d%s", p_pre_fn, no, p_ext_fn);
#endif

	INFO("get a new filename: %s\n", fnames);
	return fnames;
}

/**
 * @func    get_audio_fname
 * @brief   get a new filename of audio
 * @param   no		number of file name entries
 * 					(don't care when naming by time)
 * @return  point of string buffer (filename)
 */
char* get_audio_fname (int no)
{
#ifdef TIME_FILE_NAME_TYPE
	struct tm time;
	char tname[TIME_N_LEN + 1] = { '\0' };
	no = no;
	/* clear filename buffer */
	memset(fnames, '\0', NAME_LENS + 1);
	/* get time by RTC and transform to string */
	rtc_get_time(&time);
	snprintf(tname, TIME_N_LEN, "%04d%02d%02d_%02d%02d%02d",
		time.tm_year + 1970, time.tm_mon + 1, time.tm_mday, 
        time.tm_hour, time.tm_min, time.tm_sec);
	/* combine a new filename */
	snprintf(fnames, NAME_LENS, "%s%s", tname, a_ext_fn);
#else
	/* clear filename buffer */
	memset(fnames, '\0', NAME_LENS + 1);
	/* combine a new filename */
	snprintf(fnames, NAME_LENS, "%s%03d%s", a_pre_fn, no, a_ext_fn);
#endif

	INFO("get a new filename: %s\n", fnames);
	return fnames;
}

#ifdef NUMBER_FILE_NAME_TYPE	// naming by total number of files
/**
 * @func    get_latest_fno
 * @brief   get a latest file of number (filename entries)
 * @param   hls		file list of handle
 * @return  plus for successful, minus if any error occurred
 * @note	this function is find number of name entries in latest file,
 * 			the number must unique and naming by it and can't use total
 * 			numbers of file because issue of delete file.
 */
int get_latest_fno (void* hls)
{
	int rc = 0;
	int nls = 0;
	int nread = 0;
	char *pos;
	int fattrib = 0;
	char *ptr;
	int val = -1;

	/* get number of file in file list */
	rc = gettotalls(hls, &nls, &nread, FATTR_READ_BIT, FATTR_READ_BIT);
	if (nls <= 0) {
		return 0;
	}
	/* move position to tail (NULL file) in file list  */
	rc = pos2taills (hls);
    if (rc < 0) {
        ERROR ("pos2taills(), rc=%d\n", rc);
        return -1;
    }
    /* get latest file */
    rc = getprevls(hls, &pos, &fattrib);
    if (rc < 0) {
    	ERROR("get file list is fail!\n");
    	return -1;
    }

    ptr = strchr ((char*) pos, '_');
	if (ptr == NULL) {
		/* bad format */
		ERROR("File name is bad format!\n");
		return -1;
	}
	ptr++;

	for (; *ptr; ptr++) {
		if (*ptr >= '0' && *ptr <= '9') {
			if(val < 0)
				val = 0;
			val = (val * 10) + (*ptr - '0');
		} else {
			break;
		}
	}

	if(val < 0) {
		ERROR("File name is bad format!\n");
		return -1;
	} else if(val >= MAX_NUM_FILES) {
		WARN("get a number is %d, but the %d maximum number of file!\n",
			val, MAX_NUM_FILES);
		return -1;
	}

	val += 1;

	return val;
}
#endif

/**
 * @func    check_and_fattrib
 * @brief   check the state of individual bits in file attribute
 * @param   fattrib		file of attribute
 * 			mask		bits mask
 * @return  1 if all bits are '1', 0 if one of bits is '0'
 */
int check_and_fattrib (int fattrib, int mask)
{
	int i = 0;
	int cnt = 0;

	for(i = 0; i < FATTR_TOTAL_BITS; i++) {
		if(mask & (1 << i)) {
			if(!(fattrib & (1 << i)))
				return 0;
			else
				cnt++;
		}
	}
	if(cnt)
		return 1;

	return 0;
}

/**
 * @func    check_or_fattrib
 * @brief   check the state of individual bits in file attribute
 * @param   fattrib		file of attribute
 * 			mask		bits mask
 * @return  1 if one of bits is '1', 0 if all bits are '0'
 */
int check_or_fattrib (int fattrib, int mask)
{
	int i = 0;

	for(i = 0; i < FATTR_TOTAL_BITS; i++) {
		if(mask & (1 << i))
			if(fattrib & (1 << i))
				return 1;
	}

	return 0;
}

/**
 * @func    mark_fattrib
 * @brief   mark the state of individual bits in file attribute
 * @param   hls			file list of handle
 * 			fname		filename
 * 			fattrib		file of attribute
 * 			mask		bits mask
 * @return  0 for successful, minus if any error occurred
 * @note	user can mark every bit once but only '0' to '1'
 */
int mark_fattrib (void* hls, char *fname, int fattrib, int mask)
{
	int rc = 0;

	fattrib |= mask;

	rc = setattrls(hls, fname, fattrib);
    if (rc < 0) {
        ERROR ("setattrls(), rc=%d\n", rc);
        return rc;
    }

	return 0;
}

/**
 * @func    get_total_file
 * @brief   scan and get a number of file in file list
 * @param   hls			file list of handle
 * 			nls			total number of file
 * 			unread		number of unread file
 * @return  0 for successful, minus if any error occurred
 */
int get_total_file (void* hls, int* nls, int* unread)
{
	int rc = 0;
	char *pos;
	int fattrib = 0;

	/* get number of file in file list */
	rc = gettotalls(hls, nls, unread, FATTR_READ_BIT, FATTR_READ_BIT);
	if (*nls <= 0) {
		return 0;
	}

	/* move position to tail (NULL file) in file list  */
	rc = pos2taills (hls);
    if (rc < 0) {
        ERROR ("pos2taills(), rc=%d\n", rc);
        return -1;
    }

    /* get latest file */
    rc = getprevls (hls, &pos, &fattrib);
    if (rc < 0) {
    	ERROR ("getprevls(), rc=%d\n", rc);
        return -1;
    }

	return rc;
}

/**
 * @func    get_curr_file
 * @brief   get filename and attribute at the current position of file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
int get_curr_file (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = getcurrls(hls, pos, fattrib);

	return rc;
}

/**
 * @func    get_next_file
 * @brief   get filename and attribute at the next position of file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
int get_next_file (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = getnextls(hls, pos, fattrib);
	if (rc < 0) {
		WARN("in tail now and to head!\n");
		rc = pos2headls (hls);
		if (rc < 0) {
			ERROR ("pos2headls(), rc=%d\n", rc);
			return rc;
		}
		rc = getnextls(hls, pos, fattrib);
		if (rc < 0) {
			return rc;
		}
		rc = 1;	// in head now and file no must clear
	}

	return rc;
}

/**
 * @func    get_prev_file
 * @brief   get filename and attribute at the previous position of file
 * 			in the file list.
 * @param   hls			file list of handle
 * 			pos			string point of filename
 * 			fattrib		point of file attribute
 * @return  0 for successful, minus if any error occurred
 */
int get_prev_file (void* hls, char** pos, int* fattrib)
{
	int rc = 0;

	rc = getprevls(hls, pos, fattrib);
	if (rc < 0) {
		WARN("in head now and to tail!\n");
		rc = pos2taills (hls);
		if (rc < 0) {
			ERROR ("pos2taills(), rc=%d\n", rc);
			return rc;
		}
		rc = getprevls(hls, pos, fattrib);
		if (rc < 0) {
			return rc;
		}
		rc = 1;	// in tail now and file no must clear
	}

	return rc;
}

/**
 * @func    delete_curr_file
 * @brief   delete current of file
 * @param   hls			file list of handle
 * @return  0 for successful, minus if any error occurred
 */
int delete_curr_file (void* hls)
{
	int rc = 0;
	char *pos;
	int fattrib = 0;

	rc = getcurrls(hls, &pos, &fattrib);
	if (rc < 0) {
		ERROR ("getcurrls(), rc=%d\n", rc);
		return rc;
	}

	if(fattrib & FATTR_PROTE_BIT) {
		WARN ("fattrib = 0x%02X!\n", fattrib);
		WARN ("This file is protected, can't delete it!\n");
		return -1;
	}

    rc = deletels (hls);
    if (rc < 0) {
        ERROR ("deletels(), rc=%d\n", rc);
        return rc;
    }

	rc = getcurrls(hls, &pos, &fattrib);
	if (rc < 0) {
		rc = pos2headls (hls);
		if (rc < 0) {
			ERROR ("pos2headls(), rc=%d\n", rc);
			return rc;
		}
		rc = getnextls(hls, &pos, &fattrib);
		if (rc < 0) {
			ERROR ("getnextls(), rc=%d\n", rc);
			return rc;
		}
	}

	return rc;
}

/**
 * @func    scan_media_files
 * @brief   scan and check number of media file in the file list.
 * @param   mdi		media
 * 			fs		file system
 * @return  0 for successful, minus if any error occurred
 */
int scan_media_files (int mdi, int fs, int *nls, int *nread)
{
	int rc = 0;
	HLS *hls = NULL;

	if(mdi >= MEDIA_NUM || mdi < 0) {
		ERROR("unknown of media list!\n");
		return -1;
	}
	if(fs >= FS_NUM || fs < 0) {
		ERROR("unknown of file system!\n");
		return -1;
	}

	if(fs == FATFS) {
		/* check SD card of status */
		rc = chk_sd_status();
		if(rc != SDC_MOUNTED) {
			return rc;
		}
	}

    /* open photo list of NOR file system */
    hls = openls(mdi, fs);
    if (hls == NULL) {
    	ERROR("unknown of file issue!\n");
    	return -1;
    }

    /* get files and unread of total number */
	rc = get_total_file(hls, nls, nread);
	if (rc < 0) {
		ERROR("unknown of file issue!\n");
		return -1;
	}
	if (*nls < 0) {
		ERROR("unknown of file issue!\n");
		return -2;
	} else {
		INFO("media:%d fs:%d\n", mdi, fs);
		INFO("total files: %d\n", *nls);
		INFO("total new files: %d\n", *nread);
	}

	return *nls;
}

/**
 * @func    scan_pic_files
 * @brief   scan and check number of picture file in the file list.
 * @param   fs		file system
 * @return  0 for successful, minus if any error occurred
 */
int scan_pic_files (int fs, int *nls, int *nread)
{
	return scan_media_files(PIC_LS, fs, nls, nread);
}

/**
 * @func    scan_aud_files
 * @brief   scan and check number of audio file in the file list.
 * @param   fs		file system
 * @return  0 for successful, minus if any error occurred
 */
int scan_aud_files (int fs, int *nls, int *nread)
{
	return scan_media_files(AUMSG_LS, fs, nls, nread);
}

/**
 * @func    scan_vid_files
 * @brief   scan and check number of video file in the file list.
 * @param
 * @return  0 for successful, minus if any error occurred
 */
int scan_vid_files (int *nls, int *nread)
{
	return scan_media_files(VIDEO_LS, FATFS, nls, nread);
}


/**
 * @func    sd_detect
 * @brief   check SD card of status and auto mount if unmounted.
 * @param   none
 * @return  2 if mount SD successfully, or 1 if SD is inserted,
 * 			or 0 if no SD
 */
int chk_sd_status (void)
{
    int rc = 0;

    rc = sdc_status();
    switch (rc) {
    case SDC_NONE:
        printf("No SD.\n");
        break;
    case SDC_INSERT:
        printf("SD is inserted.\n");
        break;
    case SDC_MOUNTED:
        printf("SD is mounted.\n");
        break;
    default:
        printf("Unknown SD state.\n");
        break;
    }

    return rc;
}

/**
 * @func    format_sdcared
 * @brief   to format SD card based on media configure.
 * @param   none
 * @return  0 for successful, minus if any error occurred
 */
int format_sdcared (void)
{
	int rc = 0;

//	rc = chk_sd_status();
//	if(rc != SDC_MOUNTED)
//		goto EXIT;

	watchdog_keepalive(60000);

	_umount();
    rc = _format();
    if (rc < 0) {
        ERROR ("_format(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = chk_sd_status();

EXIT:

    return rc;
}

/**
 * @func    chk_bkup_dir
 * @brief   check dir, and create dir if it is not exsit.
 * @param   dir_name		directory 
 * @return  0 for successful, minus if any error occurred
 */
int chk_bkup_dir(char *dir_name)
{
    struct dir dir[1];
    int rc = 0;

    rc = _opendir (dir, dir_name);
    if (rc >= 0) {
        //dbg(0, "rc = %d\n", rc);
        return rc;
    }
    //str2uper(dir_name);
    dbg(0, "make dir %s\n", dir_name);
    rc = _mkdir (dir_name);
    if (rc < 0) {
        ERROR ("_mkdir(), rc=%d\n", rc);
        goto EXIT;
    }
    return 0;  
    
EXIT:
    return rc;  
}

#define COPY_LEN	32
/**
 * @func    pic_backup2sd
 * @brief   specified a picture copy to SD card from NOR file system.
 * @param   media_type		media of type
 * 			fname			filename
 * @return  0 for successful, minus if any error occurred
 */
int pic_backup2sd (int media_type, char *fname)
{
	struct pic_property prop;
    int rc = 0;
    char filename[COPY_LEN + 1] = { '\0' };
#ifdef NUMBER_FILE_NAME_TYPE
    int j = 0;
    struct stat_t st;    
#endif 

    rc = chk_sd_status();
    if(rc != SDC_MOUNTED)
    	return -1;

    memset(&prop, 0, sizeof(struct pic_property));

    rc = chk_bkup_dir("/bkup_pic");

    if (rc < 0){
        return -1;
    }

#ifdef NUMBER_FILE_NAME_TYPE
    memset(filename, 0, sizeof(filename));
    rc = pic_get_info(NORFS, fname, &prop);
    if (rc < 0) {
        ERROR("pic_get_info(), rc = %d\n", rc);
        return -1;
    }
    snprintf(filename, COPY_LEN, "/bkup_pic/%04d%02d%02d_%02d%02d%02d.jpg",
		prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec);

	/* check same filename of file and get latest of file */
	for (j = 1; j < 100; j++) {
		rc = _stat(filename, &st);
		if (rc < 0) {
			if (rc == -E_FILE_NOT_EXIST) {
				break;
			} else {
				dbg(0, "file [%s] stat error - (%d)", filename, rc);
				goto EXIT;
			}
		} else {
			memset(filename, 0, sizeof(filename));
			snprintf(filename, COPY_LEN, "/bkup_pic/%04d%02d%02d_%02d%02d%02d-%02d.jpg",
				prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec, j);
		}
	}

	dbg(0, "file name :%s\n", filename);

	rc = copynor2sd(media_type, fname, filename);
	if (rc < 0) {
		dbg(0, "NOR2SD Error !! (%d)", rc);
	}
#else
	snprintf(filename, COPY_LEN, "/bkup_pic/%s", fname);
	dbg(0, "file name :%s\n", filename);    
	rc = copynor2sd(media_type, fname, filename);
	if (rc < 0) {
		dbg(0, "NOR2SD Error !! (%d)", rc);
		goto EXIT;
	}
#endif
EXIT:

	return rc;
}

/**
 * @func    aumsg_backup2sd
 * @brief   specified a audio massage copy to SD card from NOR file system.
 * @param   media_type		media of type
 * 			fname			filename
 * @return  0 for successful, minus if any error occurred
 */
int aumsg_backup2sd (int media_type, char *fname)
{
    int rc = 0;
    struct aumsg_property prop;
    char filename[COPY_LEN + 1] = { '\0' };
#ifdef NUMBER_FILE_NAME_TYPE
    int j = 0;
    struct stat_t st;
#endif

    rc = chk_sd_status();
    if(rc != SDC_MOUNTED)
    	return -1;

    memset(&prop, 0, sizeof(struct aumsg_property));

    rc = chk_bkup_dir("/bkup_aud");

    if (rc < 0){
        return -1;
    }

#ifdef NUMBER_FILE_NAME_TYPE
    memset(filename, 0, sizeof(filename));
	rc = aumsg_get_info(NORFS, fname, &prop);
	if (rc < 0) {
		ERROR("aumsg_get_info(), rc = %d\n", rc);
		return -1;
	}
	snprintf(filename, "/bkup_aud/%04d%02d%02d_%02d%02d%02d.wav",
		prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec);

	/* check same filename of file and get latest of file */
	for (j = 1; j < 100; j++) {
		rc = _stat(filename, &st);
		if (rc < 0) {
			if (rc == -E_FILE_NOT_EXIST) {
				break;
			} else {
				dbg(0, "file [%s] stat error - (%d)", filename, rc);
				goto EXIT;
			}
		} else {
			memset(filename, 0, sizeof(filename));
			sprintf(filename, "/bkup_aud/%04d%02d%02d_%02d%02d%02d-%02d.wav",
				prop.year, prop.mon, prop.mday, prop.hour, prop.min, prop.sec, j);
		}
	}
	dbg(0, "file name :%s\n", filename);

	rc = copynor2sd(media_type, fname, filename);
	if (rc < 0) {
		dbg(0, "NOR2SD Error !! (%d)", rc);
	}
#else
	snprintf(filename, COPY_LEN, "/bkup_aud/%s", fname);
	dbg(0, "file name :%s\n", filename);
	rc = copynor2sd(media_type, fname, filename);
	if (rc < 0) {
		dbg(0, "NOR2SD Error !! (%d)", rc);
		goto EXIT;
	}
#endif

EXIT:

	return rc;
}
