/**
 *  @file   mconfig.c
 *  @brief  Medium Storage Config
 *  $Id: mconfig.c,v 1.23 2014/09/19 03:18:10 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.23 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/27  hugo    New file.
 *
 */
#include <stdio.h>
#include <string.h>
#include <mconfig.h>
#include <norfs.h>
#include <fat32.h>
#include <debug.h>
#include <nor.h>
#include <sys.h>
#include <keymap.h>

#define DIRNAME_LEN 8

typedef struct {
    int magic;
#define MSC_MAGIC       0xbabeface
#define MSC_MAGIC_INIT  0xdadaface
    char dirname[DIRNAME_LEN+1];
    int nfiles;
    unsigned int maxsize;
} MSC;

static MSC _msc[FS_NUM][MEDIA_NUM];
extern unsigned int _medium_start;

static int media_check (FS fs, MEDIA media)
{
    if (fs < 0 || fs >= FS_NUM) {
        ERROR("bad fs\n");
        return -1;
    }

    if (media < 0 || media >= MEDIA_NUM) {
        ERROR("bad media\n");
        return -1;
    }
    
    return 0;
}

static KeyString ksMedia[] = {
    {MEDIA_PIC,     "MEDIA_PIC"},
    {MEDIA_AUMSG,   "MEDIA_AUMSG"},
    {MEDIA_VIDEO,   "MEDIA_VIDEO"},
    {MEDIA_USER0,   "MEDIA_USER0"},
    {MEDIA_USER1,   "MEDIA_USER1"},
};
#define ksMediaNum  (sizeof(ksMedia)/sizeof(ksMedia[0]))

static KeyString ksFS[] = {
    {NORFS,     "NORFS"},
    {FATFS,     "FATFS"},
};

#define ksFSNum  (sizeof(ksFS)/sizeof(ksFS[0]))


char *_media_name (MEDIA media)
{
    return key2str (ksMedia,ksMediaNum, media);
}

char *
_filesystem_name (FS fs)
{
    return key2str (ksFS, ksFSNum, fs);
}

void media_config_show (void)
{
    int i;
    int rc;
    char *name;
    int nfiles, maxsize;
    int realfiles;

   printf("======  Media Config as ========\n");
    for(i = 0; i < MEDIA_NUM; i++) {
        rc = media_get_config(FATFS, i, &name, &nfiles, &maxsize);
        if (rc < 0)
            continue;
        if (nfiles <  0) {
            rc = media_get_realconfig(FATFS, i, &name, &realfiles, &maxsize);
            printf("mconfig(%d,%d) %s %-11.11s '%s' files=%d (%d) maxsize=%d\n", FATFS, i, 
                   _filesystem_name(FATFS), _media_name (i), name, nfiles, realfiles, 
                   maxsize);
        } else {
            printf("mconfig(%d,%d) %s %-11.11s '%s' files=%d maxsize=%d\n", FATFS, i, 
                   _filesystem_name(FATFS), _media_name (i), name, nfiles, maxsize);
        }
    }   

    for (i = 0; i < MEDIA_NUM; i++) {
        rc = media_get_config(NORFS, i, &name, &nfiles, &maxsize);
        if (rc < 0)
            continue;
        if (nfiles < 0) {
            rc = media_get_realconfig(NORFS, i, &name, &realfiles, &maxsize);
            printf("mconfig(%d,%d) %s %-11.11s '%s' files=%d (%d) maxsize=%d\n", NORFS, i, 
                    _filesystem_name(NORFS), _media_name (i), name, nfiles, realfiles, 
                    maxsize);
        } else {
            printf("mconfig(%d,%d) %s %-11.11s '%s' files=%d maxsize=%d\n", NORFS, i, 
                    _filesystem_name(NORFS), _media_name (i), name, nfiles, maxsize);
        }
    }
    printf("===============================\n");
}

int media_config_reset()
{
    extern int _norfs_reset_partition(void);
    memset(_msc, 0, sizeof(_msc));
    _norfs_reset_partition();
    return 0;
}

int media_config_done(void)
{
    int blk_start = (unsigned int) &_medium_start/NOR_BLKSIZ;
    int blk_end   = gSYSC.pnor->last_ba;
    int i, j, rc;
    int maxsize;
    int nfiles;
    int pads = 0;

    /* set partition for NORFS */ 
    for (i = 0; i < 2; i++) {
        for(j = 0; j < MEDIA_NUM; j++) {
            MSC *msc = &_msc[NORFS][j];
            if (msc->magic != MSC_MAGIC_INIT)
                continue;
                
            if (i == 0 && msc->nfiles < 0)
                continue;
            else if (i == 1 && msc->nfiles >= 0)
                continue;
                
            msc->magic = MSC_MAGIC;    
            struct norfs_partition p[1];
            p->start = blk_start;
            
            maxsize = msc->maxsize;
            nfiles = msc->nfiles;
            if (msc->nfiles < 0) { 
                nfiles = ((blk_end - blk_start) * NOR_BLKSIZ) / maxsize;
                if (msc->nfiles < -1 && nfiles > -msc->nfiles) {
                    nfiles = -msc->nfiles;
                }
            }

            p->count = (nfiles * maxsize) / NOR_BLKSIZ;

            if (nfiles > NOR_FILE_NUM) {
                ERROR("%d Over maximum file number %d\n", nfiles, NOR_FILE_NUM);
                rc = -1;
                goto EXIT;
            }

            if (p->start + p->count > blk_end) {
                ERROR("No enough space for config f %d s %d\n", msc->nfiles, msc->maxsize);
                rc = -1;
                goto EXIT;
            }
            
            p->file_num = nfiles;
            p->file_len = maxsize;
            rc = norfs_set_partition (j, p);
            if (rc < 0) {
                ERROR ("norfs_set_partition(), rc=%d\n", rc);
                goto EXIT;
            }

            blk_start += rc;
        }
    }    
    
    for(j = 0; j < MEDIA_NUM; j++) {
        MSC *msc = &_msc[FATFS][j];
        if (msc->magic != MSC_MAGIC_INIT)
            continue;
            
        if (msc->nfiles < 0) 
            pads++;   
        
        msc->magic = MSC_MAGIC;    
    }
    
    if (pads > 1) {
        ERROR("too many nfiles < 0 config for FATFS\n");
        rc = -1;
        goto EXIT;
    }
    
    return 0;
EXIT:
    media_config_reset();

    return rc;
}


int media_config (FS fs, MEDIA media, char *dirname, int nfiles, int maxsize)
{
    MSC *msc;
    //int rc;

    if (media_check (fs, media) < 0)
        return -1;
    
    if (maxsize % (64*1024) != 0 || maxsize <= 0) {
        ERROR("invalid maxsize %d\n", maxsize);
        return -1;
    }
    
    if (strlen(dirname) > DIRNAME_LEN) {
        ERROR("invlid dirname size\n");
        return -1;
    }

    msc = &_msc[fs][media];
    if (msc->magic == MSC_MAGIC || msc->magic == MSC_MAGIC_INIT) {
        ERROR("invalid sequence to invoke media_config\n");
        return -1;
    }

    if (nfiles < -MCONFIG_MAX_NFILES) {        
        ERROR("invalid nfiles %d\n", nfiles);
        return -1;    
    }

    msc->nfiles = nfiles;
    msc->maxsize = maxsize;

    strcpy (msc->dirname, str2lower (dirname));
    msc->magic = MSC_MAGIC_INIT;
    //media_config_show ();
    return 0;
}

char *media_dirname (FS fs, MEDIA media)
{
    if (media_check (fs, media) < 0)
        return NULL;

    if (_msc[fs][media].magic != MSC_MAGIC)
        return NULL;

    return _msc[fs][media].dirname;
}

int media_partition (FS fs, MEDIA media, int *start, int *count)
{
    struct norfs_partition p[1];
    int rc;

    if (media_check (fs, media) < 0)
        return -1;

    if (_msc[fs][media].magic != MSC_MAGIC)
        return -1;

    if (fs != NORFS)
        return -1;

    rc = norfs_get_partition (media, p);
    if (rc < 0) {
        ERROR ("norfs_get_partition(), rc=%d\n", rc);
        return rc;
    }

    *start = p->start;
    *count = p->count;

    return 0;
}

int media_get_config(FS fs, MEDIA media, char **pname, int *pnfiles, int *pmaxsize)
{
    if (media_check(fs, media) < 0)
        return -1;
    if (_msc[fs][media].magic != MSC_MAGIC)
    	return -1;

    *pname = _msc[fs][media].dirname;
    *pnfiles = _msc[fs][media].nfiles;
    *pmaxsize = _msc[fs][media].maxsize;
	
    return 0;
    	
}

int media_get_realconfig(FS fs, MEDIA media, char **pname, int *pnfiles, int *pmaxsize)
{
    int nfiles;
    int nsize;
    int rc;
    struct norfs_partition p[1];

    if (media_check(fs, media) < 0)
        return -1;
    if (_msc[fs][media].magic != MSC_MAGIC)
    	return -1;

    nfiles = _msc[fs][media].nfiles;
    if (nfiles < 0) {
        if (fs == NORFS) {
            rc = norfs_get_partition (media, p);
            if (rc < 0) {
                ERROR ("norfs_get_partition(), rc=%d\n", rc);
                return rc;
            }
            nfiles = p->file_num;
        } else if (fs == FATFS) {
            if (_ismounted()) {
                rc = _fatfs_get_partition(media, &nfiles, &nsize);
                if (rc < 0) {
                    ERROR("_fatfs_get_partition(), rc=%d\n", rc);
                    return rc;
                }
            }
        }
    }
    *pname = _msc[fs][media].dirname;
    *pnfiles = nfiles;    
    *pmaxsize = _msc[fs][media].maxsize;

    return 0;
}

