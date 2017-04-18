/**
 *  @file   fat32.c
 *  @brief  library for simple FAT32 filesystem 
 *  $Id: fatfs.c,v 1.80 2016/05/11 11:56:25 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.80 $
 *
 *  Copyrgight (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/02/06  New file.
 *  @date   2014/01/27  Jedy 0xe6, For reserved directory _format will
 *                      create 4 dire for long file name. On dirent_to_dire
 *                      if the file does not occupy all 4 dire, it will 
 *                      mark '0xe6' to make fool PC windows for file copy.
 *
 */
#include <config.h>
#include <ctype.h>
#include <alloca.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <debug.h>
#include <ls.h>
#include <fat32.h>
#include <time.h>
#include <sd.h>
#include <sys.h>
#include <mconfig.h>
#include <heap.h>

static struct fat_t *pfat = NULL;
static struct fat32 *fat = NULL;

extern struct sysc_t gSYSC;

typedef int bool;
enum {false = 0, true = 1};

#define MAGIC_FAT       0x2348089a
#define MAGIC_FD        0x1484fa29

#define SIGNATURE_BS    0xAA55
#define SIGNATURE_TW    0x3673
#define SIGNATURE_DIRE  0x36

#define SFN_SIZE        12  /* 8.3 short name */

/* Signature for RSV and NORMAL file/dir */
#define SIG_NORMAL          SIGNATURE_DIRE
#define SIG_RSV(xtype)      (SIGNATURE_DIRE + (xtype+1))
#define IS_SIGNATURE(xsig)  (((xsig) >= SIGNATURE_DIRE) && ((xsig) <= SIG_RSV(RSVDIR_NUM)))
#define IS_RSV_SIG(xsig)    (((xsig) > SIGNATURE_DIRE) && ((xsig) <= SIG_RSV(RSVDIR_NUM)))
#define RSV_TYPE(xsig)      ((xsig) - SIGNATURE_DIRE - 1) 


/* RSV_DIRE_COUNT */
#define RSV_DIRE_COUNT  4

//#define FAT32_DEBUG

#define CLUS2SECT(CLUS) (fat->root_start + ((CLUS) - FIRST_ROOTCLUSTER) * fat->sect_per_clus)

#define DIRE_PER_FILE   (1+((NAMELEN + 12)/13)) /* 1 for sfn, others for lfn */

typedef struct {
    dire_date       Date;
    dire_time       Time;
    unsigned short  dire_index;
} __attribute__((packed)) fileInfo_t;

#define DateTime(file) (((file)->Date << 16) | ((file)->Time << 0))
static int
compare_byTime (const void *src, const void *dst)
{
    return DateTime((fileInfo_t *)src) - DateTime ((fileInfo_t *)dst);
}

/*****************************************************************************
 * debug helper
 *****************************************************************************/

static void __attribute__ ((unused))
fat32_info (struct fat32 *fat)
{
    int i;
    printf ("[fat32]\n");
    printf ("  sect_per_fat: %d\n", fat->sect_per_fat);
    printf ("  sect_per_clus: %d\n", fat->sect_per_clus);
    printf ("  lba_start: %d\n", fat->lba_start);
    printf ("  fat_start: %d\n", fat->fat_start);
    printf ("  root_start: %d\n", fat->root_start);
    printf ("  ----\n");
    for (i = 0; i < RSVDIR_NUM; i++) {
        if (!fat->e[i].rsvf_size || !fat->e[i].rsvf_num)
            continue;
        printf ("  rsvf[%d] %4d x %6d KB\n", i,
                fat->e[i].rsvf_num, fat->e[i].rsvf_size*32);
    }
    printf ("  ----\n");
    printf ("         %8s %8s\n", "cluster", "@sector"); 
    printf ("  s_dir: %8d %8d\n", fat->s_dir, CLUS2SECT (fat->s_dir));
    printf ("  s_seg: %8d %8d\n", fat->s_seg, CLUS2SECT (fat->s_seg));
    printf ("  s_rsv: %8d %8d\n", fat->s_rsv, CLUS2SECT (fat->s_rsv));
    printf ("  e_fat: %8d %8d\n", fat->e_fat, CLUS2SECT (fat->e_fat));
    printf ("\n");
}

static void __attribute__ ((unused))
bootsector_info (struct bootsector *bs)
{
    int i;
    printf ("[Boot Sector]\n");
    printf ("  JumpCommand: 0x%02x%02x%02x\n",
            bs->JumpCommand[0], bs->JumpCommand[1], bs->JumpCommand[2]);
    printf ("  OEMName: %c%c%c%c%c%c%c%c\n",
            bs->SystemIdentifier[0], bs->SystemIdentifier[1],
            bs->SystemIdentifier[2], bs->SystemIdentifier[3],
            bs->SystemIdentifier[4], bs->SystemIdentifier[5],
            bs->SystemIdentifier[6], bs->SystemIdentifier[7]);
    printf ("  SectorSize: %d\n", bs->SectorSize);
    printf ("  SectorPerCluster: %d\n", bs->SectorPerCluster);
    printf ("  ReservedSectorCount: %d\n", bs->ReservedSectorCount);
    printf ("  FATNumber: %d\n", bs->FATNumber);
    printf ("  RootDirEntryNumber: %d\n", bs->RootDirEntryNumber);
    printf ("  TotalSector16: %d\n", bs->TotalSector16);
    printf ("  MediumIdentifier: 0x%02x\n", bs->MediumIdentifier);
    printf ("  FATSz16: %d\n", bs->FATSz16);
    printf ("  SectorPerTrack: %d\n", bs->SectorPerTrack);
    printf ("  SlideNumber: %d\n", bs->SlideNumber);
    printf ("  HiddenSectorNumber: %u\n", bs->HiddenSectorNumber);
    printf ("  TotalSector32: %u\n", bs->TotalSector32);
    printf ("  FATSz32: %u\n", bs->FATSz32);
    printf ("  ExtFlags: 0x%04x\n", bs->ExtFlags);
    printf ("  FSVer: %d\n", bs->FSVer);
    printf ("  RootClus: %u\n", bs->RootClus);
    printf ("  FSInfo: %d\n", bs->FSInfo);
    printf ("  BkBootSec: %d\n", bs->BkBootSec);
    printf ("  TWSignatureWord: 0x%04x\n", bs->TWSignatureWord);
    printf ("  DirSize:       %6d (%04x)\n", bs->DirSize, bs->DirSize);
    printf ("  DirNumber:     %6d (%04x)\n", bs->DirNumber, bs->DirNumber);
    printf ("  SegmentSize:   %6d (%04x)\n", bs->SegmentSize, bs->SegmentSize);
    printf ("  SegmentNumber: %6d (%04x)\n", bs->SegmentNumber, bs->SegmentNumber);
    for (i = 0; i < RSVDIR_NUM; i++ ) {
        if (bs->e[i].RsvFileNumber != 0) {
            printf ("  RsvFileSize  [%d]: %6d (%04x)\n", i, bs->e[i].RsvFileSize, bs->e[i].RsvFileSize);
            printf ("  RsvFileNumber[%d]: %6d (%04x)\n", i, bs->e[i].RsvFileNumber, bs->e[i].RsvFileNumber);
        }
    }
    printf ("  SignatureWord: 0x%04x\n", bs->SignatureWord);
    printf ("\n");
}

static void __attribute__ ((unused))
partition_info (struct partition *partition)
{
    printf ("[PARTITION]\n");
    printf ("  BootIndicator: %d\n", partition->BootIndicator);
    printf ("  StartingHead: %d\n", partition->StartingHead);
    printf ("  StartingSector: %d\n", partition->StartingSector);
    printf ("  StartingCylinder: %d\n", partition->StartingCylinder);
    printf ("  SystemID: 0x%02x\n", partition->SystemID);
    printf ("  EndingHead: %d\n", partition->EndingHead);
    printf ("  EndingSector: %d\n", partition->EndingSector);
    printf ("  EndingCylinder: %d\n", partition->EndingCylinder);
    printf ("  RelativeSector: %u\n", partition->RelativeSector);
    printf ("  TotalSector: %u\n", partition->TotalSector);
    printf ("\n");
}

static void __attribute__ ((unused))
dirent_info (struct dirent *dirent)
{
    printf ("[DirEntry]\n");
    printf ("  Name: %s\n", dirent->Name);
    printf ("  StartCluster: %u\n", dirent->StartCluster);
    printf ("  Length: %u\n", dirent->Length);
    printf ("  Time: %02d:%02d:%02d\n",
            FAT_GET_HOUR (dirent->Time),
            FAT_GET_MIN (dirent->Time),
            FAT_GET_SEC (dirent->Time));
    printf ("  Date: %04d-%02d-%02d\n",
            FAT_GET_YEAR (dirent->Date),
            FAT_GET_MONTH (dirent->Date),
            FAT_GET_DAY (dirent->Date));
    printf ("  Attrib: %02x\n", dirent->Attrib);
    printf ("  Attribute: %02x\n", dirent->Attribute);
    printf ("  Signature: %x (%d)\n", dirent->signature, RSV_TYPE(dirent->signature));
    printf ("  dire_count: %d\n", dirent->dire_count);
    printf ("  dire_index: %d\n", dirent->dire_index);
    printf ("  dire_prev: %d\n", dirent->dire_prev);
    printf ("  dire_next: %d\n", dirent->dire_next);
    printf ("\n");
}

static void __attribute__ ((unused))
dir_info (struct dir *dir)
{
    dirent_info ((struct dirent *)dir);
    printf ("[Dir]\n");
    printf ("  curr: %d\n", dir->curr);
    printf ("  head: %d\n", dir->head);
    printf ("  tail: %d\n", dir->tail);
    printf ("\n");
}

/*****************************************************************************
 * low-level I/O with lock
 *****************************************************************************/

extern __mips32__ int test_and_set_bit (unsigned long nr, volatile unsigned long *addr);
extern __mips32__ void clear_bit (unsigned long nr, volatile unsigned long *addr);

static int
ll_probe (void)
{
    struct drive_op *op = pfat->op;
    void *hcard = pfat->hcard;

    return op->init_card (hcard);
}

static int
ll_remove (void)
{
    struct drive_op *op = pfat->op;
    void *hcard = pfat->hcard;

    return op->release_card (hcard);
}

static int
ll_size (void)
{
    struct drive_op *op = pfat->op;
    void *hcard = pfat->hcard;

    return op->device_size (hcard);
}

static int
ll_read (int start, int count, void *buf)
{
    struct drive_op *op = pfat->op;
    void *hcard = pfat->hcard;
    int rc;

    while (test_and_set_bit (0, &pfat->lock) != 0);
    rc = op->read_sector (hcard, start, count, 512, buf);
    clear_bit (0, &pfat->lock);

    return rc;
}

static int
ll_write (int start, int count, void *buf)
{
    struct drive_op *op = pfat->op;
    void *hcard = pfat->hcard;
    int rc;

    while (test_and_set_bit (0, &pfat->lock) != 0);
    rc = op->write_sector (hcard, start, count, 512, buf);
    clear_bit (0, &pfat->lock);

    return rc;
}

static int
ll_erase (int start, int count)
{
    struct drive_op *op = pfat->op;
    void *hcard = pfat->hcard;
    char buf[SECT_SIZE] __attribute__ ((aligned (4)));
    int i;
    int rc = 0;

    memset (buf, 0, SECT_SIZE);
    while (test_and_set_bit (0, &pfat->lock) != 0);
    for (i = 0; i < count; i++) {
        rc = op->write_sector (hcard, start + i, 1, 512, buf);
        if (rc < 0)
            break;
    }
    clear_bit (0, &pfat->lock);

    return rc;
}

/*****************************************************************************
 * String Operation
 *****************************************************************************/

static inline int
isallcase (char *name)
{
    int lc = 0, up = 0;

    for (; *name != '\0'; name++) {
        if (islower (*name))
            lc++;
        if (isupper (*name))
            up++;
    }

    if (lc == 0 || up == 0)
        return 1;

    return 0;
}

static int
make_pathname (char *pathbuf, const char *name)
{
    char *ptr;
    if (!strcmp (name, "..")) {
        strcpy (pathbuf, fat->cur_dir);
        /* upper level */
        ptr = strrchr (pathbuf, '/');
        *++ptr = '\0';
    } else
    if (!strcmp (name, ".")) {
        strcpy (pathbuf, fat->cur_dir);
    } else
    if (name[0] != '/') {
        /* relative path */
        if (strlen (name) + strlen (fat->cur_dir) + 1 > PATHLEN) {
            return -E_DIR_NAME;
        }

        if (strcmp (fat->cur_dir, "/") == 0)
            sprintf (pathbuf, "/%s", name);
        else
            sprintf (pathbuf, "%s/%s", fat->cur_dir, name);
    } else {
        /* absolute path */
        if (strlen (name) > PATHLEN) {
            return -E_DIR_NAME;
        }

        strcpy (pathbuf, name);
    }

    return 0;
}

static void
split_dir_obj (char *pathbuf, char **dirname, char **objname)
{
    char *ptr;

    ptr = strrchr (pathbuf, '/');
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
dir_file_name (const char *name, char *pathbuf, char **dirname, char **filename)
{
    int rc;

    rc = make_pathname (pathbuf, name);
    if (rc < 0) {
        return rc;
    }

    split_dir_obj (pathbuf, dirname, filename);

    return 0;
}

static char *
getdir (char *path, char **dir)
{
    while (*path == '/')
        path++;

    *dir = (*path != '\0') ? path : NULL;

    path = strchr (path, '/');
    if (path) {
        *path++ = '\0';
        if (*path == '\0')
            path = NULL;
    }

    return path;
}

/*****************************************************************************
 * bootsector & partition operation
 *****************************************************************************/
#define NumFATs         2
#define RsvdSectCnt     32

/* the offset of Master Boot Record and Partition Table */
#define MASTER_BOOT_RECORD_OFFSET               0
#define PARTITION_TABLE0_OFFSET                 446
#define PARTITION_TABLE1_OFFSET                 462
#define PARTITION_TABLE2_OFFSET                 478
#define PARTITION_TABLE3_OFFSET                 494
#define PARTITION_AREA_SIGNATURE_OFFSET         510

static int
fat_read_bootsector (struct fat32 *fat)
{
    char buf[SECT_SIZE] __attribute__ ((aligned (4)));
    struct bootsector *bs;
    int sector;
    int i;
    int n_rsv;
    int rc;

    sector = fat->lba_start;
    rc = ll_read (sector, 1, buf);
    if (rc < 0) {
        ERROR ("ll_read(), rc=%d\n", rc);
        return rc;
    }

    bs = (struct bootsector *) buf;
    if (bs->TWSignatureWord != SIGNATURE_TW) {
        fat->is_readonly = 1;
    }

    if (bs->SectorSize != SECT_SIZE) {
        rc = -E_SD_UNSUPPORTED;
        goto ERR;
    }
    if (bs->FATSz16 != 0) {
        rc = -E_FS_UNSUPPORTED;
        goto ERR;
    }
    if (bs->SignatureWord != SIGNATURE_BS) {
        fat->is_readonly = 1;
    }

    fat->sect_per_fat = bs->FATSz32;
    fat->sect_per_clus = bs->SectorPerCluster;
    fat->fat_start = fat->lba_start + bs->ReservedSectorCount;
    fat->root_start = fat->fat_start + (bs->FATNumber * bs->FATSz32);

    n_rsv = 0;
    for (i = 0; i < RSVDIR_NUM; i++ ) {
        fat->e[i].rsvf_size = bs->e[i].RsvFileSize;
        fat->e[i].rsvf_num = bs->e[i].RsvFileNumber;
        n_rsv += fat->e[i].rsvf_size * fat->e[i].rsvf_num;
    }

    fat->s_dir = fat->c_dir = FIRST_ROOTCLUSTER;
    fat->n_dir = bs->DirSize * bs->DirNumber;
    fat->s_seg = fat->s_dir + fat->n_dir;
    fat->n_seg = bs->SegmentSize * bs->SegmentNumber;
    fat->s_rsv = fat->s_seg + fat->n_seg;
    fat->n_rsv = n_rsv;
    fat->e_fat = fat->s_rsv + fat->n_rsv;

    fat32_info (fat);
    printf ("TotSect: %u, LbaStart: %u, FatStart: %u, RootStart: %u\n",
            fat->nsector, fat->lba_start, fat->fat_start, fat->root_start);

    /* check media config */
    return 0;

ERR:
    bootsector_info (bs);
    return rc;
}

static int
fat_calc_FATSz (unsigned int nsector)
{
    unsigned int FATSz = 1;
    unsigned int data_sect, data_clus;
    int adj;

    while (1) {
        data_sect = nsector - RsvdSectCnt - (NumFATs * FATSz);
        data_clus = data_sect / SECT_PER_CLUS;
        adj = CDIV (data_clus, FATE_PER_SECT) - FATSz;
        if (adj <= 0)
            break;
        FATSz += (adj / 2) | 1;
    }

    return FATSz;
}

static void
fat_make_bootsector (struct fat32 *fat, void *buf)
{
    struct bootsector *bs = buf;
    int i;

    memset (bs, 0, sizeof (struct bootsector));
    bs->JumpCommand[0] = 0xEB;
    bs->JumpCommand[1] = 0x58;
    bs->JumpCommand[2] = 0x90;
    memcpy (bs->SystemIdentifier, "MSDOS5.0", 8);
    bs->SectorSize = SECT_SIZE;
    bs->SectorPerCluster = fat->sect_per_clus;
    bs->ReservedSectorCount = RsvdSectCnt;
    bs->FATNumber = NumFATs;
    bs->MediumIdentifier = 0xf8;
    //bs->SectorPerTrack = 0x3f;
    //bs->SlideNumber = 0xff;
    //bs->HiddenSectorNumber = 0x2000;
    bs->TotalSector32 = fat->nsector - fat->lba_start;
    bs->FATSz32 = fat->sect_per_fat;
    bs->RootClus = FIRST_ROOTCLUSTER;
    bs->FSInfo = 1;
    bs->BkBootSec = 6;
    bs->FAT32_PhyDiskNumber = 0x80;
    bs->FAT32_ExtendSignature = 0x29;
    bs->FAT32_VolumeID = 0x84ac26c9;
    memcpy (bs->FAT32_VolumeLable, "SD Card    ", 11);
    memcpy (bs->FAT32_FileSystemType, "FAT32   ", 8);
    bs->TWSignatureWord = SIGNATURE_TW;
    bs->SignatureWord = SIGNATURE_BS;
    bs->DirSize = CLUS_PER_DIR;
    bs->DirNumber = MAX_DIR_CNT;
    bs->SegmentSize = CLUS_PER_SEG;
    bs->SegmentNumber = fat->n_seg / CLUS_PER_SEG;
    for (i = 0; i < RSVDIR_NUM; i++) {
       bs->e[i].RsvFileSize = fat->e[i].rsvf_size;
       bs->e[i].RsvFileNumber = fat->e[i].rsvf_num;
    }

    bootsector_info (bs);
}

static int
fat_read_partition (struct fat32 *fat)
{
    char buf[SECT_SIZE] __attribute__ ((aligned (4)));
    struct partition partition[1] __attribute__ ((aligned (4)));
    int rc;

    rc = ll_read (0, 1, buf);
    if (rc < 0) {
        ERROR ("ll_read(), rc=%d\n", rc);
        return rc;
    }

    /* XXX: memcpy due to data alignment */
    memcpy (partition, buf + PARTITION_TABLE0_OFFSET, sizeof(struct partition));
    switch (partition->BootIndicator) {
        case 0x00:                 /* inactive */
        case 0x80:                 /* active */
            break;
        default:
            /* invalid boot indicator */
            fat->lba_start = 0;
            goto EXIT;
    }
    switch (partition->SystemID) {
        case 0x01:                 /* FAT 12 */
        case 0x04:                 /* FAT 16 with size < 32 MB */
        case 0x06:                 /* FAT 16 with size >= 32 MB */
        case 0x0e:                 /* Windows 95 FAT 16 with LBA */
        case 0x0b:                 /* Windows 95 FAT 32 */
        case 0x0c:                 /* Windows 95 FAT 32 with LBA */
            break;
        default:
            /* unsupported system ID */
            fat->lba_start = 0;
            goto EXIT;
    }

    fat->lba_start = partition->RelativeSector;

EXIT:

    //partition_info (partition);
    return 0;
}

int
fat_info (void)
{
    struct fat32 fat[1];
    int rc;

    /* device init */
    rc = ll_probe ();
    if (rc < 0) {
        ERROR ("ll_probe(), rc=%d\n", rc);
        return rc;
    }

    fat->nsector = ll_size ();
    rc = fat_read_partition (fat);
    if (rc < 0) {
        ERROR ("fat_read_partition(), rc=%d\n", rc);
        return rc;
    }

    rc = fat_read_bootsector (fat);
    if (rc < 0) {
        ERROR ("fat_read_bootsector(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

/*****************************************************************************
 * FAT Table operation
 *****************************************************************************/
#define EOC             0xfffffff

static int
fate_cache_flush (void)
{
    int sector;
    int rc;

    sector = fat->fate_cache_sector;
    if (sector < 0) {
        /* cache not defined */
        return 0;
    }

    if (!fat->fate_cache_taint) {
        /* cache not tainted */
        return 0;
    }

    //INFO ("fate_cache_flush >>>> ll_write(%d)\n", fat->fat_start + sector);
    rc = ll_write (fat->fat_start + sector, 1, fat->fate_cache);
    if (rc < 0) {
        ERROR ("ll_write(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
fate_cache_load (int sector)
{
    int rc;

    /* flush current cache */
    rc = fate_cache_flush ();
    if (rc < 0) {
        ERROR ("fate_cache_flush(), rc=%d\n", rc);
        return rc;
    }

    /* reload cache */
    //INFO ("fate_cache_load  >>>> ll_read(%d)\n", fat->fat_start + sector);
    rc = ll_read (fat->fat_start + sector, 1, fat->fate_cache);
    if (rc < 0) {
        ERROR ("ll_read(), rc=%d\n", rc);
        return rc;
    }
    fat->fate_cache_sector = sector;
    fat->fate_cache_taint = 0;

    return 0;
}

static int
fate_get (unsigned int index, unsigned int *value)
{
    unsigned int *fate = fat->fate_cache;
    int sector, offset;
    int rc;

    if (index > fat->e_fat) {
        ERROR ("E_FS_OUT_OF_RANGE\n");
        return -E_FS_OUT_OF_RANGE;
    }

    sector = index / FATE_PER_SECT;
    offset = index % FATE_PER_SECT;

    /* reload if cache missed */
    if (sector != fat->fate_cache_sector) {
        rc = fate_cache_load (sector);
        if (rc < 0) {
            ERROR ("fate_cache_load(), rc=%d\n", rc);
            return rc;
        }
    }

    *value = fate[offset];
    return 0;
}

static int
fate_set (int index, unsigned int value)
{
    unsigned int *fate = fat->fate_cache;
    int sector, offset;
    int rc;

    if (index > fat->e_fat) {
        ERROR ("E_FS_OUT_OF_RANGE\n");
        return -E_FS_OUT_OF_RANGE;
    }

    sector = index / FATE_PER_SECT;
    offset = index % FATE_PER_SECT;

    /* reload if cache missed */
    if (sector != fat->fate_cache_sector) {
        rc = fate_cache_load (sector);
        if (rc < 0) {
            ERROR ("fate_cache_load(), rc=%d\n", rc);
            return rc;
        }
    }

    /* update cache */
    fate[offset] = value;
    fat->fate_cache_taint++;
    return 0;
}

static int
fate_shrink (unsigned int clus, int count)
{
    unsigned int next;
    int rc;

    while (clus < 0x0ffffff0) {
        /* get next cluster */
        rc = fate_get (clus, &next);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }

        if (next == 0) {
            WARN ("FATE truncated @%d\n", clus);
            break;
        }

        /* XXX FAT dangling is possible if power off or SD card is removed */
        /* update fat entry */
        if (--count <= 0) {
            rc = fate_set (clus, (count == 0) ? EOC : 0);
            if (rc < 0) {
                ERROR ("fate_get(), rc=%d\n", rc);
                return rc;
            }
        }

        clus = next;
    }
    fate_cache_flush ();

    return 0;
}

void
fate_used (void)
{
    unsigned int clus;
    unsigned int next = -1;
    int i, first = 0;
    int rc;

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT)
        return;

    printf ("scan fat table: %u - %u\n", fat->s_seg, fat->e_fat);

    for (i = fat->s_seg; i < fat->e_fat; i++) {
        rc = fate_get (i, &clus);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return;
        }

        if (next == -1) {
            if (clus == 0)
                continue;
            first = i;
            if (clus != EOC) {
                next = clus;
                continue;
            }
        } else
        if (next == i) {
            if (clus != EOC) {
                next = clus;
                continue;
            }
        }

        printf ("%6d - %6d  %6d\n", first, i, i - first + 1);
        next = -1;
    }

    if (next != -1)
        printf ("%6d - %6d  %6d\n", first, i, i - first + 1);
}

void
fate_free (void)
{
    unsigned int clus;
    unsigned int last = -1;
    int i, first = 0;
    int rc;

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT)
        return;

    printf ("scan fat table: %u - %u\n", fat->s_seg, fat->e_fat);

    for (i = fat->s_seg; i < fat->e_fat; i++) {
        rc = fate_get (i, &clus);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return;
        }

        if (last == -1) {
            if (clus == 0) {
                first = i;
                last = i;
            }
            continue;
        } else {
            if (clus == 0) {
                last = i;
                continue;
            }
        }

        printf ("%6d - %6d  %6d\n", first, last, last - first + 1);
        last = -1;
    }

    if (last != -1)
        printf ("%6d - %6d  %6d\n", first, last, last - first + 1);
}

void
fate_dump (unsigned int clus, unsigned int count)
{
    unsigned int next;
    unsigned int i;
    int rc;

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT)
        return;

    if (clus + count > fat->e_fat)
        count = fat->e_fat - clus;

    for (i = 0; i < count; i++) {
         rc = fate_get (clus + i, &next);
         if (rc < 0) {
             ERROR ("fate_get(), rc=%d\n", rc);
             return;
         }

         if (i % 8 == 0) printf ("%6d : ", clus + i);
         if (next > 0x0ffffff0)
             printf ("%7X ", next);
         else
             printf ("%7d ", next);
         if ((i + 1) % 8 == 0) printf ("\n");
    }
    if ((i + 1) % 8 == 0) printf ("\n");
}

int
fate_trace (unsigned int clus)
{
    unsigned int next;
    int idx = 0;
    int count = 0;
    int first = -1;
    int rc;

    printf ("FATE trace:\n");
    while (clus < 0x0ffffff0) {
        /* get next cluster */
        rc = fate_get (clus, &next);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }

        count++;
        if (first == -1) {
            first = clus;
        }
        if ((clus + 1) != next) {
            printf ("  R[%d] %6d - %6d : %6d\n", idx++, first, clus, clus - first + 1);
            first = -1; /* for next region */
        }

        clus = next;
    }
    printf ("  ---------------------- %6d\n", count);

    return count;
}

/*****************************************************************************
 * dire cache
 *****************************************************************************/

/* cache of directory entries */
static struct dire  _dire_cache[DIRE_PER_SECT];
static int          _dire_cache_cluster;
static int          _dire_cache_sector = -1;
static char         _dire_cache_taint = 0;

#define dire_cache_hit(cluster, sector) ((cluster == _dire_cache_cluster) && (sector == _dire_cache_sector))

#define dire_cache_reset(cluster, sector) do { _dire_cache_cluster = cluster; _dire_cache_sector = sector; } while(0)

static int
dire_cache_flush (void)
{
    int sector;

    if (_dire_cache_sector < 0)
        /* invalid cache */
        return 0;

    if (!_dire_cache_taint)
        /* untainted */
        return 0;

    _dire_cache_taint = 0;

    sector = CLUS2SECT (_dire_cache_cluster) + _dire_cache_sector;
    //INFO ("dire_cache_flush >>>> ll_write(%d)\n", sector);
    return ll_write (sector, 1, _dire_cache);
}

static int
dire_cache_load (void)
{
    int sector;

    sector = CLUS2SECT (_dire_cache_cluster) + _dire_cache_sector;
    //INFO ("dire_cache_load  >>>> ll_read(%d)\n", sector);
    return ll_read (sector, 1, _dire_cache);
}

static int
dire_cache_prepare (int cluster, int sector)
{
    int rc;

    if (sector >= SECT_PER_DIR) {
        ERROR ("E_DIR_OUT_OF_RANGE, sector=%d\n", sector);
        return -E_DIR_OUT_OF_RANGE;
    }

    if (dire_cache_hit (cluster, sector))
        return 0;

    rc = dire_cache_flush ();
    if (rc < 0) {
        ERROR ("dire_cache_flush(), rc=%d\n", rc);
        return rc;
    }

    dire_cache_reset (cluster, sector);

    rc = dire_cache_load ();
    if (rc < 0) {
        ERROR ("dire_cache_load(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
dire_cache_get (struct dire *dire, int cluster, int index)
{
    int sector, offset;
    int rc;

    sector = index / DIRE_PER_SECT;
    offset = index % DIRE_PER_SECT;

    rc = dire_cache_prepare (cluster, sector);
    if (rc < 0) {
        ERROR ("dire_cache_prepare(), rc=%d\n", rc);
        return rc;
    }

    memcpy (dire, _dire_cache + offset, sizeof (struct dire));
    return 0;
}

static int
dire_cache_set (struct dire *dire, int cluster, int index)
{
    int sector, offset;
    int rc;

    sector = index / DIRE_PER_SECT;
    offset = index % DIRE_PER_SECT;

    rc = dire_cache_prepare (cluster, sector);
    if (rc < 0) {
        ERROR ("dire_cache_prepare(), rc=%d\n", rc);
        return rc;
    }

    /* update cache */
    if (dire)
        memcpy (_dire_cache + offset, dire, sizeof (struct dire));
    else
        memset (_dire_cache + offset, 0xe5, 1);

    _dire_cache_taint++;
    return 0;
}

/*****************************************************************************
 * dire operation
 *****************************************************************************/

static int
dire_get (struct dire *dire, int cluster, int index, int count)
{
    int i;
    int rc;

    for (i = 0; i < count; i++) {
        rc = dire_cache_get (dire + i, cluster, index + i);
        if (rc < 0) {
            ERROR ("dire_get_one(), rc=%d\n", rc);
            return rc;
        }
    }

    return count;
}

static int
dire_set (struct dire *dire, int cluster, int index, int count)
{
    int i;
    int rc;

    for (i = 0; i < count; i++) {
        rc = dire_cache_set (dire ? dire + i : NULL, cluster, index + i);
        if (rc < 0) {
            ERROR ("dire_set_one(), rc=%d\n", rc);
            return rc;
        }
    }

    return count;
}

static bool
dire_is_available (struct dire *dire, int count)
{
    int i;

    for (i = 0; i < count; i++) {
        if (dire[i].Name[0] != 0xe5 && dire[i].Name[0] != 0x00)
            return false;
    }
    return true;
}

static int
dire_request (struct dir* dir , struct dire* dire, int cluster, struct dirent *dirent)
{
    int index, start_index, count, max_index;
    unsigned int memoStartCluster = fat->memo_startcluster;
    unsigned int memoUpdateIndex = fat->memo_updateindex;
    int rc;
    if (IS_RSV_SIG(dir->dirent->signature)) {
        int im = RSV_TYPE(dir->dirent->signature);
        max_index = fat->e[im].rsvf_num * 4 + 4; /* . .. .rsv .rsv2 4 entry */
    } else if (IS_SIGNATURE(dir->dirent->signature)) {
        max_index = DIRE_PER_DIR; 
    } else {
        max_index = DIRE_PER_DIR_NOSIG;
    }

    if (memoStartCluster == cluster)
        start_index = memoUpdateIndex;
    else
        start_index = 0;

    count = dirent->dire_count;
    
    for(index = start_index; index < max_index; index++) {
        rc = dire_get(dire, cluster, index, count);
        if ( rc > 0) {
            if (dire_is_available(dire, count) == true) {
                goto SUCCESS;
           }
        }
        else if(rc < 0){
            ERROR ("dire_get(), rc=%d\n", rc);
            return rc;
        }
        
    }

    for (index = 0; index < start_index; index++) {
        rc = dire_get(dire, cluster, index, count);
         if (rc > 0) {
            if (dire_is_available(dire, count) == true) {
                goto SUCCESS;
            }
        }
        else if(rc < 0){
            ERROR ("dire_get(), rc=%d\n", rc);
            return rc;
        }
    }

    /* no more directory entry */
    return -E_FILE_NO_SPACE;

SUCCESS:
    dirent->dire_index = index;
    fat->memo_updateindex = index;
    fat->memo_startcluster = cluster;
    return 0;
}

static int
dire_update (struct dire *dire, int cluster, struct dirent *dirent)
{
    int index, count;
    int rc;

    index = dirent->dire_index;
    count = dirent->dire_count;

    if (IS_RSV_SIG(dirent->signature) && (dirent->Attrib & ATTRIB_ARCHIVE)) {
        count = 4;
    }

    rc = dire_set (dire, cluster, index, count);
    if (rc < 0) {
        ERROR ("dire_set(), rc=%d\n", rc);
        return rc;
    }

    rc = dire_cache_flush ();
    if (rc < 0) {
        ERROR ("dire_cache_flush(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

/*****************************************************************************
 * dirent cache
 *****************************************************************************/

#define NUM_DIRENT_CACHE    4
static struct dirent _dirent_cache[NUM_DIRENT_CACHE];
static unsigned char _dirent_cache_index = 0;

static void
dirent_cache_reset (void) //20150528 Kevin Add. 
{
    int i;
    
    _dirent_cache_index = 0;
    for(i = 0; i <NUM_DIRENT_CACHE; i++)
        memset (&_dirent_cache[i], 0, sizeof (struct dirent));
}

static struct dirent *
dirent_cache_find_byName (int dirCluster, const char *Name)
{
    struct dirent *dirent;
    int i;

    for (i = 0; i < NUM_DIRENT_CACHE; i++) {
        dirent = _dirent_cache + i;
        if ((dirent->dirCluster == dirCluster) &&
            strcmp (dirent->Name, Name) == 0)
            return dirent;
    }
    return NULL;
}

static struct dirent *
dirent_cache_find_byIndex (int dirCluster, int dire_index)
{
    struct dirent *dirent;
    int i;

    for (i = 0; i < NUM_DIRENT_CACHE; i++) {
        dirent = _dirent_cache + i;
        if ((dirent->dirCluster == dirCluster) &&
            (dirent->dire_index == dire_index))
            return dirent;
    }
    return NULL;
}

static void
dirent_cache_set (struct dirent *dirent, int dirCluster)
{
    struct dirent *_dirent;

    if (!(dirent->Attrib & ATTRIB_ARCHIVE))
        return;
    if (strcmp (dirent->Name, ".") == 0)
        return;
    if (strcmp (dirent->Name, "..") == 0)
        return;

    dirent->dirCluster = dirCluster;

    _dirent = dirent_cache_find_byIndex (dirCluster, dirent->dire_index);
    if (_dirent == NULL) {
        _dirent = _dirent_cache + (_dirent_cache_index++ % NUM_DIRENT_CACHE);
    }

    memcpy (_dirent, dirent, sizeof (struct dirent));
}

static void
dirent_cache_del (struct dirent *dirent, int dirCluster)
{
    struct dirent *_dirent;

    if (!(dirent->Attrib & ATTRIB_ARCHIVE))
        return;
    if (strcmp (dirent->Name, ".") == 0)
        return;
    if (strcmp (dirent->Name, "..") == 0)
        return;

    dirent->dirCluster = dirCluster;

    _dirent = dirent_cache_find_byIndex (dirCluster, dirent->dire_index);
    if (_dirent == NULL)
        return;

    //dbg (0, "del %d:%d %s %d/%d\n", dirent->dirCluster, dirent->dire_index, dirent->Name, dirent->dire_prev, dirent->dire_next);
    memset (_dirent, 0, sizeof (struct dirent));
}

/*****************************************************************************
 * dirent operation
 *****************************************************************************/

#define SET_LONG_FILE_NAME(ptr, i, name) { ptr[i] = *name; ptr[i+1] = '\0'; if(*name == '\0') break; else name++; }
#define GET_LONG_FILE_NAME(ptr, i, name) { *name = ptr[i];if(*name == '\0') break; else name++; }

static int
dirent_name (struct dirent *dirent, const char *filename)
{
    char SFName[SFN_SIZE+1], *name, *ext;
    int namelen;
    int onlyshort = 0;
    int dire_count;

    if (strlen (filename) > NAMELEN) {
        ERROR ("E_FILE_NAME\n");
        return -E_FILE_NAME;
    }

    /* check for only short name */
    if ((namelen = strlen (filename)) <= 12) {
        strcpy (SFName, filename);

        name = SFName;
        ext = strchr (SFName, '.');
        if (ext == NULL)
            ext = "";
        else
            *ext++ = '\0';

        if (strlen (name) <= 8 && strlen (ext) <= 3)
            onlyshort = 1;
    }

    if (onlyshort) {
        strcpy (SFName, filename);
        dire_count = 1;
    } else {
        memset (SFName, 0, 16);
        strncpy (SFName, filename, 6);
        ext = strchr (SFName, '.');
        if (ext == NULL)
            strcat (SFName, "~1");
        else
            *ext = '\0';

        ext = strchr (filename, '.');
        if (ext)
            strncat (SFName, ext, 4);

        dire_count = 1 + ((namelen + 12) / 13);
    }

    if (dire_count > DIRE_PER_FILE) {
        ERROR ("too many dire\n");
        return -1;
    }

    dirent->dire_count = dire_count;

    /* copy names */
    strcpy (dirent->Name, filename);

    return 0;
}

static void
dirent_time (struct dirent *dirent)
{
    struct tm t;

    rtc_get_time (&t);
    dirent->Time = FAT_SET_TIME (t.tm_hour, t.tm_min, t.tm_sec);
    dirent->Date = FAT_SET_DATE (t.tm_year + 1970, t.tm_mon + 1, t.tm_mday);
}

static int
dirent_create (struct dirent *dirent, const char *filename, char sig)
{
    int rc;

    memset (dirent, 0, sizeof (struct dirent));

    rc = dirent_name (dirent, filename);
    if (rc < 0) {
        ERROR ("dirent_name(), rc=%d\n", rc);
        return rc;
    }
    
    dirent->signature = sig;
    dirent->Attribute = 0x00;

    if (IS_RSV_SIG(sig)) {
        if (dirent->dire_count > RSV_DIRE_COUNT) {
            ERROR ("E_FILE_NAME\n");
            return -E_FILE_NAME;
        }
        dirent->dire_count = RSV_DIRE_COUNT;
    }

    dirent_time (dirent);

    return rc;
}

static void
dirent_from_dire (struct dirent *dirent, struct dire *dire)
{
    char SFName[SFN_SIZE+1];
    char *FileName, *LongFileName, *Ptr;
    struct dire *FirstDirEntry;
    int i;

    /* store original directory entry to FirstDirEntry */
    FirstDirEntry = dire;

    while (dire->Attrib == ATTRIB_LONG_NAME)
        dire++;

    /* get short file name */
    FileName = SFName;
    for (i = 0; i < 8; i++, FileName++) {   /* 8.3 name, end at ' ' */
        if (dire->Name[i] == ' ')
            break;
        *FileName = dire->Name[i];
    }
    for (i = 0; i < 3; i++, FileName++) {   /* 8.3 extension, end at ' ' */
        if (dire->Extension[i] == ' ')
            break;
        if (i == 0) {
            *FileName = '.';
            FileName++;
        }
        *FileName = dire->Extension[i];
    }
    *FileName = '\0';

    /* get file attribute */
    dirent->Time = (dire_time) (dire->Time);
    dirent->Date = (dire_date) (dire->Date);
    dirent->StartCluster = FAT_GET_CLUS (dire);
    dirent->Length = dire->FileLength;
    dirent->Attrib = dire->Attrib;
    dirent->dire_prev = dire->Prev;
    dirent->dire_next = dire->Next;
    dirent->file_total = dire->Total;
    dirent->Attribute = dire->Attribute;
    dirent->signature = IS_SIGNATURE(dire->signature) ? dire->signature : 0;

    dirent->dire_count = (int)(dire - FirstDirEntry + 1);

    /* get long file name (UTF-16 characters) */
    LongFileName = dirent->Name;
    if (FirstDirEntry == dire) {
        strcpy (LongFileName, SFName);
        str2lower (LongFileName);
        return;
    }

    while (dire != FirstDirEntry) {
        dire--;
        Ptr = (char *) dire;

        /* so for, we only support one-byte character */
        GET_LONG_FILE_NAME (Ptr,  1, LongFileName);
        GET_LONG_FILE_NAME (Ptr,  3, LongFileName);
        GET_LONG_FILE_NAME (Ptr,  5, LongFileName);
        GET_LONG_FILE_NAME (Ptr,  7, LongFileName);
        GET_LONG_FILE_NAME (Ptr,  9, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 14, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 16, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 18, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 20, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 22, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 24, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 28, LongFileName);
        GET_LONG_FILE_NAME (Ptr, 30, LongFileName);
    }
    *LongFileName = '\0';
}

static void
dirent_to_dire (struct dirent *dirent, struct dire *dire)
{
    char SFName[SFN_SIZE+1];
    char *FileName, *LongFileName, *Ptr;
    struct dire *FirstDirEntry;
    int count;
    int Seq = 1;
    unsigned char *fcbName;
    unsigned char sum = 0;
    int i;

    /* store original directory entry to FirstDirEntry */
    FirstDirEntry = dire;

    if (IS_RSV_SIG(dirent->signature) && (dirent->Attrib & ATTRIB_ARCHIVE)) {
        count = 4;
        memset (dire, 0xe6, sizeof (struct dire) * count);
    } else {
        count = dirent->dire_count;
        memset (dire, 0x00, sizeof (struct dire) * count);
    }
    dire += (count - 1);

    /* workaround short file name for windows xp */
    if (strlen (dirent->Name) > 12) {
        sprintf (SFName, "N~%06d", dirent->StartCluster);
        char *ext = strrchr (dirent->Name, '.');
        if (ext)
            strncat (SFName, ext, 4);
    } else {
        strcpy (SFName, dirent->Name);
        str2upper (SFName);
    }

    /* set short file name */
    FileName = SFName;
    if (!strcmp (FileName, ".")) {
        memcpy (dire->Name, ".       ", 8);
        memcpy (dire->Extension, "   ", 3);
    } else
    if (!strcmp (FileName, "..")) {
        memcpy (dire->Name, "..      ", 8);
        memcpy (dire->Extension, "   ", 3);
    } else {
        for (i = 0; i < 8; i++) {           /* 8.3 name, end at ' ' */
            if ((*FileName == '.') || (*FileName == '\0')) {
                dire->Name[i] = ' ';
            } else {
                dire->Name[i] = *FileName;
                FileName++;
            }
        }
        if (*FileName != '\0') {            /* omit '.' */
            FileName++;
        }
        for (i = 0; i < 3; i++) {           /* 8.3 extension, end at ' ' */
            if (*FileName == '\0') {
                dire->Extension[i] = ' ';
            } else {
                dire->Extension[i] = *FileName;
                FileName++;
            }
        }
    }

    /* set file attribute */
    dire->Attrib = dirent->Attrib;
    dire->Prev = dirent->dire_prev;
    dire->Next = dirent->dire_next;
    dire->Total = dirent->file_total;
    dire->Attribute = dirent->Attribute;
    dire->Time = (unsigned short) dirent->Time;
    dire->Date = (unsigned short) dirent->Date;
    dire->StartCluster = (unsigned short) (dirent->StartCluster);
    dire->StartClusterEx = (unsigned short) (dirent->StartCluster >> 16);
    dire->FileLength = dirent->Length;
    dire->signature = dirent->signature;

    /* calculate checksum of short name */
    fcbName = (unsigned char *)dire->Name;
    for (i = 0; i < 11; i++) {
        sum = ((sum & 1) << 7) + (sum >> 1) + fcbName[i];
    }

    /* set long file name (UTF-16 characters) */
    LongFileName = dirent->Name;
    while (dire != FirstDirEntry) {
        dire--;
        Ptr = (char *) dire;
        memset (Ptr, 0xff, 32);
#define LAST_LONG_ENTRY 0x40
        Ptr[0x00] = Seq++ | (dire == FirstDirEntry ? LAST_LONG_ENTRY : 0);
        Ptr[0x0b] = ATTRIB_LONG_NAME;
        Ptr[0x0c] = 0;
        Ptr[0x0d] = sum;
        Ptr[0x1a] = 0;
        Ptr[0x1b] = 0;

        /* so for, we only support one-byte character */
        SET_LONG_FILE_NAME (Ptr,  1, LongFileName);
        SET_LONG_FILE_NAME (Ptr,  3, LongFileName);
        SET_LONG_FILE_NAME (Ptr,  5, LongFileName);
        SET_LONG_FILE_NAME (Ptr,  7, LongFileName);
        SET_LONG_FILE_NAME (Ptr,  9, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 14, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 16, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 18, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 20, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 22, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 24, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 28, LongFileName);
        SET_LONG_FILE_NAME (Ptr, 30, LongFileName);
    }

    /* special handle for rsv file */
    /* rev file always take 4 dire for long file name */
    if (dire != FirstDirEntry) {
        Ptr = (char *) dire;
        Ptr[0x00] = (Seq-1) | LAST_LONG_ENTRY;

        while(dire != FirstDirEntry) {
            dire--;
            Ptr = (char *)dire;
            memset(Ptr, 0xff, 32);
    
            Ptr[0x00] = 0xe6; /* special reserved entry for rsv lfn */
            Ptr[0x0b] = ATTRIB_LONG_NAME;
            Ptr[0x0c] = 0;
            Ptr[0x0d] = sum;
            Ptr[0x1a] = 0;
            Ptr[0x1b] = 0;
        }
    }
}

struct dirent *
dirent_next (struct dir *dir, struct dirent *dirent)
{
    struct dire dire[4];
    int cluster = dir->dirent->StartCluster;
    int count, index, max_index;

    if (IS_RSV_SIG(dir->dirent->signature)) {
        int im = RSV_TYPE(dir->dirent->signature);
        max_index = fat->e[im].rsvf_num * 4 + 4; /* . .. .rsv .rsv2 4 entry */
   } else if (IS_SIGNATURE(dir->dirent->signature)) {
        max_index = DIRE_PER_DIR; 
    } else {
        max_index = DIRE_PER_DIR_NOSIG;
    }

    for (index = dir->curr; index < max_index; index++) {
        count = MIN (4, max_index - index);

        if (dire_get (dire, cluster, index, count) <= 0)
            break;

        if (dire[0].Attrib == ATTRIB_VOLUME_ID) {
            /* skip volumn label entry */
            continue;
        }

        if (dire[0].Name[0] == 0xe5) {
            /* skip free directory entry */
            continue;
        }

        if (dire[0].Name[0] == 0x00 || dire[0].Name[0] == 0xff) {
            /* end of directory entry table */
            return NULL;
        }

        /* directory entry info */
        dirent_from_dire (dirent, dire);
        dirent->dire_index = index;
        dirent_cache_set (dirent, dir->dirent->StartCluster);

        /* advance dire_index */
        dir->curr = dirent->dire_index + dirent->dire_count;

        return dirent;
    }

    /* no more directory entry */
    return NULL;
}

//struct dirent *
static int
dirent_get (struct dir *dir, struct dirent *dirent, int index)
{
    struct dire _dire[4], *dire = _dire;
    int cluster = dir->dirent->StartCluster;
    int rc;

    if (index == LINK_NULL)
        return false;

    rc = dire_get (dire, cluster, index, 4);
    if (rc < 0) {
        /* no more directory entry */
        ERROR ("dire_get()\n");
        return rc;
    }
    dirent->Name[0] = dire->Name[0];
    unsigned char name = (unsigned char)dire->Name[0];
    if (name == 0xe5 || name == 0x00 || name == 0xff)
        return false;

    while (dire->Name[0] == 0xe6) {
        dire++;
    }

 

    /* get directory entry info */
    dirent_from_dire (dirent, dire);
    dirent->dire_index = index;
    dirent_cache_set (dirent, dir->dirent->StartCluster);

    return true;
}

static int
dirent_set (struct dir *dir, struct dirent *dirent, int index)
{
    struct dire dire[DIRE_PER_FILE];
    int cluster = dir->dirent->StartCluster;
    int rc;

    if (dirent->dire_index != index) {
        ERROR ("bad index\n");
        return -1;
    }

    dirent_to_dire (dirent, dire);
    dirent_cache_set (dirent, dir->dirent->StartCluster);
    rc = dire_update (dire, cluster, dirent);
    if (rc < 0) {
        ERROR ("dire_update(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
dirent_del (struct dir *dir, struct dirent *dirent, int index)
{
    int cluster = dir->dirent->StartCluster;
    int rc;

    if (dirent->dire_index != index) {
        ERROR ("bad index\n");
        return -1;
    }

    dirent_cache_del (dirent, dir->dirent->StartCluster);
    rc = dire_update (NULL, cluster, dirent);
    if (rc < 0) {
        ERROR ("dire_update(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
dirent_update (struct dir *dir, int index, int prev, int next, int total)
{
    struct dirent dirent[1];
    int rc;

    if (!dirent_get (dir, dirent, index)) {
        ERROR ("dirent_get()\n");
        return -1;
    }

    if (prev != -1)
        dirent->dire_prev = prev;
    if (next != -1)
        dirent->dire_next = next;
    if (index == 0)
        dirent->file_total = total;

    rc = dirent_set (dir, dirent, index);
    if (rc < 0) {
        ERROR ("dirent_set(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

static int
dirent_add (struct dir *dir, struct dirent *dirent)
{
    struct dire dire[DIRE_PER_FILE];
    int rc;
    int cluster = dir->dirent->StartCluster;
    char sig = dir->dirent->signature;

    /* request free directory entries */
    rc = dire_request (dir, dire, cluster, dirent);
    if (rc < 0) {
        ERROR ("dire_request(), rc=%d\n", rc);
        return rc;
    }

    /* rsv dir ? */
    if (IS_RSV_SIG(sig)) {
        int id = RSV_DIRE_COUNT - 1;
        int im;
        if (dirent->dire_count != RSV_DIRE_COUNT) {
            ERROR ("!RSV_DIRE_COUNT\n");
            return -1;
        }
        if (dire[id].signature != sig) {
            ERROR ("E_FILE_NO_SPACE\n");
            return -E_FILE_NO_SPACE;
        }
        
        if (dire[id].signature != sig) {
            ERROR ("bad signature\n");
            return -1;
        }
        if (dirent->signature != sig) {
            ERROR ("bad signature\n");
            return -1;
        }
        im = RSV_TYPE(sig);
        if (im < 0 || im > RSVDIR_NUM) {
            ERROR ("illegal im(%d)\n", im);
            return -1;
        }
        dirent->Length = fat->e[im].rsvf_size * CLUS_SIZE;
        dirent->StartCluster = dire[id].StartCluster | (dire[id].StartClusterEx << 16);
    }

    /* manage signature */
    dirent->signature = sig;

    if (!IS_RSV_SIG(sig))
        goto EXIT;

    /*** manage double linked list ***/
    /* update dir->tail */
    dirent->dire_next = LINK_NULL;
    dirent->dire_prev = dir->tail;
    if (dir->tail != LINK_NULL) {
        /* [dir->tail] point  new dirent */
        rc = dirent_update (dir, dir->tail, -1, dirent->dire_index, 0);
        if (rc < 0) {
            ERROR ("dirent_update(), rc=%d\n", rc);
            return rc;
        }
    }
    /* update "." dirent - keep head, tail */
    if (dir->head == LINK_NULL) {
        dir->head = dirent->dire_index;
    }
    dir->tail = dirent->dire_index;
    if (dir->dirent->StartCluster != FIRST_ROOTCLUSTER) {
        int total = dir->total + 1;
        rc = dirent_update (dir, 0, dir->head, dir->tail, total);
        if (rc < 0) {
            ERROR ("dirent_update(), rc=%d\n", rc);
            return rc;
        }
    }

EXIT:
    /* create current - write new dirctory entries */
    rc = dirent_set (dir, dirent, dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_set(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

int
dirent_find_by_name (struct dir *dir, const char *name, struct dirent *dirent)
{
    struct dirent *_dirent;

    _dirent = dirent_cache_find_byName (dir->dirent->StartCluster, name);
    if (_dirent != 0) {
        memcpy (dirent, _dirent, sizeof (struct dirent));
        return 0;
    }

    dir->curr = 0;
    while ((dirent_next (dir, dirent))) {
        if (strcmp (dirent->Name, name) == 0){            
            return 0;
        }
    }

    /* not found */
    return -1;
}

int
dirent_find_by_index (struct dir *dir, const int index, struct dirent *dirent)
{
    struct dirent *_dirent;

    _dirent = dirent_cache_find_byIndex (dir->dirent->StartCluster, index);
    if (_dirent != 0) {
        memcpy (dirent, _dirent, sizeof (struct dirent));
        return 0;
    }

    dir->curr = 0;
    while ((dirent_next (dir, dirent))) {
        if (dirent->dire_index == index) 
            return 0;
    }

    /* not found */
    return -1;
}

/*****************************************************************************
 * file descriptor operation
 *****************************************************************************/

void
fd_cur_sector (struct fd32 *fd, int *sector, int *number)
{
    struct region *reg = fd->region;
    int cur_sector, acc_sector = 0;
    int s, n;
    int i;

    if (fd->offset > fd->size) {
        ERROR ("out of range\n");
        return;
    }

    cur_sector = fd->offset / SECT_SIZE;
    
    for (i = 0; i < fd->region_cnt; i++) {
        s = reg[i].start * fat->sect_per_clus;
        n = reg[i].count * fat->sect_per_clus;
        if (acc_sector + n > cur_sector) {
            *sector = fat->root_start + (s + cur_sector - acc_sector) - (FIRST_ROOTCLUSTER * fat->sect_per_clus);
            *number = (acc_sector + n) - cur_sector;
            return;
        }
        acc_sector += n;
    }

    /* never reach here */
}

static int
fd_region_alloc (struct fd32 *fd, unsigned int size)
{
    struct region *region = fd->region;
    unsigned int clus, next;
    int count;
    int i, j;
    int rc;

    size = ROUNDUP (size, SEG_SIZE);

    /* init regions */
    for (i = 0; i < REGION_NUM; i++) {
        region[i].start = EOC;
        region[i].count = 0;
    }
    fd->region_cnt = 0;

    if (size == 0)
        /* zero file size */
        return 0;

    /* find space */
    count = CDIV (size, (fat->sect_per_clus * SECT_SIZE));
    for (i = fat->s_seg; i < fat->s_rsv; i += CLUS_PER_SEG) {
        rc = fate_get (i, &clus);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }

        if (clus == 0) {
            /* free fat entry */
            if (region->start == EOC) {
                region->start = i;
                fd->region_cnt++;
            }
            region->count += CLUS_PER_SEG;
            count -= CLUS_PER_SEG;
            if (count == 0)
                break;
        } else {
            /* used fat entry */
            if (region->start != EOC) {
                if (fd->region_cnt >= REGION_NUM) {
                    ERROR ("E_FILE_NO_REGION\n");
                    return -E_FILE_NO_REGION;
                }
                region++;
            }
        }
    }

    if (count) {
        /* out of space */
        ERROR ("E_FILE_NO_SPACE\n");
        return -E_FILE_NO_SPACE;
    }

    /* fill space */
    for (i = 0; i < fd->region_cnt; i++) {
        region = fd->region + i;

        for (j = 0; j < region->count; j++) {
            clus = region->start + j;
            next = (j < (region->count - 1)) ? (clus + 1) : region[1].start;

            rc = fate_set (clus, next);
            if (rc < 0) {
                ERROR ("fate_set(), rc=%d\n", rc);
                return rc;
            }
        }
    }
    fate_cache_flush ();

    return 0;
}

static int
fd_region_load (struct fd32 *fd, unsigned int clus, unsigned int length)
{
    struct region *region = fd->region;
    unsigned int next = EOC;
    int i, count, region_cnt = 0;
    int rc;

    /* init */
    for (i = 0; i < REGION_NUM; i++) {
        region[i].start = EOC;
        region[i].count = 0;
    }
    fd->region_cnt = 0;

    /* load region */
    count = CDIV (length, (fat->sect_per_clus * SECT_SIZE));

    for (i = 0; i < count; i++) {
        if (region->start == EOC) {
            region->start = clus;
            region_cnt++;
            if (region_cnt >= REGION_NUM) {
                ERROR ("too many regions\n");
                return -1;
            }
        }

        rc = fate_get (clus, &next);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }
        region->count++;

        if (next != (clus + 1)) {
            if (next == EOC)
                break;
            region++;
        }
        clus = next;
    }

    fd->region_cnt = region_cnt;

    return 0;
}

/*****************************************************************************
 * dir operation
 *****************************************************************************/

static void
dir_init (struct dir *dir, int clus)
{
    memset (dir, 0, sizeof (struct dir));

    dir->dirent->StartCluster = clus;
    dir->curr = 0;
    dir->head = dir->tail = LINK_NULL;
}

static int
dir_open (struct dir *dir, const char *name)
{
    struct dirent *dirent = (struct dirent *)dir;
    char pathbuf[PATHLEN + 1], *path, *dirname;
    struct dir dirbuf[2];
    struct dir *pdir = dirbuf + 0;  /* parent dir */
    struct dir *cdir = dirbuf + 1;  /* child dir */
    struct dir *tdir;
    unsigned int StartCluster = FIRST_ROOTCLUSTER;
    unsigned int dire_index = 0;
    int rc;
    char signature = 0;

    if (name == NULL)
        /* root directory */
        goto EXIT;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (*name != '/') {
        ERROR ("E_DIR_NAME\n");
        return -E_DIR_NAME;
    }

    if (strlen (name) > PATHLEN) {
        ERROR ("E_DIR_NAME\n");
        return -E_DIR_NAME;
    }

    /* try sub-dir */
    path = strcpy (pathbuf, name);
    dir_init (pdir, FIRST_ROOTCLUSTER);
    while (1) {
        path = getdir (path, &dirname);
        if (dirname == NULL) {
            /* root directory */
            break;
        }

        rc = dirent_find_by_name (pdir, dirname, dirent);
        if (rc < 0 || dirent->Attrib != ATTRIB_DIRECTORY) {
            ERROR ("E_DIR_NOT_EXIST\n");
            return -E_DIR_NOT_EXIST;
        }
#if 0
        if (fat->check_tw_signature && !dirent->signature) {
            ERROR ("E_DIR_SIGNATURE\n");
            return -E_DIR_SIGNATURE;
        }
#endif
        signature = dirent->signature;
        StartCluster = dirent->StartCluster;
        dire_index = dirent->dire_index;
        dir_init (cdir, StartCluster);

        if (path == NULL) {
            /* leaf directory */
            break;
        }

        /* swap pdir/cdir */
        tdir = pdir;
        pdir = cdir;
        cdir = tdir;
    }
    memcpy (dir, dirent, sizeof (struct dirent));

EXIT:
    dir->dirent->StartCluster = StartCluster;
    dir->curr = 0;
    //dir_info (dir);

    if (dir->dirent->StartCluster != FIRST_ROOTCLUSTER) {
        struct dirent dirent[1];
        rc = dirent_get (dir, dirent, 0);
        if(rc<0){
            ERROR ("dirent_get()\n");
            return rc;
        }
        dir->head = dirent->dire_prev;
        dir->tail = dirent->dire_next;
        dir->total = dirent->file_total;
    } else {
        dir->dirent->signature = SIG_NORMAL;
    }

    return 0;
}

static int
dir_find_space (void)
{
    unsigned int clus;
    int i;
    int rc;

    for (i = fat->c_dir; i < fat->s_seg; i += CLUS_PER_DIR) {
        rc = fate_get (i, &clus);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }

        if (clus == 0) {
            fat->c_dir = i;
            return i;
        }
    }

    for (i = fat->s_dir; i < fat->c_dir; i += CLUS_PER_DIR) {
        rc = fate_get (i, &clus);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }

        if (clus == 0) {
            fat->c_dir = i;
            return i;
        }
    }

    ERROR ("E_DIR_NO_SPACE\n");
    return -E_DIR_NO_SPACE;
}

static int
dir_fill_space (struct dir *dir)
{
    int i, clus, next;
    int rc;

    for (i = 0; i < 4; i++) {
        clus = dir->dirent->StartCluster + i;
        next = i < 3 ? clus + 1 : EOC;
        rc = fate_set (clus, next);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }
    }
    fate_cache_flush ();

    return 0;
}

/******************************************************************************
 * File System API
 *****************************************************************************/
#define NUM_FAT_TABLE   1
const char current_dir_name[11] = {
    '.', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
};
const char parent_dir_name[11] = {
    '.',  '.', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20,
};
const char rsv_dir_name[11] = {
    '.', 'r', 's', 'v', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};

const char rsv_dir_name2[11] = {
    '.', 'r', 's', 'v', '2', 0x20, 0x20, 0x20, 0x20, 0x20, 0x20
};

static void
dire_make_dir (struct dire *dire, const char *name, int StartCluster, char sig, int hidden)
{
    struct tm t;
    rtc_get_time (&t);

    memset (dire, 0, sizeof (struct dire));
    memcpy (dire, name, 11);
    dire->Attrib = ATTRIB_DIRECTORY | (hidden ? ATTRIB_HIDDEN : 0);
    dire->Prev = dire->Next = LINK_NULL;
    dire->Time = (unsigned short) FAT_SET_TIME (t.tm_hour, t.tm_min, t.tm_sec);
    dire->Date = (unsigned short) FAT_SET_DATE (t.tm_year+1970, t.tm_mon+1, t.tm_mday);
    dire->StartCluster = (unsigned short) (StartCluster);
    dire->StartClusterEx = (unsigned short) (StartCluster >> 16);
    dire->FileLength = 0;
    dire->signature = sig;
}

static void
dire_make_file (struct dire *dire, int length, int StartCluster, char sig)
{
    struct tm t;
    rtc_get_time (&t);

    memset (dire, 0, sizeof (struct dire));
    dire->Name[0] = 0x00;
    dire->Attrib = ATTRIB_ARCHIVE;
    dire->Time = (unsigned short) FAT_SET_TIME (t.tm_hour, t.tm_min, t.tm_sec);
    dire->Date = (unsigned short) FAT_SET_DATE (t.tm_year+1970, t.tm_mon+1, t.tm_mday);
    dire->StartCluster = (unsigned short) (StartCluster);
    dire->StartClusterEx = (unsigned short) (StartCluster >> 16);
    dire->FileLength = length;
    dire->signature = sig;
    dire->Attribute = 0x00;
}


int 
_fatfs_get_partition(int media, int *pfiles, int *psize)
{
    if (media < 0 || media >= RSVDIR_NUM) {
        ERROR("invalid argument\n");
        return -1;
    }

    if (!_ismounted()) {
        ERROR("fatfs is not mounted\n");
        return -1;
    }

    *pfiles = fat->e[media].rsvf_num;
    *psize  = fat->e[media].rsvf_size;

    return 0;    
}

int
_format (void)
{
    char *xbuf = (char *) heap_alloc(32*1024);
    unsigned int *e = (unsigned int *) xbuf;
    struct dire *dire = (struct dire *) xbuf;

    int s, t, i, j;
    int sec, idx, cnt;
    int rc;

    char *dirname[RSVDIR_NUM];
    int  nfiles, maxsize;
    int  im, ie, sd;
    unsigned int s_rsv, e_fat = 0;


    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) != 0 || fat->magic == MAGIC_FAT) {
        ERROR ("E_FS_BUSY\n");
        rc = -E_FS_BUSY;
        goto EXIT;
    }

    /* define */
#ifdef FAT32_DEBUG
    printf ("FATE_SIZE: %u\n", FATE_SIZE);
    printf ("SECT_SIZE: %u\n", SECT_SIZE);
    printf ("FATE_PER_SECT: %u\n", FATE_PER_SECT);
    printf ("CLUS_SIZE: %u (%uK)\n", CLUS_SIZE, CLUS_SIZE >> 10);
    printf ("SECT_PER_CLUS: %u\n", SECT_PER_CLUS);
    printf ("SEG_SIZE: %u (%uK)\n", SEG_SIZE, SEG_SIZE >> 10);
    printf ("SECT_PER_SEG: %u\n", SECT_PER_SEG);
    printf ("CLUS_PER_SEG: %u\n", CLUS_PER_SEG);
    printf ("DIR_SIZE: %u (%uK)\n", DIR_SIZE, DIR_SIZE >> 10);
    printf ("DIR_SIZE_NOSIG: %u (%uK)\n", DIR_SIZE_NOSIG, DIR_SIZE_NOSIG >> 10);
    printf ("CLUS_PER_DIR: %u\n", CLUS_PER_DIR);
    printf ("CLUS_PER_DIR_NOSIG: %u\n", CLUS_PER_DIR_NOSIG);
    printf ("sizeof(bootsector): %lu\n", sizeof (struct bootsector));
    printf ("----\n");

    printf ("mconfig -----\n");
    if (MEDIA_NUM > RSVDIR_NUM) {
        ERROR ("too many media\n");
        rc = -1;
        goto EXIT;
    }
    for (im = 0; im < MEDIA_NUM; im++) {
        if (media_get_config(FATFS, im, &dirname[im], &nfiles, &maxsize) < 0)
            continue;
        if (nfiles <= -MCONFIG_MAX_NFILES)
            continue;
        printf(" media %d - %s : nfiles %d  maxsize %d\n", im, dirname[im], nfiles, maxsize); 
    }
#endif

    /* disk layout */
    rc = ll_probe ();
    if (rc < 0) {
        ERROR ("ll_probe(), rc=%d\n", rc);
        goto EXIT;
    }
    fat->nsector = ll_size ();
    fat->sect_per_fat = fat_calc_FATSz (fat->nsector);
    fat->sect_per_clus = SECT_PER_CLUS;
    fat->lba_start = 0;
    fat->fat_start = fat->lba_start + RsvdSectCnt;
    fat->root_start = fat->fat_start + (fat->sect_per_fat * NumFATs);

    /* reserved file */
    for (i = 0;i < RSVDIR_NUM; i++) {
        fat->e[i].rsvf_size = 0;
        fat->e[i].rsvf_num = 0;
    }
    for (im = 0; im < MEDIA_NUM; im++) {
        if (media_get_config(FATFS, im, &dirname[im], &nfiles, &maxsize) < 0 ||
                (nfiles < -MCONFIG_MAX_NFILES))
            continue;
        fat->e[im].rsvf_size = (maxsize + CLUS_SIZE-1)/CLUS_SIZE;
        fat->e[im].rsvf_num  = nfiles;
    }
    
    /* fat table layout */
    int n_ent = (fat->nsector - fat->root_start) / SECT_PER_CLUS;
    int e_tbl = fat->sect_per_fat * FATE_PER_SECT;
    /* if nfiles == -1, system will calculate nfiles according card size */
    /* but still reserve space 128MBytes */
    int rspace_sectors = 128*1024*1024 / CLUS_SIZE; 
    int n_rem; 
    int max_nfiles;

    fat->n_dir = CLUS_PER_DIR * MAX_DIR_CNT;
    fat->n_rsv = 0;
    for (i = 0; i < RSVDIR_NUM; i++) {
        if (fat->e[i].rsvf_num < 0)
            continue;
        fat->n_rsv += fat->e[i].rsvf_size * fat->e[i].rsvf_num;
    }

    /* for nfiles == -1 */
    n_rem = n_ent - fat->n_rsv - rspace_sectors;
    for (i = 0; i< RSVDIR_NUM; i++) {
        if (fat->e[i].rsvf_num >= 0)
            continue;

        if (n_rem < 0) {
            ERROR("No more space for nfiles < 0 media_config\n");
            rc = -E_FS_NO_SPACE;
            goto EXIT;
        }

        if (fat->e[i].rsvf_size <= 0) {
            ERROR ("bad rsvf_size\n");
            rc = -1;
            goto EXIT;
        }
        if (fat->e[i].rsvf_num < -1) {
            max_nfiles = -fat->e[i].rsvf_num;
        } else {
            max_nfiles = MCONFIG_MAX_NFILES;
        }

        dbg(2,"rsvf_num %d n_rem %d max_nfiles %d\n",
            fat->e[i].rsvf_num, n_rem, max_nfiles);

        fat->e[i].rsvf_num = n_rem  / fat->e[i].rsvf_size;  

        if (fat->e[i].rsvf_num > max_nfiles)
            fat->e[i].rsvf_num = max_nfiles;

        dbg(2, "nfiles < 0, reserve cluster = %d (%d * %d)\n", 
            fat->e[i].rsvf_num * (fat->e[i].rsvf_size / CLUS_SIZE), 
            fat->e[i].rsvf_num,
            fat->e[i].rsvf_size);
        fat->n_rsv += fat->e[i].rsvf_size * fat->e[i].rsvf_num;
    }

    dbg(2,"n_rsv %d n_ent %d\n", fat->n_rsv, n_ent);

    if (n_ent < (fat->n_dir + fat->n_rsv)) {
       ERROR("Require reserve size %d*%d is bigger than card size %d*%d\n", 
            (fat->n_dir+fat->n_rsv), CLUS_SIZE, n_ent,  CLUS_SIZE);     
       rc = -E_FS_NO_SPACE;
       goto EXIT;
    }
    fat->n_seg = (n_ent - fat->n_dir - fat->n_rsv) & ~(CLUS_PER_SEG - 1);
    fat->s_dir = fat->c_dir = FIRST_ROOTCLUSTER;
    fat->s_seg = fat->s_dir + fat->n_dir;
    fat->s_rsv = fat->s_seg + fat->n_seg;
    fat->e_fat = fat->s_rsv + fat->n_rsv;

    //fat32_info (fat);
    if ((fat->s_rsv / FATE_PER_SECT) <= 0) {
        ERROR ("bad s_rsv\n");
        rc = -1;
        goto EXIT;
    }

    /* format boot sector (0/6) */
    fat_make_bootsector (fat, xbuf);
    for (i = 0; i < 2; i++) {
        s = fat->lba_start + (6 * i);
        rc = ll_write (s, 1, xbuf);
        if (rc < 0) {
            goto EXIT;
        }
    }

    /* setup sectors of FAT1/2 */
    for (t = 0; t < NUM_FAT_TABLE; t++) {
        s = fat->fat_start + (fat->sect_per_fat * t);
        sec = fat->s_rsv / FATE_PER_SECT + s;
        idx = fat->s_rsv % FATE_PER_SECT;
        cnt = 0;

        /* dir partition */
        memset (xbuf, 0, SECT_SIZE);
        e[0] = 0x0ffffff8;  /* reserved */
        e[1] = 0x0fffffff;  /* reserved (EOC) */

                            /* Root Directory Chain */
        for(i = 2; i < CLUS_PER_DIR + 2; i++)
            e[i] = i+1;  /* next to 1st FAT cluster */
        e[i] = EOC;
#if 0
        e[2] = 0x00000003;  /* nex to 1st FAT cluster */
        e[3] = 0x00000004;  /* next to 2nd FAT cluster */
        e[4] = 0x00000005;  /* next to 3rd FAT cluster */
        e[5] = 0x00000006;  /* next to 4th FAT cluster */
        e[6] = EOC;         /* End of Directory Chain */
#endif
        ie = CLUS_PER_DIR + 2;
        for(i = 0; i < RSVDIR_NUM; i++) {
            if (fat->e[i].rsvf_num == 0)
                continue;

            for (j = 0; j < CLUS_PER_DIR; j++) 
                e[ie+j] = ie+1+j;
            e[ie+j] = EOC;
            ie += CLUS_PER_DIR;
#if 0
            e[ie]   = ie+1; /* next to 1st FAT cluster */
            e[ie+1] = ie+2; /* next to 2nd FAT cluster */
            e[ie+2] = ie+3; /* next to 3rd FAT cluster */
            e[ie+3] = ie+4; /* next to 4th FAT cluster */
            e[ie+4] = EOC;  /* End of Directory Chain */
            ie+=5;
#endif
        }

        rc = ll_write (s, 1, xbuf);
        if (rc < 0) {
            goto EXIT;
        }
        s += 1;

        /* segment partition */
        memset (xbuf, 0, XBUF_SIZE); // clear for erase
        while (s < sec) {
            int n = MIN (SECT_PER_XBUF, sec - s);
            rc = ll_write (s, n, xbuf);
            if (rc < 0) {
                goto EXIT;
            }
            s += n;
        }

        /* reserved partition */
        s_rsv = fat->s_rsv;
        e_fat = s_rsv;
        for (im = 0 ; im < RSVDIR_NUM; im++) {
            if (fat->e[im].rsvf_num == 0)
                continue;
            
            e_fat = s_rsv + (fat->e[im].rsvf_num * fat->e[im].rsvf_size);
            cnt = 0;
            for (i = s_rsv; i < e_fat; i++) {
                e[idx] = i + 1;
                idx++;
                cnt++;

                if (cnt == fat->e[im].rsvf_size) {
                    e[idx-1] = EOC;
                    cnt = 0; /* for next reserved file */
                }
                if (idx == FATE_PER_XBUF) {
                    rc = ll_write (s, SECT_PER_XBUF, xbuf);
                    if (rc < 0) {
                        goto EXIT;
                    }
                    s += SECT_PER_XBUF;
                    idx = 0; /* for next xbuf */
                }
            }
            s_rsv = e_fat;
        }
        if (fat->e_fat != e_fat) {
            ERROR ("bad e_fat\n");
            rc = -1;
            goto EXIT;
        }
        /* rest of table */
        for (i = fat->e_fat; i < e_tbl; i++) {
            e[idx++] = 0;
            if (idx == FATE_PER_XBUF) {
                rc = ll_write (s, SECT_PER_XBUF, xbuf);
                if (rc < 0) {
                    goto EXIT;
                }
                s += SECT_PER_XBUF;
                idx = 0; /* for next xbuf */
            }
        }
        if (idx) {
            rc = ll_write (s, idx / FATE_PER_SECT, xbuf);
            if (rc < 0) {
                goto EXIT;
            }
        }
    }

    /* make root directory */
    s = fat->root_start;
    idx = 0; ie = CLUS_PER_DIR + 2;
    for (im = 0; im < RSVDIR_NUM; im++) {
        int len;
        char aname[16];
        if (fat->e[im].rsvf_num == 0)
            continue;
        memset(aname, 0, 16);
        memset(aname, 0x20, 11);
        len = strlen(dirname[im]) > 11 ? 11 : strlen(dirname[im]);
        for (i = 0; i < len; i++) {
            if (dirname[im][i] >= 'a' && dirname[im][i] <= 'z')
                aname[i] = dirname[im][i]-'a'+'A';
            else
                aname[i] = dirname[im][i];
        }
        dire_make_dir (&dire[idx++], aname, ie, SIG_RSV(im), 0);
        ie+=CLUS_PER_DIR;
    }

    for (i = 0; i < DIRE_PER_DIR; i++) {
        memset (&dire[idx], 0, sizeof (struct dire));
        idx++;

        if (idx == DIRE_PER_XBUF) {
            rc = ll_write (s, SECT_PER_XBUF, xbuf);
            if (rc < 0) {
                goto EXIT;
            }
            s += SECT_PER_XBUF;
            idx = 0; /* for next xbuf */
        }
    }

    /* make media reserve files */
    s_rsv = fat->s_rsv;
    sd = CLUS_PER_DIR * SECT_PER_CLUS;
    ie = CLUS_PER_DIR + 2;
    for (im = 0; im < RSVDIR_NUM; im++ ) {
        if (fat->e[im].rsvf_num == 0)
            continue;
        s = fat->root_start + sd;
        sd += CLUS_PER_DIR * SECT_PER_CLUS;
        
        idx = 0;
        dire_make_dir (&dire[idx++], current_dir_name, ie, SIG_RSV(im), 0);
        dire_make_dir (&dire[idx++], parent_dir_name, 0, SIG_NORMAL, 0);
        dire_make_dir (&dire[idx++], rsv_dir_name, ie, SIG_NORMAL, 1);
        dire_make_dir (&dire[idx++], rsv_dir_name2, ie, SIG_NORMAL, 1); 
        ie += CLUS_PER_DIR;
        for (i = 0; i < DIRE_PER_DIR / RSV_DIRE_COUNT; i++) {
            if (i < fat->e[im].rsvf_num) {
                 int len, startclus;
                 len = fat->e[im].rsvf_size * CLUS_SIZE;
                 startclus = s_rsv + (fat->e[im].rsvf_size * i);
                 dire_make_file (&dire[idx++], 0, 0, 0);
                 dire_make_file (&dire[idx++], 0, 0, 0);
                 dire_make_file (&dire[idx++], 0, 0, 0);
                 dire_make_file (&dire[idx++], len, startclus, SIG_RSV(im));
                 if (idx > DIRE_PER_XBUF) {
                     ERROR ("bad idx\n");
                     rc = -1;
                     goto EXIT;
                 }
            } else {
                 memset (&dire[idx], 0, sizeof (struct dire)*RSV_DIRE_COUNT);
                 idx+=4;
                 if (idx > DIRE_PER_XBUF) {
                     ERROR ("bad idx\n");
                     rc = -1;
                     goto EXIT;
                 }
            }

            if (idx == DIRE_PER_XBUF) {
                rc = ll_write (s, SECT_PER_XBUF, xbuf);
                if (rc < 0) {
                    goto EXIT;
                }
                s += SECT_PER_XBUF;
                idx = 0; /* for next xbuf */
            }
        }
        s_rsv += (fat->e[im].rsvf_num * fat->e[im].rsvf_size);
    }

    printf ("Filesystem formated... size: %u MB\n",
            (fat->n_seg + fat->n_rsv) / 32);

    heap_release(xbuf);

    return 0;

EXIT:
    ll_remove ();
    heap_release(xbuf);
    return rc;
}

/*
 * FAT/FS2 mount check
 *
 *  When card is inserted/removed, fat->mnt_cnt++.
 *    if fat is umounted, fat->mnt_cnt = 0, 2, 4, 6... (even)
 *    if fat is mounted,  fat->mnt_cnt = 1, 3, 5, 7... (odd)
 *  When file is opened,
 *    fd->mnt_cnt = fat->mnt_cnt;
 *  When read/write and
 *    fat->mnt_cnt != fd->mnt_cnt, the filesystem is re-mounted after open().
 * Return < 0 error 
 *        = 0 normal mounted
 *        = 1 readonly mounted
 */

int
_mount (struct fat_t *pfat)
{
    int rc;
    int i;
    char *dirname;
    int nfiles, maxsize;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) != 0 || fat->magic == MAGIC_FAT) {
        ERROR ("E_FS_BUSY\n");
        return -E_FS_BUSY;
    }

//    printf ("check_tw_signature: %d\n", fat->check_tw_signature);
    dirent_cache_reset(); //20150528 Kevin Add. 
    fat->is_readonly = 0;

    /* device init */
    rc = ll_probe ();
    if (rc < 0) {
        ERROR ("ll_probe(), rc=%d\n", rc);
        return rc;
    }
    fat->nsector = ll_size ();
    rc = fat_read_partition (fat);
    if (rc < 0) {
        ERROR ("fat_read_partition(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = fat_read_bootsector (fat);
    if (rc < 0) {
        ERROR ("fat_read_bootsector(), rc=%d\n", rc);
        goto EXIT;
    }

    if (! fat->is_readonly) { 
        /* check rsv region with mconfig */
        if (MEDIA_NUM > RSVDIR_NUM) {
            ERROR ("too many media\n");
            return -1;
        }
        for (i = 0; i < MEDIA_NUM; i++) {
            if (media_get_config(FATFS, i, &dirname, &nfiles, &maxsize) < 0 ||
                (nfiles == 0 || nfiles < -MCONFIG_MAX_NFILES))
                continue;
            if (nfiles < 0)
                nfiles = fat->e[i].rsvf_num;
            if (nfiles != fat->e[i].rsvf_num ||
                    ((maxsize + CLUS_SIZE-1)/CLUS_SIZE) != fat->e[i].rsvf_size) {
                printf("ERROR: resver region is not match application's media config\n");
                printf("media config %d %s %d %d (%d clus)\n", i, dirname, nfiles, maxsize, (maxsize+CLUS_SIZE-1)/CLUS_SIZE);
                printf("fat  rsvf_num %d rsv_size %d\n", fat->e[i].rsvf_num, fat->e[i].rsvf_size);                         rc = -E_FS_UNSUPPORTED;
                goto EXIT;
            }
        }
    }

    fat->magic = MAGIC_FAT;
    fat->mnt_cnt++;
    fat->fate_cache_sector = -1;
    _dire_cache_sector = -1;
    _dire_cache_taint = 0;
    strcpy (fat->cur_dir, "/");

    printf ("Filesystem mounted as %s... size: %u MB\n",
            (fat->is_readonly) ? "readonly" : "readwrite", 
            (fat->n_seg + fat->n_rsv) * fat->sect_per_clus / 2048);

    if (pfat) {
        pfat->root_start = fat->root_start;
        pfat->sect_per_clus = fat->sect_per_clus;
    }

    if (fat->is_readonly)
        return 1;
    else 
        return 0;

EXIT:
    ll_remove ();
    return rc;
}

int
_upgrade_mount (struct fat_t *pfat)
{
    // fat->check_tw_signature = 0;

    return _mount(pfat);
}

int
_umount (void)
{
    unsigned short mnt_cnt = fat->mnt_cnt;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    /* device release */
    rc = ll_remove ();
    if (rc < 0) {
        ERROR ("ll_remove(), rc=%d\n", rc);
        return rc;
    }

    memset (fat, 0, sizeof (struct fat32));
    fat->mnt_cnt = mnt_cnt + 1;

    return 0;
}

/* 
 * return 
 *   0 : not mounted
 *   1 : normal mounted
 *   2 : readonly mounted
 */     
int
_ismounted (void)
{
    int mounted;
    if (fat->mnt_cnt & 1) {
        /* test SD state */
        char buf[512];
        int rc;

        rc = ll_read (0, 1, buf);
        if (rc < 0) {
            _umount ();
        }
    }

    mounted = (fat->mnt_cnt & 1) ? 1 : 0;
    if (mounted && fat->is_readonly)
        mounted = 2;    
    
    return mounted;

}

int
_statfs (struct statfs_t *st)
{
    unsigned int clus, free_clus_cnt = 0;
    int i;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    for (i = fat->s_seg; i < fat->s_rsv; i += CLUS_PER_SEG) {
        rc = fate_get (i, &clus);
        if (rc < 0) {
            ERROR ("fate_get(), rc=%d\n", rc);
            return rc;
        }

        if (clus == 0) {
            /* a free segment found */
            free_clus_cnt += CLUS_PER_SEG;
        }
    }

    st->f_bsize = (fat->sect_per_clus * SECT_SIZE);
    st->f_blocks = (fat->n_seg + fat->n_rsv);
    st->f_bfree = free_clus_cnt;
    st->f_namelen = NAMELEN;

    int clus_size = fat->sect_per_clus * SECT_SIZE;
    for (i = 0; i < RSVDIR_NUM; i++) {
        st->e[i].rsvf_len = fat->e[i].rsvf_size * clus_size;
        st->e[i].rsvf_num = fat->e[i].rsvf_num;
    }

    return 0;
}

int
_mkdir (const char *name)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char pathbuf[PATHLEN + 1], *dirname, *objname;
    unsigned int parent_clus, clus;
    int rc;

    if (fat->is_readonly) {
        ERROR("Can't mkdir on a readonly filesystem\n");
        return -E_FS_ACCESS_DENY;
    }

    /* extract dirname & objname */
    rc = dir_file_name (name, pathbuf, &dirname, &objname);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }
    if (objname == NULL)
        /* root exists */
        return 0;

    /* open dir */
    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }
    parent_clus = dir->dirent->StartCluster;
    if (parent_clus == FIRST_ROOTCLUSTER)
        parent_clus = 0;

    /* check if object exists */
    rc = dirent_find_by_name (dir, objname, dirent);
    if (rc == 0) {
        ERROR ("dirent_find_by_name(), rc=%d\n", rc);
        return -E_DIR_EXIST;
    }

    /* check if enough space for dir */
    rc = dir_find_space ();
    if (rc < 0) {
        ERROR ("dir_find_space(), rc=%d\n", rc);
        return rc;
    }
    clus = rc;

    /* create dirent by objname */
    rc = dirent_create (dirent, objname, SIG_NORMAL);
    if (rc < 0) {
        ERROR ("dirent_create(), rc=%d\n", rc);
        return rc;
    }
    dirent->Attrib = ATTRIB_DIRECTORY;
    dirent->Length = 0;
    dirent->StartCluster = clus;

    /* add a directory entry */
    rc = dirent_add (dir, dirent);
    if (rc < 0) {
        ERROR ("dirent_add(), rc=%d\n", rc);
        return rc;
    }

    /* wirte FAT entries */
    dir_init (dir, clus);
    rc = dir_fill_space (dir);
    if (rc < 0) {
        ERROR ("dir_fill_space(), rc=%d\n", rc);
        return rc;
    }

    /* clear dir content */
    rc = ll_erase (CLUS2SECT (clus), SECT_PER_DIR);
    if (rc < 0) {
        ERROR ("ll_erase(), rc=%d", rc);
        return rc;
    }

    /* open sub-dir */
    make_pathname (pathbuf, name);
    rc = dir_open (dir, pathbuf);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }

    /* make "." */
    rc = dirent_create (dirent, ".", SIG_NORMAL);
    if (rc < 0) {
        ERROR ("dirent_create(), rc=%d\n", rc);
        return rc;
    }
    dirent->Attrib = ATTRIB_DIRECTORY;
    dirent->Length = 0;
    dirent->StartCluster = clus;

    rc = dirent_add (dir, dirent);
    if (rc < 0) {
        ERROR ("dirent_add(), rc=%d\n", rc);
        return rc;
    }

    /* make ".." */
    rc = dirent_create (dirent, "..", SIG_NORMAL);
    if (rc < 0) {
        ERROR ("dirent_create(), rc=%d\n", rc);
        return rc;
    }
    dirent->Attrib = ATTRIB_DIRECTORY;
    dirent->Length = 0;
    dirent->StartCluster = parent_clus;

    rc = dirent_add (dir, dirent);
    if (rc < 0) {
        ERROR ("dirent_add(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

int
_rmdir (const char *name)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char pathbuf[PATHLEN + 1], *dirname, *objname;
    int cluster;
    int count = 0;
    int rc;

    if (fat->is_readonly) {
        ERROR("Can't remove dir on a readonly filesystem\n");
        return -E_FS_ACCESS_DENY;
    }

    /* open dir */
    make_pathname (pathbuf, name);
    rc = dir_open (dir, pathbuf);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }
    cluster = dir->dirent->StartCluster;

    /* check objs in dir */
    while ((dirent_next (dir, dirent))) {
        if (strcmp (dirent->Name, ".") && strcmp (dirent->Name, ".."))
            count++;
    }
    if (count > 0) {
        ERROR ("E_DIR_NOT_EMPTY\n");
        return -E_DIR_NOT_EMPTY;
    }

    /* extract dirname & objname */
    split_dir_obj (pathbuf, &dirname, &objname);

    /* open dir */
    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }

    /* check if object exists */
    rc = dirent_find_by_name (dir, objname, dirent);
    if (rc < 0) {
        ERROR ("dirent_find_by_name(), rc=%d\n", rc);
        return -E_FILE_NOT_EXIST;
    }

    /* recycle FAT entry */
    rc = fate_shrink (dirent->StartCluster, 0);
    if (rc < 0) {
        ERROR ("fate_shrink(), rc=%d\n", rc);
        return rc;
    }

    /* delete directory entry */
    rc = dirent_del (dir, dirent, dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_del(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

int
_chdir (const char *name)
{
    char pathbuf[PATHLEN + 1], *ptr;
    struct dir dir[1];
    int rc;

    if (!strcmp (name, "..")) {
        /* upper level */
        ptr = strrchr (fat->cur_dir, '/');
        if (ptr == fat->cur_dir)
            ptr++;
        *ptr = '\0';
    } else
    if (!strcmp (name, ".")) {
        /* do nothing */
    } else
    if (name[0] != '/') {
        /* relative path */
        if (strlen (name) + strlen (fat->cur_dir) + 1 > PATHLEN) {
            ERROR ("E_DIR_NAME\n");
            return -E_DIR_NAME;
        }

        if (strcmp (fat->cur_dir, "/") == 0)
            sprintf (pathbuf, "/%s", name);
        else
            sprintf (pathbuf, "%s/%s", fat->cur_dir, name);

        rc = dir_open (dir, pathbuf);
        if (rc < 0) {
            ERROR ("dir_open(), rc=%d\n", rc);
            return rc;
        }

        strcpy (fat->cur_dir, pathbuf);
    } else {
        /* absolute path */
        if (strlen (name) > PATHLEN) {
            ERROR ("E_DIR_NAME\n");
            return -E_DIR_NAME;
        }

        rc = dir_open (dir, name);
        if (rc < 0) {
            ERROR ("dir_open(), rc=%d\n", rc);
            return rc;
        }

        strcpy (fat->cur_dir, name);
    }

    return 0;
}

char *
_getcwd (char *buf, size_t size)
{

    return strncpy (buf, fat->cur_dir, size);
}

int
_opendir (struct dir *dir, const char *name)
{
    char pathbuf[PATHLEN + 1];
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (!strcmp (name, ".")) {
        strcpy (pathbuf, fat->cur_dir);
    } else {
        rc = make_pathname (pathbuf, name);
        if (rc < 0) {
            ERROR ("make_pathname(), rc=%d\n", rc);
            return rc;
        }
    }

    return dir_open (dir, pathbuf);
    
}

int
_closedir (struct dir *dir)
{
    /* do nothing */
    return 0;
}

int
_gettotal (struct dir *dir, int *total, int *npick, int filter, int mask)
{
    struct dirent dirent[1];
    int file_total = dir->total;
    int _total = 0, _npick = 0;
    int i;
    int rc;



    for (i = 0 ; i < DIRE_PER_DIR; i += 4) {
    	/* return minus if total mismatch */
		if (_total > file_total) {	//if (file_total == _total) {
		    *total = _total;
		    *npick = _npick;
			ERROR ("dir total mismatch\n");
            return -E_DIR_NOT_EMPTY;
        }
        
        rc = dirent_get (dir, dirent, i);
        if(rc<0){
            ERROR ("dirent_get()\n");
            break; //READ failed
        }
        if (!rc){
            if (dirent->Name[0] == 0x00 || dirent->Name[0] == 0xff) {
            /* end of directory entry table */
                break;
            }
            continue;
        }

        if (!(dirent->Attrib & ATTRIB_ARCHIVE))
            continue;
        _total++;

        if (filter && (dirent->Attribute & mask))
            continue;
        _npick++;
    }

    *total = _total;
    *npick = _npick;

    return 0;
}

struct dirent *
_readdir (struct dir *dir, struct dirent *dirent)
{
    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT)
        return NULL;

    while (dirent_next (dir, dirent)) {
        return dirent;
    }

    return NULL;
}

int
_getdirent (struct dir *dir, struct dirent *dirent, int index)
{
    int rc = dirent_get (dir, dirent, index);
    if(rc<0)
        ERROR ("dirent_get()\n");
    return rc;
}

int
_open (struct fd32 *fd, const char *name, unsigned int size, unsigned char flag)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char pathbuf[PATHLEN + 1], *dirname, *filename;
    unsigned int max;
    int cluster;
    int file_exist;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fat->is_readonly && (flag & FD_WRONLY)) {
        ERROR ("Can't create file on a readonly filesystem\n");
        return -E_FS_ACCESS_DENY;
    }

    /* init */
    memset (fd, 0, sizeof (struct fd32));
    fd->magic = MAGIC_FD;
    fd->mnt_cnt = fat->mnt_cnt;

    /* extract dirname & basename */
    rc = dir_file_name (name, pathbuf, &dirname, &filename);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    /* open dir */
    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }
    cluster = dir->dirent->StartCluster;

    /* check if file exists */
    if ((flag & (FD_WRONLY | FD_NOCHECKEXIST)) == (FD_WRONLY | FD_NOCHECKEXIST)) {
        file_exist = 0;
    } else {
        file_exist = (dirent_find_by_name (dir, filename, dirent) == 0);
    }

    if ((flag & FD_WRONLY)== FD_WRONLY) {
        if (!file_exist) {
             
            /* find disk space */
            if (IS_RSV_SIG(dir->dirent->signature)) {
                rc = dirent_create(dirent, filename, dir->dirent->signature);
                if (rc < 0) {
                    ERROR ("dirent_create(), rc=%d\n", rc);
                    return rc;
                }
                dirent->Attrib = ATTRIB_ARCHIVE;
                if (dirent->dire_count != 4) {
                    ERROR ("dirent->dire_count=%d\n", dirent->dire_count);
                    return -E_FILE_NAME;
                }
                rc = dirent_add(dir, dirent);
                if (rc < 0) {
                    ERROR ("dirent_add(), rc=%d\n", rc);
                    return rc;
                }
                /* allocate disk space */
                fd->region_cnt = 1;
                fd->region[0].start = dirent->StartCluster;
                fd->region[0].count = dirent->Length / CLUS_SIZE;
                fd->magic = MAGIC_FD;
                fd->size = dirent->Length;
                fd->dir_clus = dir->dirent->StartCluster;
                fd->start_clus = dirent->StartCluster;
                fd->offset = 0;
                fd->flag = flag; // FD_WRONLY;
            } else {
                rc = fd_region_alloc (fd, size);
                if (rc < 0) {
                    ERROR ("fd_region_alloc(), rc=%d\n", rc);
                    return rc;
                }

                /* create dirent by filename */
                rc = dirent_create (dirent, filename, dir->dirent->signature);
                if (rc < 0) {
                    ERROR ("dirent_create(), rc=%d\n", rc);
                    return rc;
                }
                dirent->Attrib = ATTRIB_ARCHIVE;
                dirent->Length = size;
                dirent->StartCluster = fd->region[0].start;

                /* add a directory entry */
                rc = dirent_add (dir, dirent);
                if (rc < 0) {
                    ERROR ("dirent_add(), rc=%d\n", rc);
                    return rc;
                }
            }
        } else {
            if (IS_RSV_SIG(dirent->signature))
                return -E_FILE_EXIST;
            /* update modify time */
            dirent_time (dirent);

            /* resize if necessary */
            if (size != dirent->Length) {
                /* scan used fat entries */
                rc = fate_trace (dirent->StartCluster);
                if (rc < 0) {
                    ERROR ("fate_trace(), rc=%d\n", rc);
                    return rc;
                }
                max = rc * (fat->sect_per_clus * SECT_SIZE);

                /* resize */
                dirent->Length = MIN (size, max);
                rc = dirent_set (dir, dirent, dirent->dire_index);
                if (rc < 0) {
                    ERROR ("dirent_set(), rc=%d\n", rc);
                    return rc;
                }
            }

            /* load region */
            rc = fd_region_load (fd, dirent->StartCluster, dirent->Length);
            if (rc < 0) {
                WARN ("fat_load_fd(), rc=%d\n", rc);
            }
        }

        /* clear the first 1k */
        if ((flag & FD_CLEAR1K) == FD_CLEAR1K) {
            char buf[2 * SECT_SIZE];
            int sector = CLUS2SECT (dirent->StartCluster);
            memset (buf, 0, 2 * SECT_SIZE);
            rc = ll_write (sector, 2, buf);
            if (rc < 0) {
                ERROR ("ll_write(), rc=%d\n", rc);
                return rc;
            }
        }
    } else
    if ((flag & FD_RDONLY) == FD_RDONLY) {
        if (file_exist) {
            /* load region */
            rc = fd_region_load (fd, dirent->StartCluster, dirent->Length);
            if (rc < 0) {
                WARN ("fat_load_fd(), rc=%d\n", rc);
            }
        } else {
            ERROR ("E_FILE_NOT_EXIST\n");
            return -E_FILE_NOT_EXIST;
        }
    } else {
        ERROR ("E_FILE_FLAG\n");
        return -E_FILE_FLAG;
    }

    fd->magic = MAGIC_FD;
    fd->length = ROUNDDOWN (dirent->Length, 4096);
    fd->size = dirent->Length;
    fd->dir_clus = dir->dirent->StartCluster;
    fd->dire_index = dirent->dire_index;
    fd->start_clus = dirent->StartCluster;
    fd->start_lba = CLUS2SECT (dirent->StartCluster);
    fd->offset = 0;
    fd->flag = flag;

    return 0;
}

int
_close (struct fd32 *fd)
{
    struct dirent dirent[1];
    struct dir dir[1];
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fd->magic != MAGIC_FD || fd->flag == FD_UNKNOWN)
        /* file is not opened */
        return 0;

    if ((fd->flag & FD_RDONLY)== FD_RDONLY) {
        goto DONE;
    }

    /* if actual writed size less than open size, fix FAT table and root dir */
    if (fd->offset > fd->size) {
        ERROR ("out of range\n");
        return -1;
    }
    if (fd->offset == fd->size)
        goto DONE;

    /* fix file length in directory entry */
    dir->dirent->StartCluster = fd->dir_clus;
    rc = dirent_get (dir, dirent, fd->dire_index);
    if(rc<0){
        ERROR ("dirent_get()\n");
        return rc; //Read failed
    }
    if ( rc== NULL) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* reset file size */
    dirent->Length = fd->offset;
    rc = dirent_set (dir, dirent, dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_set(), rc=%d\n", rc);
        return rc;
    }

    /* NOT update fat chain */

    fd->size = fd->offset;

DONE:
    fd->magic = 0;
    return 0;
}

int
_remove (const char *name)
{
    struct dir dir[1];
    struct dirent dirent[1];
    struct fd32 fd[1];
    char pathbuf[PATHLEN + 1], *dirname, *filename;
    int cluster;
    int rc;
    char sig;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fat->is_readonly) {
        ERROR("Can't delete file on a readonly filesystem\n");
        return -E_FS_ACCESS_DENY;
    }

    /* extract dirname & basename */
    rc = dir_file_name (name, pathbuf, &dirname, &filename);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }
    cluster = dir->dirent->StartCluster;

    rc = dirent_find_by_name (dir, filename, dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    if (dirent->Attrib & ATTRIB_PROTECT) {
        ERROR ("E_FILE_PROTECT\n");
        return -E_FILE_PROTECT;
    }

    sig = dirent->signature;

    /* init */
    memset (fd, 0, sizeof (struct fd32));
    fd->magic = MAGIC_FD;
    fd->mnt_cnt = fat->mnt_cnt;
    fd->size = dirent->Length;

    /* load region */
    rc = fd_region_load (fd, dirent->StartCluster, dirent->Length);
    if (rc < 0) {
        WARN ("fat_load_fd(), rc=%d\n", rc);
    }

    /*** manage double linked list ***/
    /* update "." dirent - keep head, tail */
    if (dir->head == dirent->dire_index) {
        dir->head = dirent->dire_next;
    }
    if (dir->tail == dirent->dire_index) {
        dir->tail = dirent->dire_prev;
    }
    if (dir->dirent->StartCluster != FIRST_ROOTCLUSTER) {
        int total = dir->total - 1;
        rc = dirent_update (dir, 0, dir->head, dir->tail, total);
        if (rc < 0) {
            ERROR ("dirent_update(), rc=%d\n", rc);
            return rc;
        }
    }
    /* update previsou */
    if (dirent->dire_prev != LINK_NULL) {
        rc = dirent_update (dir, dirent->dire_prev, -1, dirent->dire_next, 0);
        if (rc < 0) {
            ERROR ("dirent_update(), rc=%d\n", rc);
            return rc;
        }
    }
    /* update next */
    if (dirent->dire_next != LINK_NULL) {
        rc = dirent_update (dir, dirent->dire_next, dirent->dire_prev, -1, 0);
        if (rc < 0) {
            ERROR ("dirent_update(), rc=%d\n", rc);
            return rc;
        }
    }
    /* delete current */
    rc = dirent_del (dir, dirent, dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_del(), rc=%d\n", rc);
        return rc;
    }


    if (! IS_RSV_SIG(sig)) {
        /* delete fat chain */
        rc = fate_shrink (dirent->StartCluster, 0);
        if (rc < 0) {
            ERROR ("fate_shrink(), rc=%d\n", rc);
            return rc;
        }
    }

    return 0;
}

int
_rename (const char *old_name, const char *new_name)
{
    /* old file */
    struct dir old_dir[1];
    struct dirent old_dirent[1];
    char old_pathbuf[PATHLEN + 1], *old_dname, *old_fname;

    /* new file */
    struct dir new_dir[1];
    struct dirent new_dirent[1];
    char new_pathbuf[PATHLEN + 1], *new_dname, *new_fname;

    int same_dir;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fat->is_readonly) {
        ERROR("Can't rename file on a readonly filesystem\n");
        return -E_FS_ACCESS_DENY;
    }

    /* extract old dname & fname */
    rc = dir_file_name (old_name, old_pathbuf, &old_dname, &old_fname);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    rc = dir_open (old_dir, old_dname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }

    rc = dirent_find_by_name (old_dir, old_fname, old_dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* extract new dname & fname */
    rc = dir_file_name (new_name, new_pathbuf, &new_dname, &new_fname);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }
    if (!new_fname)
        new_fname = old_fname;

    if (old_dname != NULL && new_dname != NULL) {
        same_dir = (strcmp (old_dname, new_dname) == 0);
    } else {
        same_dir = (old_dname == NULL && new_dname == NULL);
    }
    if (!same_dir) {
        rc = dir_open (new_dir, new_dname);
        if (rc < 0) {
            ERROR ("dir_open(), rc=%d\n", rc);
            return rc;
        }
    } else {
        if (!strcmp (old_fname, new_fname)) {
            /* src == dst */
            return 0;
        }

        memcpy (new_dir, old_dir, sizeof (struct dir));
    }

    /* create new direntory entry */
    memcpy (new_dirent, old_dirent, sizeof (struct dirent));
    rc = dirent_name (new_dirent, new_fname);
    if (rc < 0) {
        ERROR ("dirent_name(), rc=%d\n", rc);
        return rc;
    }

    rc = dirent_add (new_dir, new_dirent);
    if (rc < 0) {
        ERROR ("dirent_add(), rc=%d\n", rc);
        return rc;
    }

    /* delete old direntory entry */
    rc = dirent_del (old_dir, old_dirent, old_dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_del(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

int
_recycle (const char *old_name, const char *new_name)
{
    /* old file */
    struct dir old_dir[1];
    struct dirent old_dirent[1];
    char old_pathbuf[PATHLEN + 1], *old_dname, *old_fname;

    /* new file */
    struct dir new_dir[1];
    struct dirent new_dirent[1];
    char new_pathbuf[PATHLEN + 1], *new_dname, *new_fname;

    char buf[SECT_SIZE * 4];
    int sector;

    int same_dir;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fat->is_readonly) {
        ERROR("Can't recycle file on a readonly filesystem\n");
        return -E_FS_ACCESS_DENY;
    }

    /* extract old dname & fname */
    rc = dir_file_name (old_name, old_pathbuf, &old_dname, &old_fname);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    rc = dir_open (old_dir, old_dname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }

    rc = dirent_find_by_name (old_dir, old_fname, old_dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* extract new dname & fname */
    rc = dir_file_name (new_name, new_pathbuf, &new_dname, &new_fname);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }
    if (!new_fname)
        new_fname = old_fname;

    if (old_dname != NULL && new_dname != NULL) {
        same_dir = (strcmp (old_dname, new_dname) == 0);
    } else {
        same_dir = (old_dname == NULL && new_dname == NULL);
    }
    if (!same_dir) {
        rc = dir_open (new_dir, new_dname);
        if (rc < 0) {
            ERROR ("dir_open(), rc=%d\n", rc);
            return rc;
        }
    } else {
        if (!strcmp (old_fname, new_fname)) {
            /* src == dst */
            return 0;
        }

        memcpy (new_dir, old_dir, sizeof (struct dir));
    }

    /* create new direntory entry */
    memcpy (new_dirent, old_dirent, sizeof (struct dirent));
    rc = dirent_name (new_dirent, new_fname);
    if (rc < 0) {
        ERROR ("dirent_name(), rc=%d\n", rc);
        return rc;
    }

    rc = dirent_add (new_dir, new_dirent);
    if (rc < 0) {
        ERROR ("dirent_add(), rc=%d\n", rc);
        return rc;
    }

    /* empty first sector */
    sector = CLUS2SECT(new_dirent->StartCluster);
    memset (buf, 0, SECT_SIZE * 4);
    rc = ll_write (sector, 4, buf);
    if (rc < 0) {
        ERROR ("ll_write(), rc=%d\n", rc);
        return rc;
    }

    /* delete old direntory entry */
    rc = dirent_del (old_dir, old_dirent, old_dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_del(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

int
_read (struct fd32 *fd, void *buf, unsigned int size)
{
    int sector, count;
    int part1, part2;
    char buffer[SECT_SIZE];
    void *ptr = buf;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if (fd->magic != MAGIC_FD) {
        ERROR ("E_FILE_BAD_FD\n");
        return -E_FILE_BAD_FD;
    }

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fat->mnt_cnt != fd->mnt_cnt) {
        ERROR ("E_FS_MOUNT_CNT\n");
        return -E_FS_MOUNT_CNT;
    }

    size = MIN (size, fd->size - fd->offset);

    part1 = fd->offset % SECT_SIZE;
    part2 = MIN (SECT_SIZE - part1, size);
    if (part1) {
        fd_cur_sector (fd, &sector, &count);

        rc = ll_read (sector, 1, buffer);
        if (rc < 0) {
            ERROR ("ll_read(), rc=%d\n", rc);
            return rc;
        }

        memcpy (ptr, buffer + part1, part2);

        fd->offset += part2;
        ptr += part2;
        size -= part2;
    }

    while (size > SECT_SIZE) {
        fd_cur_sector (fd, &sector, &count);
        count = MIN ((size / SECT_SIZE), count);
        rc = ll_read (sector, count, ptr);
        if (rc < 0) {
            ERROR ("ll_read(), rc=%d\n", rc);
            return rc;
        }

        fd->offset += (count * SECT_SIZE);
        ptr += (count * SECT_SIZE);
        size -= (count * SECT_SIZE);
    }

    part1 = size;
    part2 = SECT_SIZE - part1;
    if (part1) {
        fd_cur_sector (fd, &sector, &count);

        rc = ll_read (sector, 1, buffer);
        if (rc < 0) {
            ERROR ("ll_read(), rc=%d\n", rc);
            return rc;
        }

        memcpy (ptr, buffer, part1);

        fd->offset += part1;
        ptr += part1;
        size -= part1;
    }

    if (size != 0) {
        ERROR ("internal error\n");
        return -1;
    }
    return (ptr - buf);
}

int
_write (struct fd32 *fd, void *buf, unsigned int size)
{
    int sector, count;
    int part1, part2;
    char buffer[SECT_SIZE];
    void *ptr = buf;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if (fd->magic != MAGIC_FD) {
        ERROR ("E_FILE_BAD_FD\n");
        return -E_FILE_BAD_FD;
    }

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (fat->mnt_cnt != fd->mnt_cnt) {
        ERROR ("E_FS_MOUNT_CNT\n");
        return -E_FS_MOUNT_CNT;
    }

    size = MIN (size, fd->size - fd->offset);

    part1 = fd->offset % SECT_SIZE;
    part2 = MIN (SECT_SIZE - part1, size);
    if (part1) {
        fd_cur_sector (fd, &sector, &count);

        rc = ll_read (sector, 1, buffer);
        if (rc < 0) {
            ERROR ("ll_read(), rc=%d\n", rc);
            return rc;
        }

        memcpy (buffer + part1, ptr, part2);

        rc = ll_write (sector, 1, buffer);
        if (rc < 0) {
            ERROR ("ll_write(), rc=%d\n", rc);
            return rc;
        }

        fd->offset += part2;
        ptr += part2;
        size -= part2;
    }

    while (size > SECT_SIZE) {
        fd_cur_sector (fd, &sector, &count);
        count = MIN ((size / SECT_SIZE), count);
        rc = ll_write (sector, count, ptr);
        if (rc < 0) {
            ERROR ("ll_write(), rc=%d\n", rc);
            return rc;
        }

        fd->offset += (count * SECT_SIZE);
        ptr += (count * SECT_SIZE);
        size -= (count * SECT_SIZE);
    }

    part1 = size;
    part2 = SECT_SIZE - part1;
    if (part1) {
        fd_cur_sector (fd, &sector, &count);

        rc = ll_read (sector, 1, buffer);
        if (rc < 0) {
            ERROR ("ll_read(), rc=%d\n", rc);
            return rc;
        }

        memcpy (buffer, ptr, part1);

        rc = ll_write (sector, 1, buffer);
        if (rc < 0) {
            ERROR ("ll_write(), rc=%d\n", rc);
            return rc;
        }

        fd->offset += part1;
        ptr += part1;
        size -= part1;
    }

    if (size != 0) {
        ERROR ("internal error\n");
        return -1;
    }
    return (ptr - buf);
}

int
_seek (struct fd32 *fd, unsigned int offset)
{

    /* sanity check */
    ASSERT_UNDER_STACK();

    if (fd->magic != MAGIC_FD) {
        ERROR ("E_FILE_BAD_FD\n");
        return -E_FILE_BAD_FD;
    }

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    if (offset > fd->size) {
        ERROR ("E_FILE_OVERFLOW\n");
        return -E_FILE_OVERFLOW;
    }

    fd->offset = offset;
    return 0;
}

int
_stat (const char *name, struct stat_t *st)
{
    char pathbuf[PATHLEN + 1], *dirname, *filename;
    struct dir dir[1];
    struct dirent dirent[1];
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    /* extract dirname & basename */
    rc = dir_file_name (name, pathbuf, &dirname, &filename);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }
    /* get directory entry */
    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }

    rc = dirent_find_by_name (dir, filename, dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    st->st_size = dirent->Length;
    st->mtime = dirent->Time;
    st->mdate = dirent->Date;
    st->cluster = dirent->StartCluster;
    st->attrib = dirent->Attrib;
    st->attribute = dirent->Attribute;

    return 0;
}

int
_setattr (const char *name, int attribute)
{
    char pathbuf[PATHLEN + 1], *dirname, *filename;
    struct dir dir[1];
    struct dirent dirent[1];
    int cluster;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    /* extract dirname & basename */
    rc = dir_file_name (name, pathbuf, &dirname, &filename);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    /* get directory entry */
    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }
    cluster = dir->dirent->StartCluster;

    rc = dirent_find_by_name (dir, filename, dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* update flags */
    dirent->Attribute |= attribute;

    /* write new dirctory entries */
    rc = dirent_set (dir, dirent, dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_set(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}
int
_clearattr (const char *name, int attribute)
{
    char pathbuf[PATHLEN + 1], *dirname, *filename;
    struct dir dir[1];
    struct dirent dirent[1];
    int cluster;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    /* extract dirname & basename */
    rc = dir_file_name (name, pathbuf, &dirname, &filename);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    /* get directory entry */
    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }
    cluster = dir->dirent->StartCluster;

    rc = dirent_find_by_name (dir, filename, dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    /* update flags */
    dirent->Attribute &= ~attribute;

    /* write new dirctory entries */
    rc = dirent_set (dir, dirent, dirent->dire_index);
    if (rc < 0) {
        ERROR ("dirent_set(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}
int
hack_dll (const char *name, unsigned short prev, unsigned short next, int total)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char pathbuf[PATHLEN + 1], *dirname, *filename;
    int rc;

    /* sanity check */
    ASSERT_UNDER_STACK();

    if ((fat->mnt_cnt & 1) == 0 || fat->magic != MAGIC_FAT) {
        ERROR ("E_FS_NOT_MOUNT\n");
        return -E_FS_NOT_MOUNT;
    }

    /* extract dirname & basename */
    rc = dir_file_name (name, pathbuf, &dirname, &filename);
    if (rc < 0) {
        ERROR ("dir_file_name(), rc=%d\n", rc);
        return rc;
    }

    rc = dir_open (dir, dirname);
    if (rc < 0) {
        ERROR ("dir_open(), rc=%d\n", rc);
        return rc;
    }

    rc = dirent_find_by_name (dir, filename, dirent);
    if (rc < 0) {
        ERROR ("E_FILE_NOT_EXIST\n");
        return -E_FILE_NOT_EXIST;
    }

    rc = dirent_update (dir, dirent->dire_index, prev, next, total);
    if (rc < 0) {
        ERROR ("dirent_update(), rc=%d\n", rc);
        return rc;
    }

    return 0;
}

int
scan_dll (const char *dirname)
{
    struct dir dir[1];
    struct dirent dirent[1];
    int total, idx;
    unsigned short head, tail;
    int rc;

    rc = _opendir (dir, dirname);
    if (rc < 0) {
        ERROR ("_opendir, rc=%d\n", rc);
        return rc;
    }
    rc = dirent_get (dir, dirent, 0);
    if(rc<0){
        ERROR ("dirent_get()\n");
        return rc;
    }
    if (!rc) {
        ERROR ("cannot open \".\"\n");
        return -E_DIR_NOT_EXIST;
    }
    head = dirent->dire_prev;
    tail = dirent->dire_next;

    /* check total */
    total = 0;
    for (idx = 0 ; idx < DIRE_PER_DIR; idx += 4) {
        rc = dirent_get (dir, dirent, idx);
        if(rc<0){
            ERROR ("dirent_get()\n");
            return rc;
        }
        if (!(rc)){
            if (dirent->Name[0] == 0x00 || dirent->Name[0] == 0xff) {
            /* end of directory entry table */
                break;
            }
            continue;
        }

        if (!(dirent->Attrib & ATTRIB_ARCHIVE))
            continue;
        

        total++;
    }

    if (dir->total != total) {
        ERROR ("total mismatch %d != %d\n", dir->total, total);
        return -1;
    }

    /* move forward */
    total = 0;
    rc = dirent_get (dir, dirent, idx);
    if (rc < 0){
        ERROR ("dirent_get()\n");
        return rc;
    }
    for (idx = head; rc ; idx = dirent->dire_next) {
        rc = dirent_get (dir, dirent, idx);
        if (rc < 0)
            return rc;
        total++;
    }

    if (dir->total != total) {
        ERROR ("total mismatch %d != %d\n", dir->total, total);
        return -1;
    }

    /* move backward */
    total = 0;
    rc = dirent_get (dir, dirent, idx);
    if (rc < 0){
        ERROR ("dirent_get()\n");
        return rc;
    }
        
    for (idx = tail; rc ; idx = dirent->dire_prev) {
        rc = dirent_get (dir, dirent, idx);
        if (rc < 0){
            ERROR ("dirent_get()\n");
            return rc;
        }
        total++;
    }

    if (dir->total != total) {
        ERROR ("total mismatch %d != %d\n", dir->total, total);
        return -1;
    }

    _closedir (dir);
    return 0;
}

int
rebuild_dll (const char *dirname)
{
    struct dir dir[1];
    struct dirent dirent[1];
    char *buf = heap_alloc (sizeof (fileInfo_t) * 1024);
    fileInfo_t *files = (fileInfo_t *) buf;
    int idx, cnt;
    int rc;

    rc = _opendir (dir, dirname);
    if (rc < 0) {
        ERROR ("_opendir, rc=%d\n", rc);
        goto EXIT;
    }

    idx = 0;
    while ((_readdir (dir, dirent)) && idx < 1024) {
        if ((dirent->Attrib & ATTRIB_DIRECTORY))
            continue;
        fileInfo_t *file = &files[idx++];
        file->Date = dirent->Date;
        file->Time = dirent->Time;
        file->dire_index = dirent->dire_index;
    }
    if (idx == 0) {
        /* no files */
        goto EXIT;
    }

    cnt = idx;
    qsort (files, cnt, sizeof (fileInfo_t), compare_byTime);

    fileInfo_t *head = &files[0];
    fileInfo_t *tail = &files[cnt-1];
    for (idx = 0; idx < cnt; idx++) {
        unsigned short prev, next;
        fileInfo_t *curr = &files[idx];
        prev = (curr == head) ? LINK_NULL : files[idx-1].dire_index;
        next = (curr == tail) ? LINK_NULL : files[idx+1].dire_index;

        /* update each file */
        rc = dirent_update (dir, curr->dire_index, prev, next, 0);
        if (rc < 0) {
            ERROR ("dirent_update(), rc=%d\n", rc);
            goto EXIT;
        }
    }

    /* update "." */
    rc = dirent_update (dir, 0, head->dire_index, tail->dire_index, cnt);
    if (rc < 0) {
        ERROR ("dirent_update(), rc=%d\n", rc);
        goto EXIT;
    }

EXIT:
    _closedir (dir);
    heap_release (buf);
    return rc;
}

void
fat_check_tw_signature (int on)
{
    // fat->check_tw_signature = on;
}

int
fat_init (void)
{
    fat = gSYSC.pfat->fat;    

    memset (fat, 0, sizeof (struct fat32));
    fat->fate_cache_sector = -1;
    fat->is_readonly = 0;
    strcpy (fat->cur_dir, "/");

    return 0;
}

int
fat_drv_init (void)
{
    struct sysc_t *sys_ctx = &gSYSC;
    int rc;

    pfat = sys_ctx->pfat;
    rc = fat_init ();
    if (rc == 0) {
        pfat->root_start = pfat->fat->root_start;
        pfat->lock = 0;
    }

    /* init sd */
    sd_drv_init((void *)sys_ctx);

    return rc;
}
