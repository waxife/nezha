/* 
 *  @file   avifmt.h
 *  @brief  header file of avi format
 *
 *  $Id: avifmt.h,v 1.4 2014/07/17 05:52:28 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/01/28  jonathan  New file.
 *
 */

#ifndef _AVIFMT_H_
#define _AVIFMT_H_


/* 4 bytes */
typedef unsigned short WORD;
typedef unsigned int DWORD;
/* 1 byte */
typedef char BYTE;

#define FRAME_WIDTH_CMOS     640
#define FRAME_HEIGHT_CMOS    480

#define FRAME_WIDTH_CVD      640
#define FRAME_HEIGHT_CVD     240

/* for use in AVI_avih.flags */
#define AVIF_HASINDEX       0x00000010  /* index at end of file */
#define AVIF_MUSTUSEINDEX   0x00000020
#define AVIF_ISINTERLEAVED  0x00000100
#define AVIF_TRUSTCKTYPE    0x00000800
#define AVIF_WASCAPTUREFILE 0x00010000
#define AVIF_COPYRIGHTED    0x00020000


#define AVI_avih_SIZE   56
struct AVI_avih 
{
  DWORD us_per_frame;   /* frame display rate (or 0L) */
  DWORD max_bytes_per_sec;  /* max. transfer rate */
  DWORD padding;    /* pad to multiples of this size; */
  /* normally 2K */
  DWORD flags;
  DWORD tot_frames; /* # frames in file */
  DWORD init_frames;
  DWORD streams;
  DWORD buff_sz;  
  DWORD width;
  DWORD height;
  DWORD reserved[4];
};


#define AVI_strh_SIZE   48
struct AVI_strh
{
  unsigned char type[4];      /* stream type */
  unsigned char handler[4];
  DWORD flags;
  DWORD priority;
  DWORD init_frames;       /* initial frames (???) */
  DWORD scale;
  DWORD rate;
  DWORD start;
  DWORD length;
  DWORD buff_sz;           /* suggested buffer size */
  DWORD quality;
  DWORD sample_sz;
};

struct AVI_a_strh
{
  unsigned char type[4];      /* stream type */
  DWORD fccHandler;
  DWORD flags;
  DWORD priority;
  DWORD init_frames;       /* initial frames (???) */
  DWORD scale;
  DWORD rate;
  DWORD start;
  DWORD length;
  DWORD buff_sz;           /* suggested buffer size */
  DWORD quality;
  DWORD sample_sz;
};



#define AVI_strf_SIZE   40
struct AVI_strf
{       
  DWORD sz;
  DWORD width;
  DWORD height;
  DWORD planes_bit_cnt;
  unsigned char compression[4];
  DWORD image_sz;
  DWORD xpels_meter;
  DWORD ypels_meter;
  DWORD num_colors;        /* used colors */
  DWORD imp_colors;        /* important colors */
  /* may be more for some codecs */
};

#define AVI_a_strf_SIZE   20
struct AVI_a_strf
{       
  unsigned short  wFormatTag;   /* TwoCC format */
#define ID_PCM      0x0001
#define ID_ALAW     0x0006
#define ID_MULAW    0x0007
#define ID_G726     0x0045

  unsigned short  nChannels;
  unsigned int    nSamplesPerSec;
  unsigned int    nAvgBytesPerSec;
  unsigned short  nBlockAlign;
  unsigned short  wBitsPerSample;
  unsigned short  cbSize;
  unsigned short  junk;
};


/*
  AVI_list_hdr

  spc: a very ubiquitous AVI struct, used in list structures
       to specify type and length
*/

/* size: 12 */
struct AVI_list_hdr 
{
  unsigned char id[4];   /* "LIST" */
  DWORD sz;              /* size of owning struct minus 8 */
  unsigned char type[4]; /* type of list */
};


/* size: 24 = 12 + 12 */
struct AVI_list_odml 
{
  struct AVI_list_hdr list_hdr;

  unsigned char id[4];
  DWORD sz;
  DWORD frames;
};


/* size: 236 = (12 + 56 + 48) + (12 + 56 +28) + 24 */
struct AVI_list_strl 
{
  struct AVI_list_hdr list_hdr;
  
  /* video chunk strh(56) */
  unsigned char strh_id[4];
  DWORD strh_sz;
  struct AVI_strh strh;

  /* video chunk strf(48) */
  unsigned char strf_id[4];
  DWORD strf_sz;
  struct AVI_strf strf;
  
  struct AVI_list_hdr list_a_hdr;
  
  /* audio chunk strh(56) */
  unsigned char strh_a_id[4];
  DWORD strh_a_sz;
  struct AVI_a_strh strh_a;

  /* audio chunk strf(26) */
  unsigned char strf_a_id[4];
  DWORD strf_a_sz;
  struct AVI_a_strf strf_a;

  /* list odml */
  struct AVI_list_odml list_odml;
};


/* size: 216 = 12 + 8 + 56 + 140 */
/* size: 312 = 12 + 8 + 56 + 236 */
struct AVI_list_hdrl 
{
  struct AVI_list_hdr list_hdr;

  /* chunk avih */
  unsigned char avih_id[4];
  DWORD avih_sz;
  struct AVI_avih avih;
  
  /* list strl */
  struct AVI_list_strl strl;
};


/* size: 12 */
struct AVI_list_movi 
{
  struct AVI_list_hdr list_hdr;

  /* chunk movi */
  /*   add frames here ... */
};


/* size: 1024 = 12 + 310 + 8 + 682 + 12 */
#define JUNK_SIZE   (1024 - 12 - 312 - 8 - 12)
struct AVI_riff 
{
  unsigned char id[4];   /* "RIFF" */
  DWORD sz;              /* size of total file size minus 8 */
  unsigned char type[4]; /* "AVI " */

  struct AVI_list_hdrl hdrl;

  /* chunk JUNK */
  unsigned char junk_id[4];  /* "JUNK" */
  DWORD junk_sz;             /* size of junk data */
  unsigned char junk_data[JUNK_SIZE]; /* junk data */

  /* list movi */
  struct AVI_list_hdr movi;

};

int avi_header_copy2(uint8_t *buf, uint32_t *size, uint32_t width, uint32_t height, uint32_t fps, int audio_fmt);

#endif
