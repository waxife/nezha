/**
 *  @file   enc_process.c
 *  @brief  encode process simulation for Cheetah project
 *  $Id: recfile.h,v 1.24 2015/03/18 10:01:35 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.24 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2010/09/09  sherman New file.
 *
 */

#ifndef _RECFILE_H_
#define _RECFILE_H_

#include <sys.h>
#include <dma.h>
#include "cvbs.h"
#include "enc_common.h"
#include "frm_header.h"
#include "mconfig.h"


#define ENC_SIMULATION          0
#define ENC_MAGIC               0x8713ac38

#ifdef RECORD_AUDIO
#undef RECORD_AUDIO
#endif
#define RECORD_AUDIO            1

#ifdef SYNC_ADJUST_AUDIO
#undef SYNC_ADJUST_AUDIO
#endif /* SYNC_ADJUST_AUDIO */
#define SYNC_ADJUST_AUDIO       0

#ifdef MAX_LLD_NUM
#undef MAX_LLD_NUM
#endif
#define MAX_LLD_NUM             28

#if ENC_SIMULATION
#define MAX_ENC_SHUT            2048
#endif

#define LL2C_DMAFF_TH(ll)   ((((ll) >> 29) & 0x7) << 24)
#define LL2C_SRC_WIDTH(ll)  ((((ll) >> 25) & 0x7) << 11)
#define LL2C_DST_WIDTH(ll)  ((((ll) >> 22) & 0x7) << 8)
#define LL2C_SRCAD_CTL(ll)  ((((ll) >> 20) & 0x3) << 5)
#define LL2C_DSTAD_CTL(ll)  ((((ll) >> 18) & 0x3) << 3)

#define LLP2CSR(xll)  LL2C_DMAFF_TH(xll) | LL2C_TCMASK(xll) | LL2C_SRC_WIDTH(xll) | \
                      LL2C_DST_WIDTH(xll) | LL2C_SRCAD_CTL(xll) |  LL2C_DSTAD_CTL(xll) | \
                      LL2C_SRC_SEL(xll) | LL2C_DST_SEL(xll)
#define LLPC_TCMASK     (1 << 28)

#define SDC_DATA_WINDOW             0xba800040

#ifdef MAX_VATOM
#undef MAX_VATOM
#endif

#ifdef MAX_AATOM
#undef MAX_AATOM
#endif

/* buffer number definition */
#define MAX_VATOM   15  /* max free video atom */
#define MAX_AATOM   4   /* max free audio atom */

struct aud_t {
    list_t          l;
    uint8_t         *buf;
};

struct lld_t {
    uint32_t    srcaddr;
    uint32_t    dstaddr;
    uint32_t    llp;
    uint32_t    control;
    uint32_t    totsize;
};

struct md_atom_t {
    uint16_t        reserved1:3;
    uint16_t        offset:13;
    uint16_t        reserved2:3;
    uint16_t        fill_size:13;
    uint8_t         *buf;
};


/* state for write sd */
#define IDLE_ST         0
#define BUSY_TRN        1
#define BUSY_RCV        2
#define BUSY_PRG        3


struct enc_t {
    uint32_t        magic;
    void            *free;                  /* free atom linked list for video atom */
    void            *afree;                 /* free atom linked list for audio atom */
    struct atom_t   *frmlt;                 /* frame list             frmlt->atom->atom-> ... (frmlt_pnext)-> NULL  */
    struct atom_t   **frmlt_pnext;          /* next pointer of last element */
    struct atom_t   *w_frmlt;               /* writable frame list    w_frmlt->atom->atom->h-> ... (w_frmlt_pnext)->NULL */
    struct atom_t   **w_frmlt_pnext;
    struct atom_t   *wring;                 /* on going write list    wring ->atom ->atom->h .... (wrfrmlt)->atom-> ... ->NULL */
    void            *hdr_free;              /* free header list */
    uint32_t        wsize;                  /* wsize = size of w_frmlt */
    uint32_t        frmsize;                /* frmsize = size of frmlt, current frame size */
    uint32_t        datsize;
    uint16_t        frm_w;
    uint16_t        frm_h;
    int             standard;               /* VS_NTSC, VS_PAL, VS_C656I, VS_C656P */

    /* fs2 handle */
    struct fat_t    *fat;
    struct fs2_t    *fs2;
    void            *sd_ptr;

    time_t          max_time;
    int             max_fno;
    int             max_fsize;

    /* from st_t */
    uint8_t         state;
    uint8_t         field_id;
    uint8_t         img_idx;
    uint8_t         w_img_idx;
    uint8_t         bd_idx;
    time_t          start_time;
    time_t          curr_time;
    uint32_t        write_frms;

    /* vshut, w_frmlt indexs */
    uint8_t         v_shut_idx_head;    /* v_shut FIFO */
    uint8_t         v_shut_idx_tail;
    uint8_t         v_shut_full;        /* full flag */
    uint8_t         w_busy;             /* IDLE_ST = 0, (CMD25 ->) BUSY_TRN = 1, (DMA_DONE ->) BUSY_RCV = 2, (STATUS_DATA_END ->) BUSY_PRG = 3 (SD_CARD_READY(xstatus) ->) IDLE_ST */
    uint8_t         last_w_busy[20];
    uint8_t         v_shut_idx[20][3];

    uint8_t         w_num;              /* number of atom in wring */
    uint8_t         wi;                 /* index of done wring, once all atom is written, wi == w_num */
    uint8_t         enc_bds;
#if RECORD_AUDIO
    uint8_t         aud_bds;
    uint8_t         a_shut_idx_head;
    uint8_t         a_shut_idx_tail;
    uint8_t         a_shut_full;
    uint32_t        audata_size;
#endif /* RECORD_AUDIO */
    uint8_t         quality;            /* quality(0 ~ 100) */
    uint8_t         stop_isr;           /* stop isr flag */

    uint8_t         audio_fmt;          /* audio format: G726_16, PCM, U_LAW, A_LAW, G726_32 */
    uint8_t         fa_apll;            /* default fa parameter value of audio pll */
#if SYNC_ADJUST_AUDIO
    uint8_t         i2s_num_last;       /* last value of i2s played samples from hw */
    uint16_t        i2s_num;            /* number of i2s played samples from last getting audio atom */
#endif /* SYNC_ADJUST_AUDIO */
    uint32_t        avdiff_last;        /* last av diff value */
    uint32_t        au_target_ticks;    /* target ticks value to change plld */

    struct atom_t   *v_shut[BD_NUM];    /* video encode shuttle */
#if RECORD_AUDIO
    struct aud_t    *a_shut[A_BD_NUM];
#endif /* RECORD_AUDIO */
    struct lld_t    lld[MAX_LLD_NUM];

    uint16_t        img_w;
    uint16_t        img_h;
    uint16_t        img_h_real;
    uint32_t        fps;                /* video format: fps */
    uint8_t         reserved;
    int             err;
    uint16_t        loop_cnt;           /* loop counter for avoiding infinite loop */

    /* motion detection */
    //struct md_atom_t *md_atom;

    /* sd device variable */
    drive_op_t      *sd_op;

    int8_t          force_sd_flush;
    uint32_t        max_frame_num;
    uint32_t        max_file_size;
    uint32_t        wfno;
    int8_t          reset_quality;

    int8_t          skip_avih;
    uint32_t        exp_wfno;
    int8_t          skip_frames;

#if RECORD_AUDIO
    struct aud_t    auds[MAX_AATOM];
#endif
    struct atom_t   atoms[MAX_VATOM];   /* atom entity */
};


#define DUMP_STATISTIC      0
#if DUMP_STATISTIC
struct recfile_statistic {
#if RECORD_AUDIO
    unsigned int au_fifo_overflow;
#endif /* RECORD_AUDIO */
    unsigned int sd_flush;
    unsigned int force_sd_flush;
    unsigned int au_frames;
    unsigned int release_cur_frame;
    unsigned int no_free_hdr;
    unsigned int no_write_encbds;
    unsigned int rcv_0bds;
    unsigned int irq_num[16];
    unsigned int dummy_irq_num[16];
    unsigned int drop_frames;
    unsigned int img_idx_err;
    unsigned int bd_idx_err;
    unsigned int field_id_err;
    unsigned int frmsize_over_atomsize;
    unsigned int no_free_atom;
    unsigned int vid_frame_start;
    unsigned int vid_frame_end;
};
extern struct recfile_statistic recfile_st[1];
#endif /* DUMP_STATISTIC */

struct rec_opt_t {
    MEDIA media;        /* media config, default is MEDIA_VIDEO*/
    int width;          /* video frame width */
    int height;         /* vide frame height */
    int standard;       /* frame rate */
    int time;           /* max time */
    int fno;            /* max vfno */
    int size;           /* max frame size */
    int quality;        /* encode quality, 0 ~ 100 */
    int8_t fps_ratio;   /* 1 / 2^fps_ratio frame rate. only support 0 , 1 (1/2 fps), 2 (1/4 fps), and 3 (1/8 fps) */
    int8_t skip_avih;   /* skip AVI header */
    char *user_info;    /* user info data */
    unsigned char user_info_len;
} __attribute__ ((__packed__, aligned (4)));

/**
 * @func    recfile
 * @brief   record video file.
 * @param   fname           file name
 *          rec_opt_t       record option
 *          cmd_callback    command callback function return command to codec
 * @return  0 success, or return negative error code.
 */
int recfile(const char *fname, struct rec_opt_t *rec_opt, int (*cmd_callback)(void **arg));

/**
 * @func    recfile_status
 * @brief   get record file status.
 * @param   status          status data structure is declared in sys/include/sys.h
 * @return  0 success, or return negative error code.
 */
int recfile_status(struct sysc_status_t *status);

/**
 * @func    mk_recfile_hdr
 * @brief   make record file AVI header for speed up record initial time
 * @param   fname           file name
 *          rec_opt_t       record option
 * @return  0 success, or return negative error code.
 */
int mk_recfile_hdr(const char *fname, struct rec_opt_t *rec_opt);

#endif /* _RECFILE_H_ */
