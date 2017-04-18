/**
 *  @file   sh_audio.c
 *  @brief  audio test command
 *  $Id: sh_audio.c,v 1.8 2014/03/13 08:16:59 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/09/03  hugo        New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <shell.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <serial.h>
#include <keymap.h>
#include <ctype.h>
#include <audio.h>
#include <norfs.h>
#include <debug.h>
#include <heap.h>

command_init (sh_audio_iformat, "audio_iformat", "audio_iformat [<idx>]");
command_init (sh_audio_oformat, "audio_oformat", "audio_oformat [<idx>]");
command_init (sh_audio_ipattern, "audio_ipattern", "audio_ipattern [<idx>]");
command_init (sh_audio_opattern, "audio_opattern", "audio_opattern [<idx>]");
command_init (sh_audio_igain, "audio_igain", "audio_igain [<idx>]");
command_init (sh_audio_ogain, "audio_ogain", "audio_ogrin [<idx>]");
command_init (sh_audio_record, "audio_record", "audio_record <filename>");
command_init (sh_audio_play, "audio_play", "audio_play <filename>");

#define BUFSIZ  512

KeyString ksFormat[] = {
    {0,     "G.726 16kbit"},
    {1,     "PCM"},
    {2,     "U-law"},
    {3,     "A-law"},
    {4,     "G726 32kbit"},
};
#define ksFormatSize    (sizeof(ksFormat)/sizeof(ksFormat[0]))

KeyString ksPattern[] = {
    {0,     "256Hz sin"},
    {1,     "1KHz sin"},
    {2,     "all 0x8000"},
    {3,     "all 0x7fff"},
    {4,     "interleave"},
    {5,     "all zero"},
    {6,     "disable"},
};
#define ksPatternSize   (sizeof(ksPattern)/sizeof(ksPattern[0]))

KeyString ksGain[] = {
    {0,     "0.25x"},
    {1,     "0.31x"},
    {2,     "0.37x"},
    {3,     "0.44x"},
    {4,     "0.52x"},
    {5,     "0.61x"},
    {6,     "0.70x"},
    {7,     "0.82x"},
    {8,     "1.00x"},
    {9,     "1.25x"},
    {10,    "1.44x"},
    {11,    "1.68x"},
    {12,    "1.96x"},
    {13,    "2.30x"},
    {14,    "2.52x"},
    {15,    "2.75x"},
};
#define ksGainSize      (sizeof(ksGain)/sizeof(ksGain[0]))

KeyValue kvFormat[] = {
    {0,     0},
    {1,     1},
    {2,     2},
    {3,     3},
    {4,     4},
};
#define kvFormatSize    (sizeof(kvFormat)/sizeof(kvFormat[0]))

KeyValue kvPattern[] = {
    {0,     PAT_256SIN},
    {1,     PAT_1KSIN},
    {2,     PAT_0x8000},
    {3,     PAT_0x7FFF},
    {4,     PAT_0x8000_0x7FFF},
    {5,     PAT_0x0000},
    {6,     PAT_DISABLE},
};
#define kvPatternSize   (sizeof(kvPattern)/sizeof(kvPattern[0]))

KeyValue kvGain[] = {
    {0,     0},
    {1,     1},
    {2,     2},
    {3,     3},
    {4,     4},
    {5,     5},
    {6,     6},
    {7,     7},
    {8,     8},
    {9,     9},
    {10,    10},
    {11,    11},
    {12,    12},
    {13,    13},
    {14,    14},
    {15,    15},
};
#define kvGainSize      (sizeof(kvGain)/sizeof(kvGain[0]))

static int
sh_audio_iformat (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    int key, val;
    char *str;

    /* set input format */
    if (argc > 1) {
        if (!isdigit(*argv[1]))
            goto EXIT;

        key = atoi (argv[1]);
        val = key2val (kvFormat, kvFormatSize, key);
        if (val < 0)
            goto EXIT;

        audio_ioctl (au, AUSIFMT, &val);
    }

    /* get input format */
    audio_ioctl (au, AUGIFMT, &val);
    key = val2key (kvFormat, kvFormatSize, val);
    str = key2str (ksFormat, ksFormatSize, key);
    printf ("input format: %d (%s)\n", key, str);
    return 0;

EXIT:
    print_usage (sh_audio_iformat);
    ksList (ksFormat, ksFormatSize);
    return -1;
}

static int
sh_audio_oformat (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    int key, val;
    char *str;

    /* set output format */
    if (argc > 1) {
        if (!isdigit(*argv[1]))
            goto EXIT;

        key = atoi (argv[1]);
        val = key2val (kvFormat, kvFormatSize, key);
        if (val < 0)
            goto EXIT;

        audio_ioctl (au, AUSOFMT, &val);
    }

    /* get output format */
    audio_ioctl (au, AUGOFMT, &val);
    key = val2key (kvFormat, kvFormatSize, val);
    str = key2str (ksFormat, ksFormatSize, key);
    printf ("output format: %d (%s)\n", key, str);
    return 0;

EXIT:
    print_usage (sh_audio_oformat);
    ksList (ksFormat, ksFormatSize);
    return -1;
}

static int
sh_audio_ipattern (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    int key, val;
    char *str;

    /* set input test pattern */
    if (argc > 1) {
        if (!isdigit(*argv[1]))
            goto EXIT;

        key = atoi (argv[1]);
        val = key2val (kvPattern, kvPatternSize, key);
        if  (val < 0)
            goto EXIT;

        audio_ioctl (au, AUSIPAT, &val);
    }

    /* get input format */
    audio_ioctl (au, AUGIPAT, &val);
    key = val2key (kvPattern, kvPatternSize, val);
    str = key2str (ksPattern, ksPatternSize, key);
    printf ("input test pattern: %d (%s)\n", key, str);
    return 0;

EXIT:
    print_usage (sh_audio_ipattern);
    ksList (ksPattern, ksPatternSize);
    return -1;
}

static int
sh_audio_opattern (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    int key, val;
    char *str;

    /* set output test pattern */
    if (argc > 1) {
        if (!isdigit(*argv[1]))
            goto EXIT;

        key = atoi (argv[1]);
        val = key2val (kvPattern, kvPatternSize, key);
        if  (val < 0)
            goto EXIT;

        audio_ioctl (au, AUSOPAT, &val);
    }

    /* get output format */
    audio_ioctl (au, AUGOPAT, &val);
    key = val2key (kvPattern, kvPatternSize, val);
    str = key2str (ksPattern, ksPatternSize, key);
    printf ("output test pattern: %d (%s)\n", key, str);
    return 0;

EXIT:
    print_usage (sh_audio_opattern);
    ksList (ksPattern, ksPatternSize);
    return -1;
}

static int
sh_audio_igain (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    int key, val;
    char *str;

    /* set input gain */
    if (argc > 1) {
        if (!isdigit(*argv[1]))
            goto EXIT;

        key = atoi (argv[1]);
        val = key2val (kvGain, kvGainSize, key);
        if (val < 0)
            goto EXIT;

        audio_ioctl (au, AUSIGAIN, &val);
    }

    /* get input gain */
    audio_ioctl (au, AUGIGAIN, &val);
    key = val2key (kvGain, kvGainSize, val);
    str = key2str (ksGain, ksGainSize, key);
    printf ("input gain: %d (%s)\n", key, str);
    return 0;

EXIT:
    print_usage (sh_audio_igain);
    ksList (ksGain, ksGainSize);
    return -1;
}

static int
sh_audio_ogain (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    int key, val;
    char *str;

    /* set output gain */
    if (argc > 1) {
        if (!isdigit(*argv[1]))
            goto EXIT;

        key = atoi (argv[1]);
        val = key2val (kvGain, kvGainSize, key);
        if (val < 0)
            goto EXIT;

        audio_ioctl (au, AUSOGAIN, &val);
    }

    /* get output gain */
    audio_ioctl (au, AUGOGAIN, &val);
    key = val2key (kvGain, kvGainSize, val);
    str = key2str (ksGain, ksGainSize, key);
    printf ("output gain: %d (%s)\n", key, str);
    return 0;

EXIT:
    print_usage (sh_audio_ogain);
    ksList (ksGain, ksGainSize);
    return -1;
}

#define BUFLEN  2048
#define BUFNUM  8

static int
sh_audio_record (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    struct norfs_fd nfd[1];
    char *name;
    char *buf = heap_alloc(CONFIG_NORFS_RECORD_MAX_SIZE+16);
    unsigned int offset = 0, length;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    /* config buffer descriptors */
    au->cfg.buf_len = CONFIG_AUMSG_BUFDES_SIZE;
    au->cfg.des_num = 4;
    au->cfg.format = AU_G726_32;

    /* read buffer */
    rc = audio_open (au, AU_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("audio_open(), rc=%d\n", rc);
        goto EXIT;
    }

    while (offset < CONFIG_NORFS_RECORD_MAX_SIZE) {
        rc = audio_read_nonblock (au, buf, CONFIG_NORFS_RECORD_MAX_SIZE);
        if (rc < 0) {
            ERROR ("audio_read_nonblock(), rc=%d\n", rc);
            goto EXIT;
        }
        offset = rc;

        if (getb2 () == 'q')
            break;
    }
    length = offset;
    printf (" (%d)\n", length);

    rc = audio_close (au);
    if (rc < 0) {
        ERROR ("audio_close(), rc=%d\n", rc);
        goto EXIT;
    }

    /* write buffer */
    rc = norfs_open (nfd, name, length, NOR_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_write (nfd, buf, length);
    if (rc < 0) {
        ERROR ("norfs_write(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_close (nfd);
    if (rc < 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        goto EXIT;
    }

    heap_release(buf);
    return 0;
EXIT:
    print_usage (sh_audio_record);
    heap_release(buf);
    return rc;
}

static int
sh_audio_play (int argc, char **argv, void **result, void *sys_ctx)
{
    struct au_ctx_t au[1];
    struct norfs_fd nfd[1];
    struct norfs_stat st[1];
    char *name;
    char *buf = heap_alloc(CONFIG_NORFS_RECORD_MAX_SIZE+16);
    unsigned int offset = 0, length = CONFIG_NORFS_RECORD_MAX_SIZE;
    int rc = 0;

    if (argc < 2)
        goto EXIT;
    name = argv[1];

    /* read buffer */
    rc = norfs_stat (name, st);
    if (rc < 0) {
        ERROR ("norfs_stat(), rc=%d\n", rc);
        goto EXIT;
    }
    length = st->st_size;

    rc = norfs_open (nfd, name, length, NOR_MODE_RDONLY);
    if (rc < 0) {
        ERROR ("norfs_open(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_read (nfd, buf, length);
    if (rc < 0) {
        ERROR ("norfs_read(), rc=%d\n", rc);
        goto EXIT;
    }

    rc = norfs_close (nfd);
    if (rc < 0) {
        ERROR ("norfs_close(), rc=%d\n", rc);
        goto EXIT;
    }

    /* config buffer descriptors */
    au->cfg.buf_len = CONFIG_AUMSG_BUFDES_SIZE;
    au->cfg.des_num = 4;
    au->cfg.format = AU_G726_32;

    /* write buffer */
    rc = audio_open (au, AU_MODE_WRONLY);
    if (rc < 0) {
        ERROR ("audio_open(), rc=%d\n", rc);
        goto EXIT;
    }

    while (offset < length) {
        rc = audio_write_nonblock (au, buf, length);
        if (rc < 0) {
            ERROR ("audio_write_nonblock(), rc=%d\n", rc);
            goto EXIT;
        }
        offset = rc;

        if (getb2 () == 'q')
            break;
    }
    length = offset;
    printf (" (%d)\n", length);

    rc = audio_close (au);
    if (rc < 0) {
        ERROR ("audio_close(), rc=%d\n", rc);
        goto EXIT;
    }

    heap_release(buf);
    return 0;
EXIT:
    print_usage (sh_audio_play);
    heap_release(buf);
    return rc;
}
