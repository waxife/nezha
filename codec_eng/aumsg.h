/**
 *  @file   aumsg.h
 *  @brief  header file for audio message
 *  $Id: aumsg.h,v 1.5 2014/04/14 07:06:49 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.5 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2014/01/06  hugo    movied from audmsg.h
 *
 */
#ifndef __AUMSG_H
#define __AUMSG_H

#include <audio.h>

#include <nor.h>
#include <time.h>
#include <codec.h>
#include <fourcc.h>
#include <mconfig.h>
#include <config.h>

#define AUMSG_MAGIC    0xdeadbeef

#define MSG_LEN     (64*1024)
#define BUF_LEN     CONFIG_AUMSG_BUFDES_SIZE
#define BUF_NUM     (MSG_LEN / BUF_LEN)
#define DES_NUM     4

#if (CONFIG_AUMSG_CODEC_FORMAT == 0)
#define AUMSG_DATARATE 2
#elif (CONFIG_AUMSG_CODEC_FORMAT == 1)
#define AUMSG_DATARATE 16
#elif (CONFIG_AUMSG_CODEC_FORMAT == 2)
#define AUMSG_DATARATE 4
#elif (CONFIG_AUMSG_CODEC_FORMAT == 3)
#define AUMSG_DATARATE 4
#elif (CONFIG_AUMSG_CODEC_FORMAT == 4)
#define AUMSG_DATARATE 4
#else
#error "unknown audio codec format"
#endif

#define FLAG_UNREAD_INDEX   0
#define FLAG_UNREAD_MASK    (1 << FLAG_UNREAD_INDEX)

/* aumsg buffer descriptor */
struct bd {
    unsigned int address;
    unsigned int offset;
};

/* aumsg context */
struct aumsg_ctx {
    int state;
    char *filename;
    FS fs;

    cmd_callback_t cmd_callback;

    void *buffer;
    int length;
    int offset;
    struct bd bd_list[BUF_NUM];
    int bd_number;
    int bd_head;
    int bd_tail;

    struct au_ctx_t au[1];
};

struct wave_header {
    /* Wave File Header */
    unsigned int ChunkID;
    unsigned int ChunkSize;
    unsigned int Format;

    /* Format Chunk */
    unsigned int Subchunk1ID;
    unsigned int Subchunk1Size;
    unsigned short AudioFormat;
    unsigned short NumChannels;
    unsigned int SampleRate;
    unsigned int ByteRate;
    unsigned short BlockAlign;
    unsigned short BitsPerSample;

    /* Data Chunk */
    unsigned int Subchunk2ID;
    unsigned int Subchunk2Size;
    unsigned char data[0];
};
#define WHDR_SIZE   (sizeof (struct wave_header))

struct eau_opt_t {
    FS dev;
    MEDIA media;
    int format;
};

struct dau_opt_t {
    FS dev;
    MEDIA media;
    //int format;
};

struct aumsg_property {
    time_t  ctime;
    int duration; //unit: millisecond
    int length;
    char unread;

    /* audio time field */
    int year;
    int mon;
    int mday;
    int hour;
    int min;
    int sec;
};

/* codec */
int enc_aumsg (char *name, struct eau_opt_t *opt, cmd_callback_t cmd_callback);
int encau_status (struct sysc_status_t *status);

int dec_aumsg (char *name, struct dau_opt_t *opt, cmd_callback_t cmd_callback);
int decau_status (struct sysc_status_t *status);

int aumsg_get_info (int fs, char *name, struct aumsg_property *property);
int aumsg_get_property (int fs, MEDIA media, char *fname, struct aumsg_property *property);

/* database */
int nor_audiodb_mark_read (int idx);
int nor_audiodb_mark_delete (int idx);
int nor_audiodb_scan (time_t *ctimes);

#endif /* __AUMSG_H */
