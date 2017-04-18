/**
 *  @file   ls.c
 *  @brief  file manipulate
 *  $Id: ls.c,v 1.50 2014/08/14 03:34:25 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.50 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/27  hugo    New file.
 *
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <ls.h>
#include <fs.h>
#include <norfs.h>
#include <fat32.h>

static HLS _hls[FS_NUM][MEDIA_NUM];

/*****************************************************************************
 * NOR filesystem
 *****************************************************************************/

static int nor_gettotalls (HLS *hls, int *nls, int *npick, int filter, int mask)
{
    struct norfs_dir dir[1];
    int rc;

    rc = norfs_opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        return -1;
    }

    rc = norfs_gettotal (dir, nls, npick, filter, mask);
    if (rc < 0) {
        ERROR ("norfs_gettotal(), rc=%d\n", rc);
        return -1;
    }

    norfs_closedir (dir);
    return 0;
}

static int nor_getnextls (HLS *hls, char **pos, int *attribute)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    int next;
    int rc;

    if (hls->current == TAIL_NULL) {
        return -1;
    }

    rc = norfs_opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        return -1;
    }

    /* read current */
    if (hls->current == HEAD_NULL) {
        next = dir->head;
    } else {
        rc = norfs_getdirent (dir, dirent, hls->current);
        if (rc < 0) {
            ERROR ("norfs_getdirent(), rc=%d\n", rc);
            return rc;
        }
        next = dirent->next;
    }

    /* move forward */
    hls->current = next;
    if ((hls->current & LINK_NULL) == LINK_NULL) {
        hls->current = TAIL_NULL;
        return -1;
    }
    rc = norfs_getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("norfs_getdirent(), rc=%d\n", rc);
        return rc;
    }

    *pos = strncpy (hls->filename, dirent->name, NAMELEN);
    *attribute = dirent->attribute;

    return 0;
}

static int nor_getprevls (HLS *hls, char **pos, int *attribute)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    int prev;
    int rc;

    if (hls->current == HEAD_NULL) {
        return -1;
    }

    rc = norfs_opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        return -1;
    }

    /* read current */
    if (hls->current == TAIL_NULL) {
        prev = dir->tail;
    } else {
        rc = norfs_getdirent (dir, dirent, hls->current);
        if (rc < 0) {
            ERROR ("norfs_getdirent(), rc=%d\n", rc);
            return rc;
        }
        prev = dirent->prev;
    }

    /* move forward */
    hls->current = prev;
    if ((hls->current & LINK_NULL) == LINK_NULL) {
        hls->current = HEAD_NULL;
        return -1;
    }
    rc = norfs_getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("norfs_getdirent(), rc=%d\n", rc);
        return rc;
    }

    *pos = strncpy (hls->filename, dirent->name, NAMELEN);
    *attribute = dirent->attribute;

    return 0;
}

static int nor_getcurrls (HLS *hls, char **pos, int *attribute)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    int rc;

    if ((hls->current & LINK_NULL) == LINK_NULL) {
        WARN ("LINK_NULL\n");
        return -1;
    }

    rc = norfs_opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        return -1;
    }

    /* read current */
    rc = norfs_getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("norfs_getdirent(), rc=%d\n", rc);
        return rc;
    }

    *pos = *pos = strncpy (hls->filename, dirent->name, NAMELEN);
    *attribute = dirent->attribute;

    return 0;

    return 0;
}

static int nor_deletels (HLS *hls)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    char name[NAMELEN+1];
    int rc;

    if ((hls->current & LINK_NULL) == LINK_NULL) {
        WARN ("LINK_NULL\n");
        return -1;
    }

    rc = norfs_opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        return -1;
    }

    rc = norfs_getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("norfs_getdirent(), rc=%d\n", rc);
        return rc;
    }

    if ((dirent->next & LINK_NULL) == LINK_NULL) {
        hls->current = TAIL_NULL;
    } else {
        hls->current = dirent->next;
    }

    snprintf (name, NAMELEN, "%s/%s", hls->dirname, dirent->name);
    return norfs_remove (name);
}

static int nor_getnewls (HLS *hls, char **pos, const char *fname, int victim, unsigned char filter, unsigned char alter)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    int candidate;
    int rc;

    *pos = (char *)fname;   /* assign anyway */

    rc = norfs_opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("norfs_opendir(), rc=%d\n", rc);
        return -1;
    }

    if (dir->total < hls->nfiles)
        return 0;

    if (!victim) {
        WARN ("no victim\n");
        return -1;
    }

    candidate = dir->head;
    while ((candidate & LINK_NULL) != LINK_NULL) {
        rc = norfs_getdirent (dir, dirent, candidate);
        if (rc < 0) {
            ERROR ("norfs_getdirent(), rc=%d\n", rc);
            return rc;
        }
        if (((dirent->attribute ^ alter) & (filter|alter)) == 0) {
            char name[40];
            sprintf (name, "%s/%s", hls->dirname, dirent->name);
            INFO ("remove(%s)\n", name);
            rc = norfs_remove (name);
            if (rc < 0) {
                ERROR ("norfs_remove(), rc=%d\n", rc);
                return -1;
            }
            return 0;
        }
        candidate = dirent->next;
    }

    /* not found */
    return -1;
}

static int nor_setattrls (HLS *hls, char *pos, int attribute)
{
    char name[NAMELEN+1];

    snprintf (name, NAMELEN, "%s/%s", hls->dirname, pos);
    return norfs_setattr (name, attribute);
}

/*****************************************************************************
 * FAT filesystem
 *****************************************************************************/

static int fat_gettotalls (HLS *hls, int *nls, int *npick, int filter, int mask)
{
    struct dir dir[1];
    int rc;

    rc = _opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return -1;
    }

    rc = _gettotal (dir, nls, npick, filter, mask);
    if (rc < 0) {
        ERROR ("_gettotal(), rc=%d\n", rc);
        return -1;
    }

    _closedir (dir);
    return 0;
}

static int fat_getnextls (HLS *hls, char **pos, int *attribute)
{
    struct dir dir[1];
    struct dirent dirent[1];
    int next;
    int rc;

    if (hls->current == TAIL_NULL) {
        return -1;
    }

    rc = _opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return -1;
    }

    /* read current */
    if (hls->current == HEAD_NULL) {
        next = dir->head;
    } else {
        rc = _getdirent (dir, dirent, hls->current);
        if (rc < 0) {
            ERROR ("_getdirent(), rc=%d\n", rc);
            return rc;
        }
        next = dirent->dire_next;
    }

    /* move forward */
    hls->current = next;
    if ((hls->current & LINK_NULL) == LINK_NULL) {
        hls->current = TAIL_NULL;
        return -1;
    }
    rc = _getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("_getdirent(), rc=%d\n", rc);
        return rc;
    }
    if (dirent->Attrib != ATTRIB_ARCHIVE) {
        ERROR ("bad file attribute\n");
        return -E_FILE_BAD_ATTRIB;
    }

    *pos = strncpy (hls->filename, dirent->Name, NAMELEN);
    *attribute = dirent->Attribute;

    return 0;
}

static int fat_getprevls (HLS *hls, char **pos, int *attribute)
{
    struct dir dir[1];
    struct dirent dirent[1];
    int prev;
    int rc;

    if (hls->current == HEAD_NULL) {
        return -1;
    }

    rc = _opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return -1;
    }

    /* read current */
    if (hls->current == TAIL_NULL) {
        prev = dir->tail;
    } else {
        rc = _getdirent (dir, dirent, hls->current);
        if (rc < 0) {
            ERROR ("_getdirent(), rc=%d\n", rc);
            return rc;
        }
        prev = dirent->dire_prev;
    }

    /* move forward */
    hls->current = prev;
    if ((hls->current & LINK_NULL) == LINK_NULL) {
        hls->current = HEAD_NULL;
        return -1;
    }
    rc = _getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("_getdirent(), rc=%d\n", rc);
        return rc;
    }
    if (dirent->Attrib != ATTRIB_ARCHIVE) {
        ERROR ("bad file attribute\n");
        return -E_FILE_BAD_ATTRIB;
    }

    *pos = strncpy (hls->filename, dirent->Name, NAMELEN);
    *attribute = dirent->Attribute;
    return 0;
}

static int fat_getcurrls (HLS *hls, char **pos, int *attribute)
{
    struct dir dir[1];
    struct dirent dirent[1];
    int rc;

    if ((hls->current & LINK_NULL) == LINK_NULL) {
        WARN ("LINK_NULL\n");
        return -1;
    }

    rc = _opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return -1;
    }

    /* read current */
    rc = _getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("_getdirent(), rc=%d\n", rc);
        return rc;
    }
    if (dirent->Attrib != ATTRIB_ARCHIVE) {
        ERROR ("bad file attribute\n");
        return -E_FILE_BAD_ATTRIB;
    }

    *pos = *pos = strncpy (hls->filename, dirent->Name, NAMELEN);
    *attribute = dirent->Attribute;

    return 0;
}

static int fat_deletels (HLS *hls)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char name[NAMELEN+1];
    int rc;

    if ((hls->current & LINK_NULL) == LINK_NULL) {
        WARN ("LINK_NULL\n");
        return -1;
    }

    rc = _opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return -1;
    }

    rc = _getdirent (dir, dirent, hls->current);
    if (rc < 0) {
        ERROR ("_getdirent(), rc=%d\n", rc);
        return rc;
    }

    if ((dirent->dire_next & LINK_NULL) == LINK_NULL) {
        hls->current = TAIL_NULL;
    } else {
        hls->current = dirent->dire_next;
    }

    snprintf (name, NAMELEN, "%s/%s", hls->dirname, dirent->Name);
    return _remove (name);
}

static int fat_getnewls (HLS *hls, char **pos, const char *fname, int victim, unsigned char filter, unsigned char alter)
{
    struct dir dir[1];
    struct dirent dirent[1];
    int candidate;
    int rc;

    *pos = (char *)fname;   /* assign anyway */

    rc = _opendir (dir, hls->dirname);
    if (rc < 0) {
        ERROR ("_opendir(), rc=%d\n", rc);
        return -1;
    }

    if (dir->total < hls->nfiles)
        return 0;

    if (!victim) {
        WARN ("no victim\n");
        return -1;
    }

    candidate = dir->head;
    while ((candidate & LINK_NULL) != LINK_NULL) {
        rc = _getdirent (dir, dirent, candidate);
        if (rc < 0) {
            ERROR ("_getdirent(), rc=%d\n", rc);
            return rc;
        }
        if (((dirent->Attribute ^ alter) & (filter|alter)) == 0) {
            char name[40];
            sprintf (name, "%s/%s", hls->dirname, dirent->Name);
            INFO ("remove(%s)\n", name);
            rc = _remove (name);
            if (rc < 0) {
                ERROR ("_remove(), rc=%d\n", rc);
                return -1;
            }
            return 0;
        }
        candidate = dirent->dire_next;
    }

    /* not found */
    return -1;
}

static int fat_setattrls (HLS *hls, char *pos, int attribute)
{
    char name[NAMELEN+1];

    snprintf (name, NAMELEN, "%s/%s", hls->dirname, pos);
    return _setattr (name, attribute);
}

/*****************************************************************************
 * LS public API
 *****************************************************************************/

HLS *openls (int media, int filesystem)
{
    HLS *hls;
    char *dirname;
    int nfiles, maxsize;
    int rc;

    if (filesystem < 0 || filesystem >= FS_NUM) {
        ERROR ("illigal filesystem\n");
        return NULL;
    }

    if (media < 0 || media >= MEDIA_NUM) {
        ERROR ("illigal media\n");
        return NULL;
    }

    rc = media_get_realconfig (filesystem, media, &dirname, &nfiles, &maxsize);
    if (rc < 0) {
        ERROR ("media_get_realconfig(), rc=%d\n", rc);
        return NULL;
    }

    hls = &_hls[filesystem][media];
    memset (hls, 0, sizeof (HLS));

    hls->magic = HLS_MAGIC;
    hls->media = media;
    hls->filesystem = filesystem;
    hls->dirname = dirname;
    hls->nfiles = nfiles;
    hls->maxsize = maxsize;
    hls->current = HEAD_NULL;
    hls->mark[0] = hls->mark[1]  = hls->current;

    if (filesystem == FATFS) {
        hls->gettotalls = fat_gettotalls;
        hls->getnextls = fat_getnextls;
        hls->getprevls = fat_getprevls;
        hls->getcurrls = fat_getcurrls;
        hls->deletels = fat_deletels;
        hls->getnewls = fat_getnewls;
        hls->setattrls = fat_setattrls;
    } else
    if (filesystem == NORFS) {
        hls->gettotalls = nor_gettotalls;
        hls->getnextls = nor_getnextls;
        hls->getprevls = nor_getprevls;
        hls->getcurrls = nor_getcurrls;
        hls->deletels = nor_deletels;
        hls->getnewls = nor_getnewls;
        hls->setattrls = nor_setattrls;
    }

    return hls;
}


int gettotalls (HLS *hls, int *nls, int *npick, int filter, int mask)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    return hls->gettotalls (hls, nls, npick, filter, mask);
}

int getnextls (HLS *hls, char **pos, int *attribute)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    return hls->getnextls (hls, pos, attribute);
}

int getprevls (HLS *hls, char **pos, int *attribute)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    return hls->getprevls (hls, pos, attribute);
}

int getcurrls (HLS *hls, char **pos, int *attribute)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    return hls->getcurrls (hls, pos, attribute);
}

int deletels (HLS *hls)
{
    int i;

    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    /* move mark position, if current is mark */
    for (i = 0; i < 2 ;i ++) {
        if (hls->current == hls->mark[i] && 
            (hls->mark[i] != TAIL_NULL && hls->mark[i] != HEAD_NULL)) {
            unsigned short prev;
            char *pos;
            int attr;
            hls->getprevls(hls, &pos, &attr);  
            prev = hls->current;
            hls->current = hls->mark[i];
            hls->mark[i] = prev;
        }
    }
        
    return hls->deletels (hls);
}

int getnewls (HLS *hls, char **pos, const char *fname, int victim, unsigned char filter, unsigned char alter)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    return hls->getnewls (hls, pos, fname, victim, filter, alter);
}

int closels (HLS *hls)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    hls->magic = 0;
    return 0;
}

int setattrls (HLS *hls, char *pos, int attribute)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    return hls->setattrls (hls, pos, attribute);
}

int pos2headls (HLS *hls)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    hls->current = HEAD_NULL;
    return 0;
}

int pos2taills (HLS *hls)
{
    if (hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    hls->current = TAIL_NULL;
    return 0;
}

int markls(HLS *hls, int marki)
{
    if (hls == NULL || hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    if (marki > 2 || marki < 0) {
        return -1;
    }

    hls->mark[marki] = hls->current;

    return 0;
}

int seekmarkls(HLS *hls, int marki)
{
    if (hls == NULL || hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return -1;
    }

    if (marki > 2 || marki < 0) {
        return -1;
    }

    hls->current = hls->mark[marki];

    return 0;
}

int matchmarkls(HLS *hls, int marki)
{
    if (hls == NULL || hls->magic != HLS_MAGIC) {
        ERROR ("bad magic number\n");
        return 0;
    }
    if (marki > 2 || marki < 0) {
        return 0;
    }

    return (hls->current == hls->mark[marki]) ? 1 : 0;
}
