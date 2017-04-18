/**
 *  @file   norfs.c
 *  @brief  NOR Filesystem
 *  $Id: norfs.c,v 1.40 2016/05/11 11:56:25 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.40 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/08/29  hugo    New file.
 *
 */
#include <stdlib.h>
#include <string.h>
#include <norfs.h>
#include <nor.h>
#include <time.h>
#include <sys.h>
#include <mconfig.h>
#include <config.h>
#include <debug.h>

#define NOR_BA2VA(BADDR) (ba2pa (gSYSC.pnor, BADDR) | 0x80000000)
#define NORFS_OFFSET(FD) (FD->dirent->address + FD->offset + NORFS_HEADER_SIZE)

static unsigned short dir_prevfile (struct norfs_dir *dir, unsigned short index);
static unsigned short dir_nextfile (struct norfs_dir *dir, unsigned short index);

/*****************************************************************************
 * low-level I/O
 *****************************************************************************/

static int
ll_read (int addr, int len, void *buf)
{
    //INFO ("ll_read(%08x, %d, %p)\n", addr, len, buf);
    if ((addr & 0x80000000) != 0x80000000) {
        ERROR ("invalid address\n");
        return -1;
    }

    memcpy (buf, (void *)addr, len);
    return 0;
}

static int
ll_write (int addr, int len, void *buf)
{
    //INFO ("ll_write(%08x, %d, %p)\n", addr, len, buf);
    return nor_data_write (addr, len, buf);
}

/*****************************************************************************
 * for debug
 *****************************************************************************/

int
nor_read (int offset, int length, char *buf)
{
    return ll_read (NOR_BA2VA (0) + offset, length, buf);
}

/*****************************************************************************
 * partition
 *****************************************************************************/

static struct norfs_partition partition[MEDIA_NUM];

void
norfs_show_partition (void)
{
    int i;
    for (i = 0; i < MEDIA_NUM; i++) {
        if (partition[i].magic != NORFS_MAGIC)
            continue;
        printf ("nor[%d] start=%d, count=%d, files=%d, fsize=%d\n",
                i,
                partition[i].start,
                partition[i].count,
                partition[i].file_num,
                partition[i].file_len);
    }
}

void
_norfs_reset_partition(void)
{
    memset(partition, 0, sizeof(partition));
}

int
norfs_set_partition (int media, struct norfs_partition *p)
{
    if (media < 0 || media >= MEDIA_NUM) {
        ERROR ("bad media\n");
        return -1;
    }

    if ((p->file_len % NOR_BLKSIZ) != 0) {
        ERROR ("bad file length\n");
        return -1;
    }

    p->magic = NORFS_MAGIC;
    p->count = (p->file_len * p->file_num) / NOR_BLKSIZ;
    memcpy (&partition[media], p, sizeof (struct norfs_partition));

//    norfs_show_partition ();
    return p->count;
}

int
norfs_get_partition (int media, struct norfs_partition *p)
{
    if (media < 0 || media >= MEDIA_NUM) {
        ERROR ("bad media\n");
        return -1;
    }

    if (partition[media].magic != NORFS_MAGIC) {
        ERROR ("partition not defined\n");
        return -1;
    }

    memcpy (p, &partition[media], sizeof (struct norfs_partition));
    return 0;
}

int
norfs_format_partition (void)
{
    unsigned int address;
    int i, j;
    for (i = 0; i < MEDIA_NUM; i++) {
        if (partition[i].magic != NORFS_MAGIC)
            continue;

        for (j = 0; j < partition[i].count; j++) {
            address = NOR_BA2VA (partition[i].start + j);
            if (nor_block_erase (address) < 0) {
                ERROR ("E_FILE_IO\n");
                return -E_FILE_IO;
            }
            //dbg (0, "erase(%d, %x)\n", partition[i].start + j, address);
        }
    }
    return 0;
}

/*****************************************************************************
 * name translation
 *****************************************************************************/

static void
split_dir_file (char *pathbuf, char **dirname, char **objname)
{
    char *ptr;

    ptr = strrchr (pathbuf, '/');
    if (ptr == NULL) {
        *dirname = NULL;
        *objname = pathbuf;
        return;
    }

    if (ptr == pathbuf) {
        *dirname = NULL;
    } else {
        *dirname = pathbuf;
        ptr[0] = '\0';
    }
    if (ptr[1] == '\0') {
        *objname = NULL;
    } else {
        *objname = ptr + 1;
    }
}

static int
dir2media (const char *dirname)
{
    char *name;
    int len;
    int i;

    for (i = 0; i < MEDIA_NUM; i++) {
        name = media_dirname (NORFS, i);
        if (!name)
            continue;
        len = strlen (name);

        if (strncmp (dirname, name, len) == 0)
            return i;
    }

    /* unknown */
    ERROR ("unknown dir: %s\n", dirname);
    return -1;
}

/*****************************************************************************
 * header operation
 *****************************************************************************/

static unsigned int
header_get (unsigned int addr, struct norfs_header *header)
{
    if (addr == 0xffffffff) {
        ERROR ("out of range\n");
        return NULL;
    }

    ll_read (addr, NORFS_HEADER_SIZE, header);
    return addr;
}

static int
header_set (unsigned int addr, struct norfs_header *header)
{
    char *header_data = (char *)header;
    int header_size = NORFS_HEADER_SIZE;
    int rc;

    /* header without the magic field */
    rc = ll_write (addr+4, header_size-4, header_data+4);
    if (rc < 0) {
        ERROR ("ll_write(), rc=%d\n", rc);
        return rc;
    }

    /* the magic field */
    rc = ll_write (addr, 4, header_data);
    if (rc < 0) {
        ERROR ("ll_write(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

/*****************************************************************************
 * dirent operation
 *****************************************************************************/

static struct norfs_dirent *
dirent_get (struct norfs_dir *dir, int index, struct norfs_dirent *dirent)
{
    struct norfs_header header[1];
    unsigned int file_blk, address;

    if (index >= dir->file_num) {
        /* exceed file number */
        return NULL;
    }

    file_blk = dir->file_len / NOR_BLKSIZ;
    address = NOR_BA2VA (dir->blk_start + (index * file_blk));

    if (!header_get (address, header)) {
        /* nor found */
        return NULL;
    }

    memcpy (dirent, header, NORFS_HEADER_SIZE);
    dirent->attribute = ~header->attribute; /* invert */
    dirent->address = address;
    dirent->index = index;

    return dirent;
}

typedef int (*__norfs_filter_fn_t) (const struct norfs_dirent *dirent);

static struct norfs_dirent *
dirent_next (struct norfs_dir *dir, struct norfs_dirent *dirent, __norfs_filter_fn_t filter)
{
    while ((dirent_get (dir, dir->curr++, dirent))) {
        if (!filter || filter (dirent))
            return dirent;
    }

    /* not found */
    return NULL;
}

static int filter_valid (const struct norfs_dirent *dirent)
{
    return (dirent->magic == NORFS_MAGIC);
}

static struct norfs_dirent *
dirent_next_valid (struct norfs_dir *dir, struct norfs_dirent *dirent)
{
    if (!dirent_next (dir, dirent, filter_valid))
        return NULL;

    dirent->prev = dir_prevfile (dir, dirent->index);
    dirent->next = dir_nextfile (dir, dirent->index);
    //INFO ("%5d %5d %5d\n", dirent->prev, dirent->index, dirent->next);
    return dirent;
}

static int filter_free (const struct norfs_dirent *dirent)
{
    return (dirent->magic != NORFS_MAGIC);
}

static struct norfs_dirent *
dirent_next_free (struct norfs_dir *dir, struct norfs_dirent *dirent)
{
    return dirent_next (dir, dirent, filter_free);
}

static struct norfs_dirent *
dirent_find (struct norfs_dir *dir, struct norfs_dirent *dirent, const char *name)
{
    dir->curr = 0;
    while ((dirent_next_valid (dir, dirent))) {
        if (!strcmp (dirent->name, name))
            return dirent;
    }

    /* not found */
    return NULL;
}

static struct norfs_dirent *
dirent_find_free (struct norfs_dir *dir, struct norfs_dirent *dirent)
{
    dir->curr = 0;
    while ((dirent_next_free (dir, dirent))) {
        return dirent;
    }

    /* not found */
    return NULL;
}

static void
dirent_time (struct norfs_dirent *dirent)
{
    struct tm t;

    rtc_get_time (&t);
    dirent->time = FAT_SET_TIME (t.tm_hour, t.tm_min, t.tm_sec);
    dirent->date = FAT_SET_DATE (t.tm_year+1970, t.tm_mon + 1, t.tm_mday);
}

static int
dirent_update (struct norfs_dirent *dirent)
{
    struct norfs_header header[1];

    memcpy (header, dirent, NORFS_HEADER_SIZE);
    header->attribute = ~dirent->attribute; /* invert */

    return header_set (dirent->address, header);
}

static int
dirent_invalid (unsigned int addr)
{
    unsigned int zero = 0;
    return ll_write (addr, sizeof (unsigned int), (char *)&zero);
}

/*****************************************************************************
 * dir operation
 *****************************************************************************/

struct norfs_file {
    unsigned short index;
    unsigned int   fileid;
} __attribute__((packed));

static unsigned short
dir_prevfile (struct norfs_dir *dir, unsigned short index)
{
    unsigned char *file_index = dir->file_index;
    int file_count = dir->file_count;
    int i;

    for (i = 0; i < file_count; i++) {
        if (index == file_index[i])
            break;
    }
    i--; // for prev

    return (i >= 0 ? file_index[i] : -1);
}

static unsigned short
dir_nextfile (struct norfs_dir *dir, unsigned short index)
{
    unsigned char *file_index = dir->file_index;
    int file_count = dir->file_count;
    int i;

    for (i = 0; i < file_count; i++) {
        if (index == file_index[i])
            break;
    }
    i++; // for next

    return (i < file_count ? file_index[i] : -1);
}

static int
compare (const void *p1, const void *p2)
{
    struct norfs_file *f1 = (struct norfs_file *)p1;
    struct norfs_file *f2 = (struct norfs_file *)p2;
    return (f1->fileid - f2->fileid);
}

static void
dir_scan (struct norfs_dir *dir)
{
    struct norfs_dirent dirent[1];
    struct norfs_file file[NOR_FILE_NUM], *f = file;
    int index = 0, total = 0;
    int i;

    while ((dirent_get (dir, index, dirent))) {
        f->index = index++;
        if (dirent->magic != NORFS_MAGIC)
            continue;
        total++;

        f->fileid = dirent->fileid;
        f++;
    }
    qsort (file, total, sizeof (struct norfs_file), compare);

    dir->file_count = total;
    dir->file_lastid = total > 0 ? file[total-1].fileid : 0;

    for (i = 0; i < total; i++)
        dir->file_index[i] = file[i].index;

    dir->head = total > 0 ? dir->file_index[0] : -1;
    dir->tail = total > 0 ? dir->file_index[total-1] : -1;
    dir->total = total;
}

static int
dir_open (struct norfs_dir *dir, const char *name)
{
    int media;

    if (name == NULL) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }
        
    media = dir2media (name);
    if (media < 0) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }

    memset (dir, 0, sizeof (struct norfs_dir));
    dir->blk_start = partition[media].start;
    dir->file_num = partition[media].file_num;
    dir->file_len = partition[media].file_len;

    dir_scan (dir);
    return 0;
}

/******************************************************************************
 * NOR FileSystem API
 *****************************************************************************/

int
norfs_opendir (struct norfs_dir *dir, const char *name)
{
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    rc = dir_open (dir, name);

    return rc;
}

int
norfs_closedir (struct norfs_dir *dir)
{
    /* do nothing */
    return 0;
}

int
norfs_cleandir (struct norfs_dir *dir)
{
    unsigned int address, blocks;
    int i;

    blocks = (dir->file_len / NOR_BLKSIZ) * dir->file_num;
    for (i = 0; i < blocks; i++) {
        address = NOR_BA2VA (dir->blk_start + i);
        if (nor_block_erase (address) < 0) {
            ERROR ("E_FILE_IO\n");
            return -E_FILE_IO;
        }
        //dbg (0, "erase(%d, %x)\n", dir->blk_start + i, address);
    }

    return 0;
}

struct norfs_dirent *
norfs_readdir (struct norfs_dir *dir, struct norfs_dirent *dirent)
{
    /* sanity check */
    ASSERT_UNDER_STACK();

    while (dirent_next_valid (dir, dirent)) {
        return dirent;
    }

    return NULL;
}

int
norfs_getdirent (struct norfs_dir *dir, struct norfs_dirent *dirent, int index)
{
    if (!dirent_get (dir, index, dirent))
        return 0;

    dirent->prev = dir_prevfile (dir, dirent->index);
    dirent->next = dir_nextfile (dir, dirent->index);
    //INFO ("%5d %5d %5d\n", dirent->prev, dirent->index, dirent->next);
    return 1;
}

int
norfs_open (struct norfs_fd *fd, const char *name, int size, int mode)
{
    struct norfs_dir dir[1];
    struct norfs_dirent *dirent = (struct norfs_dirent *)fd;
    char pathbuf[64], *dirname, *filename;
    int file_exist;

    /* sanity check */
    ASSERT_UNDER_STACK();

    /* init */
    memset (fd, 0, sizeof (struct norfs_fd));

    /* extract dirname & basename */
    strcpy (pathbuf, name);
    split_dir_file (pathbuf, &dirname, &filename);

    /* open dir */
    if (dir_open (dir, dirname) < 0) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }

    size = MIN (size,  dir->file_len - NORFS_HEADER_SIZE);

    /* check if file exists */
    file_exist = (dirent_find (dir, dirent, filename) != NULL);

    if (mode == NOR_MODE_WRONLY) {
        if (!file_exist) {
            /* find free block */
            if (dirent_find_free (dir, dirent) == NULL) {
                ERROR ("E_FILE_NO_SPACE\n");
                return -E_FILE_NO_SPACE;
            }
        }

        /* erase block */
        int address = dirent->address;
        if (nor_block_erase (address) < 0) {
            ERROR ("E_FILE_IO\n");
            return -E_FILE_IO;
        }

        /* create dirent */
        memset (dirent, 0, sizeof (struct norfs_dirent));
        dirent->address = address;
        dirent->magic = NORFS_MAGIC;
        dirent->fileid = ++dir->file_lastid;
        dirent->attribute = 0x00;
        dirent->length = size;
        dirent_time (dirent);
        strcpy (dirent->name, filename);
    } else
    if (mode == NOR_MODE_RDONLY) {
        if (!file_exist) {
            ERROR ("E_FILE_NOT_EXIST\n");
            return -E_FILE_NOT_EXIST;
        }
    } else {
        ERROR ("E_FILE_MODE\n");
        return -E_FILE_MODE;
    }

    fd->offset = 0;
    fd->mode = mode;

    return 0;
}

int
norfs_close (struct norfs_fd *fd)
{
    int rc;
    struct norfs_dirent *dirent = fd->dirent;

    /* sanity check */
    ASSERT_UNDER_STACK();
    if (fd->mode == NOR_MODE_UNKNOWN) {
        ERROR ("unknown mode\n");
        return -1;
    }

    if (dirent->magic != NORFS_MAGIC) {
        WARN ("file is not opened\n");
        return 0;
    }

    if (fd->mode == NOR_MODE_RDONLY)
        goto EXIT;

    /* if actual writed size less than open size, fix "length" field */
    if (dirent->length < fd->offset) {
        ERROR ("out of range\n");
        return -1;
    }
    dirent->length = fd->offset;

    /* update directory entry */
    rc = dirent_update (dirent);
    if (rc < 0) {
        WARN ("dirent_update(), rc=%d\n", rc);
    }

EXIT:
    dirent->magic = 0;
    return 0;
}

int
norfs_gettotal (struct norfs_dir *dir, int *total, int *npick, int filter, int mask)
{
    struct norfs_dirent dirent[1];
    int index = 0, _total = 0, _npick = 0;

    while ((dirent_get (dir, index++, dirent))) {
        if (dirent->magic != NORFS_MAGIC)
            continue;
        _total++;

        if (filter && (dirent->attribute & mask))
            continue;
        _npick++;
    }

    *total = _total;
    *npick = _npick;

    return 0;
}

int
norfs_remove (const char *name)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    char pathbuf[64], *dirname, *filename;

    /* sanity check */
    ASSERT_UNDER_STACK();

    /* extract dirname & basename */
    strcpy (pathbuf, name);
    split_dir_file (pathbuf, &dirname, &filename);

    if (dir_open (dir, dirname) < 0) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }

    if (!dirent_find (dir, dirent, filename)) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    return dirent_invalid (dirent->address);
}

int
norfs_read (struct norfs_fd *fd, void *buf, int size)
{
    struct norfs_dirent *dirent = fd->dirent;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if (dirent->magic != NORFS_MAGIC) {
        ERROR ("E_FILE_NOT_OPEN\n");
        return -E_FILE_NOT_OPEN;
    }

    if (fd->mode != NOR_MODE_RDONLY) {
        ERROR ("E_FILE_MODE\n");
        return -E_FILE_MODE;
    }

    size = MIN (size, dirent->length - fd->offset);
    ll_read (NORFS_OFFSET(fd), size, buf);

    fd->offset += size;
    return 0;
}

int
norfs_write (struct norfs_fd *fd, void *buf, int size)
{
    struct norfs_dirent *dirent = fd->dirent;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if (dirent->magic != NORFS_MAGIC) {
        ERROR ("E_FILE_NOT_OPEN\n");
        return -E_FILE_NOT_OPEN;
    }

    if (fd->mode != NOR_MODE_WRONLY) {
        ERROR ("E_FILE_MODE\n");
        return -E_FILE_MODE;
    }

    size = MIN (size, dirent->length - fd->offset);
    if (ll_write (NORFS_OFFSET(fd), size, buf) < 0) {
        ERROR ("E_FILE_IO\n");
        return -E_FILE_IO;
    }

    fd->offset += size;
    return 0;
}

int
norfs_seek (struct norfs_fd *fd, int offset)
{
    struct norfs_dirent *dirent = fd->dirent;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if (dirent->magic != NORFS_MAGIC) {
        ERROR ("E_FILE_NOT_OPEN\n");
        return -E_FILE_NOT_OPEN;
    }

    if ((offset + NORFS_HEADER_SIZE) > dirent->length) {
        ERROR ("E_FILE_OFFSET\n");
        return -E_FILE_OFFSET;
    }

    fd->offset = offset;
    return 0;
}

int
norfs_stat (const char *name, struct norfs_stat *st)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    char pathbuf[64], *dirname, *filename;

    /* sanity check */
    ASSERT_UNDER_STACK();

    /* extract dirname & basename */
    strcpy (pathbuf, name);
    split_dir_file (pathbuf, &dirname, &filename);

    /* get directory entry */
    if (dir_open (dir, dirname) < 0) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }

    if (!dirent_find (dir, dirent, filename)) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    st->st_size = dirent->length;
    st->mtime = dirent->time;
    st->mdate = dirent->date;
    st->attribute = dirent->attribute;
    st->start = dirent->address;

    return 0;
}

int norfs_setattr (const char *name, int attribute)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    char pathbuf[64], *dirname, *filename;

    /* sanity check */
    ASSERT_UNDER_STACK();

    /* extract dirname & basename */
    strcpy (pathbuf, name);
    split_dir_file (pathbuf, &dirname, &filename);

    /* get directory entry */
    if (dir_open (dir, dirname) < 0) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }

    if (!dirent_find (dir, dirent, filename)) {
        ERROR  ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* update flags */
    dirent->attribute |= attribute;

    /* write new dirctory entry */
    if (dirent_update (dirent) < 0) {
        ERROR ("E_FILE_IO\n");
        return -E_FILE_IO;
    }

    return 0;
}
int norfs_clearattr (const char *name, int attribute)
{
    struct norfs_dir dir[1];
    struct norfs_dirent dirent[1];
    char pathbuf[64], *dirname, *filename;

    /* sanity check */
    ASSERT_UNDER_STACK();

    /* extract dirname & basename */
    strcpy (pathbuf, name);
    split_dir_file (pathbuf, &dirname, &filename);

    /* get directory entry */
    if (dir_open (dir, dirname) < 0) {
        ERROR ("E_DIR_NOT_EXIST\n");
        return -E_DIR_NOT_EXIST;
    }

    if (!dirent_find (dir, dirent, filename)) {
        ERROR  ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* update flags */
    dirent->attribute &= ~attribute;

    /* write new dirctory entry */
    if (dirent_update (dirent) < 0) {
        ERROR ("E_FILE_IO\n");
        return -E_FILE_IO;
    }

    return 0;
}
void *
norfs_mmap (struct norfs_fd *fd, int offset)
{
    int rc = norfs_seek (fd, offset);

    return (rc < 0 ? MAP_FAILED : (void *) NORFS_OFFSET(fd));
}

void norfs_init (void)
{
    /* reset partition */
    memset (partition, 0, sizeof (partition));
}
