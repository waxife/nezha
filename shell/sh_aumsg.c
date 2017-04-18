/**
 *  @file   sh_aumsg.c
 *  @brief  audio message test command
 *  $Id: sh_aumsg.c,v 1.28 2014/04/14 07:09:10 cnyu Exp $
 *  $Author: cnyu $
 *  $Revision: 1.28 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/06  hugo        movied from sh_audmsg.c
 *
 */
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <serial.h>
#include <shell.h>
#include <ctype.h>
#include <sys.h>
#include <debug.h>

#include <norfs.h>
#include <nordb.h>

#include <event.h>
#include <command.h>
#include <fat32.h>
#include <codec_eng/aumsg.h>
#include <codec_eng/ringbell.h>

command_init (sh_aumsgrec, "aumsgrec", "aumsgrec <fs> <filename> [<format>] [<media>]");
command_init (sh_aumsgplay, "aumsgplay", "aumsgplay <fs> <filename>");
command_init (sh_aumsginfo, "aumsginfo", "aumsginfo <fs> <filename>");
command_init (sh_aumsgprop, "aumsgprop", "aumsgprop <fs> <filename> [<media>]");
command_init (sh_aumsglt, "aumsglt", "aumsglt");

int parse_filesystem (char *type);
int parse_media (char *type);
static int show_aumsginfo (int fs, char *name);

static int
aumsgrec_cmd_callback (void **arg)
{
    struct sysc_status_t status[1];
    static int last_time = -1;
    int ch;
    int cmd = 0;
    int rc = 0;

    rc = encau_status (status);
    if (rc < 0) {
        ERROR ("encau_status(), rc=%d\n", rc);
    } else {
        if (status->time != last_time) {
            last_time = status->time;
            printf("(%d)\n", status->time);
        }
    }

    ch = getb2 ();
    if (ch > 0) {
        printf ("'%c'\n", ch);
        switch (toupper (ch)) {
            case 'Q':
                printf (">> CMD_QUIT\n");
                cmd = CMD_QUIT;
                break;
            case 'S':
                printf (">> CMD_STOP\n");
                cmd = CMD_STOP;
                break;
            case 'D':
                printf (">> CMD_DELETE\n");
                cmd = CMD_DELETE;
                break;
            case 'R':
                printf(">> Ringbell start\n");
                ringbell(0, 1);
                break;
            case 'B':
                printf(">> Ringbell stop\n");
                stopbell();
                break;
        }
    }

    return cmd;
}

static int
sh_aumsgrec (int argc, char **argv, void **result, void *sys_ctx)
{
    char *name;
    struct eau_opt_t opt[1];
    struct sysc_status_t status[1];
    int rc;
    char msgbuf[ERR_MSG_LEN];

    if (argc < 3)
        goto EXIT;

    opt->dev = parse_filesystem (argv[1]);
    name = argv[2];
    opt->format = argc > 3 ? atoi(argv[3]) : CONFIG_AUMSG_CODEC_FORMAT;
    opt->media = argc > 4 ? parse_media(argv[4]) : MEDIA_AUMSG;

    rc = enc_aumsg (name, opt, aumsgrec_cmd_callback);
    INFO ("enc_aumsg()=%d\n", rc);
    if (rc == CMD_STOP_ERR) {
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    }

    rc = encau_status (status);
    if (rc < 0) {
        ERROR ("encau_status(), rc=%d\n", rc);
        goto EXIT;
    }
    INFO ("status:%d, state:%d, time:%d, offset:%d\n",
            status->status, status->state, status->time, status->offset);

    return 0;
EXIT:
    print_usage (sh_aumsgrec);
    return -1;
}

static int
aumsgplay_cmd_callback (void **arg)
{
    struct sysc_status_t status[1];
    static int last_time = -1;
    int ch;
    int cmd = 0;
    int rc = 0;

    rc = decau_status (status);
    if (rc < 0) {
        ERROR ("decau_status(), rc=%d\n", rc);
    } else {
        if (status->time != last_time) {
            last_time = status->time;
            printf("(%d)\n", status->time);
        }
    }

    ch = getb2 ();
    if (ch > 0) {
        printf ("'%c'\n", ch);
        switch (toupper (ch)) {
            case 'Q':
                printf (">> CMD_QUIT\n");
                cmd = CMD_QUIT;
                break;
            case 'S':
                printf (">> CMD_STOP\n");
                cmd = CMD_STOP;
                break;
            case 'N':
                printf (">> CMD_NEXT\n");
                cmd = CMD_NEXT;
                break;
            case 'P':
                printf (">> CMD_PREVIOUS\n");
                cmd = CMD_PREVIOUS;
                break;
            case 'D':
                printf (">> CMD_DELETE\n");
                cmd = CMD_DELETE;
                break;
        }
    }

    return cmd;
}

static int
sh_aumsgplay (int argc, char **argv, void **result, void *sys_ctx)
{
    char *name;
    struct dau_opt_t opt[1];
    struct sysc_status_t status[1];
    int fs;
    int rc;

    if (argc < 3)
        goto EXIT;

    fs = parse_filesystem (argv[1]);
    name = argv[2];
    opt->media = argc > 3 ? parse_media(argv[3]) : MEDIA_AUMSG;
    opt->dev = fs;
    rc = dec_aumsg (name, opt, aumsgplay_cmd_callback);
    INFO ("dec_aumsg()=%d\n", rc);

    rc = decau_status (status);
    if (rc < 0) {
        ERROR ("decau_status(), rc=%d\n", rc);
        goto EXIT;
    }

    printf("End of play audio message status: state = %d, time = %d, offset = %d\n",
           status->state, status->time, status->offset);

    return 0;
EXIT:
    print_usage (sh_aumsgplay);
    return -1;
}

static int
show_aumsginfo (int fs, char *name)
{
    struct aumsg_property property[1];
    int rc;

    rc = aumsg_get_info (fs, name, property);
    if (rc < 0) {
        ERROR ("aumsg_get_info(), rc=%d\n", rc);
        return -1;
    }

    printf ("length: %d byte\n", property->length);
    printf ("duration: %d ms\n", property->duration);
    printf ("unread: %d\n", property->unread);
    printf ("datetime: %4d/%02d/%02d %02d:%02d:%02d\n",
            property->year, property->mon, property->mday,
            property->hour, property->min, property->sec);

    return 0;
}

static int
sh_aumsginfo (int argc, char **argv, void **result, void *sys_ctx)
{
    char *name;
    int fs;
    int rc;

    if (argc < 3)
        goto EXIT;

    fs = parse_filesystem (argv[1]);
    name = argv[2];
    rc = show_aumsginfo (fs, name);
    if (rc < 0)
        goto EXIT;

    return 0;
EXIT:
    print_usage (sh_aumsginfo);
    return -1;
}


static int
sh_aumsgprop (int argc, char **argv, void **result, void *sys_ctx)
{
    struct aumsg_property property[1];
    char *name;
    int fs;
    int rc;
    MEDIA media;

    if (argc < 3)
        goto EXIT;

    fs = parse_filesystem (argv[1]);
    name = argv[2];
    media = argc > 3 ? parse_media(argv[3]) : MEDIA_AUMSG;
    rc = aumsg_get_property(fs, media, name, property);
    if (rc < 0) {
        ERROR ("aumsg_get_info(), rc=%d\n", rc);
        return -1;
    }

    printf ("length: %d byte\n", property->length);
    printf ("duration: %d ms\n", property->duration);
    printf ("unread: %d\n", property->unread);
    printf ("datetime: %4d/%02d/%02d %02d:%02d:%02d\n",
            property->year, property->mon, property->mday,
            property->hour, property->min, property->sec);

    return 0;
EXIT:
    print_usage (sh_aumsginfo);
    return -1;
}

static char quit = 0;
static char doorbell_flag = 0;
#if AUDIO_DRV_STATISTIC
static unsigned char reset_dac_flag = 0;
static unsigned int reset_dac_cnt = 0;
#endif /* AUDIO_DRV_STATISTIC */

static int aumsglt_rec_callback (void **arg)
{
    struct sysc_status_t status[1];
    static int last_time = -1;
    int ch;
    int cmd = 0;
    int rc = 0;
    extern int auc_dec_reset(void);

    rc = encau_status (status);
    if (rc < 0) {
        ERROR ("encau_status(), rc=%d\n", rc);
    } else {
        if (status->time != last_time) {
            last_time = status->time;
            printf("(%d)\n", status->time);
#if AUDIO_DRV_STATISTIC
            if (reset_dac_flag) {
                reset_dac_flag++;
            }
#endif /* AUDIO_DRV_STATISTIC */
        }
    }

#if AUDIO_DRV_STATISTIC
    if (!(reset_dac_flag % 5) && reset_dac_flag) {
        printf("reset DAC\n");
        auc_dec_reset();
        reset_dac_cnt++;
        reset_dac_flag = 1;
    }
#endif /* AUDIO_DRV_STATISTIC */

    if (doorbell_flag && (ringbell_status() == ringbell_idle)) {
        ringbell(0, 1);
    }

    ch = getb2 ();
    if (ch > 0) {
        printf ("'%c'\n", ch);
        switch (toupper (ch)) {
            case 'Q':
                printf (">> CMD_QUIT\n");
                cmd = CMD_QUIT;
                quit = 1;
                break;
            case 'S':
                printf("AU_CTL_INT_STATUS = 0x%08X\n", readl(AU_CTL_INT_STATUS));
                break;
            case 'R':
                printf("Reset decoder\n");
                auc_dec_reset();
                break;
        }
    }

    return cmd;
}

static int
aumsglt_dec_callback (void **arg)
{
    struct sysc_status_t status[1];
    static int last_time = -1;
    int ch;
    int cmd = 0;
    int rc = 0;

    rc = decau_status (status);
    if (rc < 0) {
        ERROR ("decau_status(), rc=%d\n", rc);
    } else {
        if (status->time != last_time) {
            last_time = status->time;
            printf("(%d)\n", status->time);
        }
    }

    ch = getb2 ();
    if (ch > 0) {
        printf ("'%c'\n", ch);
        switch (toupper (ch)) {
            case 'Q':
                printf (">> CMD_QUIT\n");
                cmd = CMD_QUIT;
                quit = 1;
                break;
        }
    }

    return cmd;
}

/*
 *  1. record file until no space in the directory.
 *  2. play all files in the directory.
 *  3. delete all files in the directory.
 *  repeat 1 - 3 until press 'q' or over 99999999 files. 
 */
static int sh_aumsglt (int argc, char **argv, void **result, void *sys_ctx)
{
    char fname[32];
    struct eau_opt_t eopt[1];
    struct dau_opt_t dopt[1];
    int file_idx = 0, oldest_file_idx = 0;
    char msgbuf[ERR_MSG_LEN];
    char play_test = 0;
    int i = 0;
    int rc = 0;

    eopt->dev = parse_filesystem ("nor");
    eopt->media = MEDIA_AUMSG;
    eopt->format = 4;
    dopt->dev = parse_filesystem ("nor");
    dopt->media = MEDIA_AUMSG;
    doorbell_flag = 0;

    for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'f':
                    eopt->format = atoi(argv[++i]);
                    if (eopt->format < 0 || eopt->format > 4) {
                        printf("Invalid format %d, valid format is 0 ~ 4\n", eopt->format);
                        goto EXIT;
                    }

                    break;
                case 'd':
                    eopt->dev = parse_filesystem (argv[++i]);
                    dopt->dev = eopt->dev;
                    break;
                case 'b':
                    doorbell_flag = 1;
                    break;
                case 'p':
                    play_test = atoi(argv[++i]);
                    break;
#if AUDIO_DRV_STATISTIC
                case 'r':
                    reset_dac_flag = 1;
                    break;
#endif /* AUDIO_DRV_STATISTIC */
            }
        }
    }

    if (doorbell_flag) {
        ringbell(0, 1);
    }

    quit = 0;
#if AUDIO_DRV_STATISTIC
    reset_dac_cnt = 0;
    au_st->fifo_overflow_int = 0;
#endif /* AUDIO_DRV_STATISTIC */
    while (!quit && file_idx < 100000000) {
        /*  1. record file until no space in the directory. */
        snprintf(fname, 12, "%08d.wav", file_idx);
        printf("record audio message [%s]\n", fname);
        rc = enc_aumsg (fname, eopt, aumsglt_rec_callback);
        printf("enc_aumsg() = %d\n", rc);
#if AUDIO_DRV_STATISTIC
#if 0
        if (au_st->fifo_overflow_int) {
            printf("FIFO overflow !!!!!!!!\n");
            au_st->fifo_overflow_int = 0;
            break;
        } else {
            printf("FIFO no overflow.\n");
        }
#endif
        printf("reset DAC : %d, FIFO overflow : %d\n", reset_dac_cnt, au_st->fifo_overflow_int);
#endif /* AUDIO_DRV_STATISTIC */
        if (rc == CMD_STOP_ERR) {
            rc = get_err(msgbuf);
            printf("get error %d, message : %s\n", rc, msgbuf);
            if (rc == -E_FILE_NO_SPACE) {
                stopbell();
                if (play_test) {
                    /* 2. play ail audio message files */
                    for (i = oldest_file_idx; i < file_idx && !quit; i++) {
                        snprintf(fname, 12, "%08d.wav", i);
                        printf("play audio message [%s]\n", fname);
                        rc = dec_aumsg(fname, dopt, aumsglt_dec_callback);
                        if (rc == CMD_STOP_ERR) {
                            rc = get_err(msgbuf);
                            printf("play audio message error %d, message : %s\n", rc, msgbuf);
                            quit = 1;
                        }
                    }
                }

                /* 3. delete ail audio message files */
                for (i = oldest_file_idx; i < file_idx; i++) {
                    snprintf(fname, 18, "aumsg/%08d.wav", i);
                    printf("remove file [%s]\n", fname);
                    if (eopt->dev == FATFS) {
                        _remove(fname);
                    } else {
                        norfs_remove(fname);
                    }
                }

                oldest_file_idx = file_idx;

                if (doorbell_flag) {
                    ringbell(0, 1);
                }
            }
        } else {
            file_idx++;
        }
    }

    if (doorbell_flag) {
        stopbell();
    }

EXIT:
    return 0;
}
