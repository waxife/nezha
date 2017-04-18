/**
 *  @file   audio.h
 *  @brief  header file of audio encode/decode driver
 *  $Id: audio.h,v 1.8 2014/04/29 01:41:42 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.8 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/09/03  hugo     New file.
 *
 */
#ifndef __AUDIO_H
#define __AUDIO_H

#include "../drivers/audio/audio.T582B.h"

/* Audio I/O ioctl */
enum {
    AUGIFMT = 1,    /* get input format */
    AUSIFMT,        /* set input format */
    AUGOFMT,        /* get output format */
    AUSOFMT,        /* set output format */
    AUGIPAT,        /* get input test pattern */
    AUSIPAT,        /* set input test pattern */
    AUGOPAT,        /* get output test pattern */
    AUSOPAT,        /* set output test pattern */
    AUGIGAIN,       /* get input gain */
    AUSIGAIN,       /* set input gain */
    AUGOGAIN,       /* get output gain */
    AUSOGAIN,       /* set output gain */
    AURESET,        /* reset device */
    AUPAUSE,        /* pause device */
    AURESUME,       /* resume device */
    AUFLUSH,        /* flush device */
};

/* Audio Format */
typedef enum {
    AU_G726_16 = 0, /* G.726 16kbit/s ADPCM */
    AU_PCM,         /* PCM (16bits signed) */
    AU_U_LAW,       /* u-law */
    AU_A_LAW,       /* a-law */
    AU_G726_32,     /* G.726 32kbit/s ADPCM */
} AU_FORMAT;

#define AU_MODE_WRONLY      AU_CMD_DEC
#define AU_MODE_RDONLY      AU_CMD_ENC
#define AU_MODE_RDRW        AU_CMD_FULL_DUPLEX

/* buildin pattern */
#define PAT_DISABLE         (0 << 3)
#define PAT_ENABLE          (1 << 3)
#define PAT_256SIN          (PAT_ENABLE | 0)
#define PAT_1KSIN           (PAT_ENABLE | 1)
#define PAT_0x8000          (PAT_ENABLE | 2)
#define PAT_0x7FFF          (PAT_ENABLE | 3)
#define PAT_0x8000_0x7FFF   (PAT_ENABLE | 4)
#define PAT_0x0000          (PAT_ENABLE | 5)

/******************************************************************************
 * Audio Driver API
 ******************************************************************************/

extern char audio_sample_rate[];

int audio_init (void);
int audio_config (struct au_ctx_t *au);
int audio_open (struct au_ctx_t *au, int mode);
int audio_close (struct au_ctx_t *au);
int audio_write_nonblock (struct au_ctx_t *au, const void *buf, int count);
int audio_write (struct au_ctx_t *au, const void *buf, int count);
int audio_read_nonblock (struct au_ctx_t *au, void *buf, int count);
int audio_read (struct au_ctx_t *au, void *buf, int count);
int audio_ioctl (struct au_ctx_t *au, int request, void *arg);
/* audio for ring bell function */
int audio_dec_irq(struct au_ctx_t *au);
int audio_init_doorbell_buffer(struct au_ctx_t *au);
void audio_doorbell_close(struct au_ctx_t *au);
#endif /* __AUDIO_H */
