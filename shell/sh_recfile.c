/**
 *  @file   sh_recfile.c
 *  @brief  record unit test
 *  $Id: sh_recfile.c,v 1.36 2015/11/17 06:46:07 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.36 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <codec.h>
#include <event.h>
#include <cache.h>
#include <debug.h>
#include <mconfig.h>
#include <ls.h>
#include <drivers/input/keypad.h>
#include <codec_eng/recfile.h>
#include <codec_eng/ringbell.h>
#include <story.h>

command_init (sh_recfile_status, "recfile_status", "recfile_status");
command_init (sh_recfile, "recfile", "recfile [name] [-w <width>] [-h <height>] [-r <frame rate>] [-s <frame size>] [-q <quality>] [-t <time>] [-f <frame no>] [-v <video standard] [-i <user info>]");
command_init (sh_recls, "recls", "recls");
command_init (sh_total_frames, "total_frames", "total_frames [fname]");

static struct rec_opt_t rec_opt_default = {
    MEDIA_VIDEO,    /* media type */
    720,            /* width */
    288,            /* height */
    AVIN_CVBS_PAL,  /* video standard */
    0,              /* record time (sec) */
    0,              /* record frame numbers (0 is ignore) */
    40 * 1024,      /* maximum frame size (bytes) */
    60,             /* quality (from 10 to 100) */
    0,              /* (1 / 2^fps_ratio) frame rate. only support 0 , 1 (1/2 fps), 2 (1/4 fps), and 3 (1/8 fps) */
    0,              /* set 1 to skip AVI header, or 0 */
    NULL,           /* user info data */
    0               /* user info data length */
};

int rec_parse(const char *fname)
{
    struct fd32 fd[1];
    unsigned int total_frames = 0, vids_length = 0;
    int rc = 0;

    rc = _open(fd, fname, 0, FD_RDONLY);
    if (rc < 0) {
        printf("open file [%s] error, rc=%d\n", fname, rc);
        goto EXIT;
    }

    rc = _seek(fd, 0x30);
    if (rc != 0) {
        printf("seek error (%d)\n", rc);
        rc = -1;
        goto EXIT;
    }

    _read(fd, &total_frames, 4);

    rc = _seek(fd, 0x8C);
    if (rc != 0) {
        printf("seek error (%d)\n", rc);
        rc = -1;
        goto EXIT;
    }

    _read(fd, &vids_length, 4);

    if (vids_length != total_frames) {
        printf("frame number error: vids_lenght = %d, total_frames = %d\n",
               vids_length, total_frames);
        rc = -1;
    } else {
        rc = vids_length;
    }

EXIT:
    _close(fd);

    return rc;
}

static int sh_recfile_status(int argc, char **argv, void **result, void *sys_ctx)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = recfile_status(&status);
    if (rc < 0) {
        printf("Failed to get record file status. (%d)\n", rc);
        goto EXIT;
    }

    printf("state = %d\n", status.state);
    printf("fno = %d\n", status.fno);
    printf("time = %d\n", status.time);
    printf("quality = %d\n", status.quality);

EXIT:
    return rc;
}

void print_cmd(void)
{
    printf("command :\n");
    printf("s -> status.\n");
    printf("d -> delete.\n");
    printf("r -> ringbell.\n");
    printf("b -> stop ringbell\n");
    printf("h -> help\n");
}

#if DUMP_STATISTIC
void reclt_st(void)
{
    int i = 0;

    printf("[statistics] \n");
#if RECORD_AUDIO
    printf("\tsd_flush = %d (force_sd_flush = %d)\n",  recfile_st->sd_flush, recfile_st->force_sd_flush);
    printf("\tau_frames = %d\n", recfile_st->au_frames);
    printf("\trelease_cur_frame = %d\n", recfile_st->release_cur_frame);
    printf("\tau_fifo_overflow = %d\n", recfile_st->au_fifo_overflow);
#endif /* RECORD_AUDIO */
    printf("\tno_free_hdr = %d\n", recfile_st->no_free_hdr);
    printf("\tno_write_encbds = %d\n", recfile_st->no_write_encbds);
    printf("\trcv_0bds = %d\n", recfile_st->rcv_0bds);
    printf("\tdrop_frames = %d\n", recfile_st->drop_frames);
    printf("\timg_idx_err = %d\n", recfile_st->img_idx_err);
    printf("\tbd_idx_err = %d\n", recfile_st->bd_idx_err);
    printf("\tfield_id_err = %d\n", recfile_st->field_id_err);
    printf("\tfrmsize_over_atomsize = %d\n", recfile_st->frmsize_over_atomsize);
    printf("\tno_free_atom = %d\n", recfile_st->no_free_atom);
    printf("\tvid_frame_start = %d\n", recfile_st->vid_frame_start);
    printf("\tvid_frame_end = %d\n", recfile_st->vid_frame_end);
    printf("\tirq number = ");
    for (i = 0; i < sizeof(recfile_st->irq_num) / sizeof(recfile_st->irq_num[0]); i++) {
        if (recfile_st->irq_num[i] > 0)
            printf("[%d]%d (%d)  ", i, recfile_st->irq_num[i], recfile_st->dummy_irq_num[i]);
    }

    printf("\n");
}
#endif /* DUMP_STATISTIC */

unsigned int cb_cnt = 0;
static int rec_cmd_callback (void **arg)
{
    struct sysc_status_t status;
    static int last_time = -1;
    int cmd = 0;
    int ch = 0;
    int rc = 0;

    cb_cnt++;
    rc = recfile_status(&status);
    if (rc < 0) {
        printf("Failed to get record file status. (%d)\n", rc);
    } else {
        if (status.time != last_time) {
            last_time = status.time;
            printf("%d (total %d frames)\n", status.time, status.fno);
        }
    }

    ch = getb2();
    if (ch > 0) {
        switch (ch) {
        case 'q':
            cmd = CMD_QUIT;
            break;
        case 's':
            rc = recfile_status(&status);
            if (rc < 0) {
                printf("Failed to get record file status. (%d)\n", rc);
            } else {
                printf("state = %d\n", status.state);
                printf("fno = %d\n", status.fno);
                printf("time = %d\n", status.time);
                printf("quality = %d\n", status.quality);
            }

            break;
        case 'd':
            printf("send CMD_DELETE\n");
            cmd = CMD_DELETE;
            break;
        case 'r':
            printf("ringbell start\n");
            ringbell(0, 1);
            break;
        case 'b':
            printf("ringbell stop\n");
            stopbell();
            break;
        case 'h':
            print_cmd();
            break;
#if DUMP_STATISTIC
        case 'g':
            reclt_st();
            break;
#endif /* DUMP_STATISTIC */
        }
    }

    return cmd;
}

static int sh_recfile(int argc, char **argv, void **result, void *sys_ctx)
{
    struct rec_opt_t rec_opt;
    HLS *hls = NULL;
    char *fname = NULL, *pfname = NULL;
    struct sysc_status_t status;
    int rc = 0;
    int i = 0;
    char msgbuf[ERR_MSG_LEN];
    char *dirname;
    int nfiles = 0, maxsize = 0;

    cb_cnt = 0;
    memcpy(&rec_opt, &rec_opt_default, sizeof(struct rec_opt_t));
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'w':
                    rec_opt.width = atoi(argv[++i]);
                    break;
                case 'h':
                    rec_opt.height = atoi(argv[++i]);
                    break;
                case 'v':
                    rec_opt.standard = atoi(argv[++i]);
                    if (rec_opt.standard == AVIN_CVBS_PAL) {
                        rec_opt.width = 720;
                        rec_opt.height = 288;
                    } else if (rec_opt.standard == AVIN_CVBS_NTSC) {
                        rec_opt.width = 720;
                        rec_opt.height = 240;
                    } else {
                        printf("Invalid video standard %d. (%d for PAL, %d for NTSC)\n",
                               rec_opt.standard, AVIN_CVBS_PAL, AVIN_CVBS_NTSC);
                        goto EXIT;
                    }

                    break;
                case 's':
                    rec_opt.size = atoi(argv[++i]) * 1024;
                    break;
                case 't':
                    rec_opt.time = atoi(argv[++i]);
                    break;
                case 'f':
                    rec_opt.fno = atoi(argv[++i]);
                    break;
                case 'q':
                    rec_opt.quality = atoi(argv[++i]);
                    break;
                case 'r':
                    rec_opt.fps_ratio = atoi(argv[++i]);
                    if (rec_opt.fps_ratio < 0 || rec_opt.fps_ratio > 3) {
                        printf("Invalid frame rate ratio %d.\n", rec_opt.fps_ratio);
                        printf("Only support 0 (no change), 1 (1/2 fps), 2 (1/4 fps), and 3 (1/8 fps)\n");
                        goto EXIT;
                    }

                    break;
                case 'i':
                    rec_opt.user_info = argv[++i];
                    rec_opt.user_info_len = strlen(argv[i]); 
                    break;
                case 'm':
                    rec_opt.media = atoi(argv[++i]);
                    break;
            }
        } else {
            fname = argv[i];
        }
    }

    if (fname != NULL) {
        pfname = strchr(fname, '.');
        if (pfname == NULL || (strncmp(pfname + 1, "avi", 3) != 0)) {
            printf("ERROR: file name (%s) format <file.avi>\n", fname);
            goto EXIT;
        }
    } else {
        printf("ERROR: no file name\n");
        goto EXIT;
    }

    hls = openls(rec_opt.media, FATFS);
    media_get_config(FATFS, rec_opt.media, &dirname, &nfiles, &maxsize);

    pfname = fname;
    rc = getnewls(hls, &pfname, pfname, 1, 0, 0);
    if (rc < 0) {
        printf("Failed to generate record file name: %s.\n", fname);
        goto EXIT;
    }

    printf("Record [%s] %dx%d , standard %d, maximum frame size %d, user info %d bytes: (%s)\n",
           fname, rec_opt.width, rec_opt.height, rec_opt.standard, rec_opt.size,
           rec_opt.user_info_len, rec_opt.user_info);
    print_cmd();

#if DUMP_STATISTIC
    /* clear recfile statistics */
    memset(recfile_st, 0, sizeof(struct recfile_statistic));
#endif /* DUMP_STATITIC */

    rc = recfile(fname, &rec_opt, rec_cmd_callback);
    printf("recfile route command %d.\n", rc);
    if (rc == CMD_STOP_ERR) {
        printf("recfile return error command %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    }

#if DUMP_STATISTIC
    reclt_st();
#endif /* DUMP_STATISTIC */

    /* get end state */
    rc = recfile_status(&status);
    if (rc < 0) {
        printf("refile_status() return error %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    } else {
        printf("state = %d\n", status.state);
        printf("fno = %d\n", status.fno);
        printf("time = %d\n", status.time);
        printf("quality = %d\n", status.quality);
        STORY_SET(S_R0, status.fno);
        STORY_SET(S_R1, status.time);
    }

    /* get_err() again to test error number is cleared by get_err() */
    rc = get_err(msgbuf);
    if (rc) {
        printf("error number do NOT be clear after get_err.\n");
        printf("get error %d, message : %s\n", rc, msgbuf);
    }

EXIT:
    if (hls)
        closels(hls);

    printf("callback count = %d\n", cb_cnt);
    return rc;
}

static char quit = 0;
static unsigned char reset_dac_flag = 0;
extern int auc_dec_reset(void);

static int recls_cb(void **arg)
{
    struct sysc_status_t status;
    static int last_time = -1;
    static char get_time = 1;
    int cmd = 0;
    int ch = 0;
    int rc = 0;

    if (get_time) {
        rc = recfile_status(&status);
        if (rc < 0) {
            printf("Failed to get record file status. (%d)\n", rc);
        } else {
            if (status.time != last_time) {
                last_time = status.time;
                printf("%d (total %d frames)\n", status.time, status.fno);
            }
        }
    }

    ch = getb2();
    if (ch > 0) {
        switch (ch) {
        case 'q':
            cmd = CMD_QUIT;
            quit = 1;
            break;
        case 's':
            cmd = CMD_QUIT;
            break;
        case 't':
            get_time = !get_time;
            if (!get_time)
                printf("not ");

            printf("print recording time...\n");
            break;
        case 'b':
            auc_dec_reset();
            reset_dac_flag = 1;
            stopbell();
            ringbell(0, 1);
            break;
#if DUMP_STATISTIC
        case 'g':
            reclt_st();
            break;
#endif /* DUMP_STATISTIC */
        }
    }

    return cmd;
}

#define FATTR_PROTE_BIT         (1 << 6)    /* protected */
#define FATTR_READ_BIT          (1 << 7)    /* read */

static int sh_recls(int argc, char **argv, void **result, void *sys_ctx)
{
    struct rec_opt_t rec_opt;
    HLS *hls = NULL;
    char fname[32], *pfname = fname;
    struct sysc_status_t status;
    int rc = 0;
    char msgbuf[ERR_MSG_LEN];
    char fps_test = 0;
    int file_idx = 0;
    int file_num = 0;
    int i = 0;
    char *dirname;
    int nfiles = 0, maxsize = 0;

    memcpy(&rec_opt, &rec_opt_default, sizeof(struct rec_opt_t));
    reset_dac_flag = 0;
    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'b':
                    reset_dac_flag = 1;
                    break;
                case 'r':
                    fps_test = 1;
                    break;
                case 't':
                    rec_opt.time = atoi(argv[++i]);
                    break;
                case 's':
                    rec_opt.size = size(argv[++i]) * 1024;
                    break;
                case 'f':
                    rec_opt.fno = atoi(argv[++i]);
                    break;
                case 'q':
                    rec_opt.quality = atoi(argv[++i]);
                    break;
                case 'v':
                    rec_opt.standard = atoi(argv[++i]);
                    if (rec_opt.standard == AVIN_CVBS_PAL) {
                        rec_opt.width = 720;
                        rec_opt.height = 288;
                    } else if (rec_opt.standard == AVIN_CVBS_NTSC) {
                        rec_opt.width = 720;
                        rec_opt.height = 240;
                    } else {
                        printf("Invalid video standard %d. (%d for PAL, %d for NTSC)\n",
                               rec_opt.standard, AVIN_CVBS_PAL, AVIN_CVBS_NTSC);
                        goto EXIT;
                    }

                    break;
                case 'l':
                    file_num = atoi(argv[++i]);
                    break;
                case 'm':
                    rec_opt.media = atoi(argv[++i]);
                    break;
            }
        }
    }

    quit = 0;
    file_idx = 0;

#if DUMP_STATISTIC
    /* clear recfile statistics */
    memset(recfile_st, 0, sizeof(struct recfile_statistic));
#endif /* DUMP_STATISTIC */

    if (reset_dac_flag) {
        ringbell(0, 1);
    }

    hls = openls(rec_opt.media, FATFS);
    media_get_config(FATFS, rec_opt.media, &dirname, &nfiles, &maxsize);

    while (rc >= 0 && !quit) {
        unsigned long long timestamp = read_cpu_count();

        snprintf(fname, 12, "%06d_%d.avi", file_idx, rec_opt.fps_ratio);
        rc = getnewls(hls, &pfname, pfname, 1/* victim */, 0, 0);
        if (rc < 0) {
            printf("Failed to generate record file name: %s.\n", fname);
            break;
        }

        printf("getnewls time: %lld\n", read_cpu_count() - timestamp);

        rec_opt.user_info = fname;
        rec_opt.user_info_len = 12;
        printf("Record [%s] %dx%d , standard %d, max frame size %d, user info %d bytes: (%s)\n",
               fname, rec_opt.width, rec_opt.height, rec_opt.standard, rec_opt.size,
               rec_opt.user_info_len, rec_opt.user_info);
        rc = recfile(fname, &rec_opt, recls_cb);
        printf("recfile route command %d.\n", rc);
    
        /* get end state */
        rc = recfile_status(&status);
        if (rc < 0) {
            printf("refile_status() return error %d.\n", rc);
            rc = get_err(msgbuf);
            if (rc) {
                printf("get error %d, message : %s\n", rc, msgbuf);
                break;
            }
        } else {
            printf("state = %d\n", status.state);
            printf("fno = %d\n", status.fno);
            printf("time = %d\n", status.time);
            printf("quality = %d\n", status.quality);

            sprintf(fname, "%s/%06d_%d.avi", dirname, file_idx, rec_opt.fps_ratio);
            rc = rec_parse(fname);
            if (rc < rec_opt.fno) {
                printf("[%s] frame number error (%d)\n", fname, rc);
                break;
            }

            if (fps_test) {
                rec_opt.fps_ratio = (rec_opt.fps_ratio + 1) & 0x03;
            }

            file_idx = (file_idx == 99999999) ? 0 : (file_idx + 1);
            if (file_num > 0 && file_idx >= file_num)
                break;
        }

#if DUMP_STATISTIC
        reclt_st();
#endif /* DUMP_STATISTIC */
    }

    if (reset_dac_flag) {
        stopbell();
    }

#if DUMP_STATISTIC
    reclt_st();
#endif /* DUMP_STATISTIC */

    /* get end state */
    rc = recfile_status(&status);
    if (rc < 0) {
        printf("refile_status() return error %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    } else {
        printf("state = %d\n", status.state);
        printf("fno = %d\n", status.fno);
        printf("time = %d\n", status.time);
        printf("quality = %d\n", status.quality);
    }

EXIT:
    if (hls)
        closels(hls);

    return 0;
}

static int sh_total_frames(int argc, char **argv, void **result, void *sys_ctx)
{
    if (argc != 2) {
        printf("Invalide argument.\n");
        return -1;
    }

    printf("[%s] total %d frames.\n", argv[1], rec_parse(argv[1]));

    return 0;
}

STORY(rec_story, rec_story_fno)
{
    story_init();

    item(recfile, f_000.avi -f 200);
    item_check(S_R0, 210, "frame number");
    item_end();

    item(recfile, f_001.avi -f 500);
    item_check(S_R0, 510, "frame number");
    item_end();

    item(recfile, f_002.avi -f 200 -r 1);
    item_check(S_R0, 210, "frame number");
    item_end();

    return story_end();
}

STORY(rec_story, rec_story_time)
{
    story_init();

    item(recfile, t_000.avi -t 15);
    item_check(S_R1, 16, "time");
    item_end();

    item(recfile, t_001.avi -t 30);
    item_check(S_R1, 31, "time");
    item_end();

    return story_end();
}

