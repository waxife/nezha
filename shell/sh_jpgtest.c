/**
 *  @file   sh_jpegenc.c
 *  @brief  jpeg encode test program
 *  $Id: sh_jpgtest.c,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2010/10/07  Jonathan    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <shell.h>
#include <cache.h>
#include <ctype.h>
#include <io.h>

command_init (sh_jenc_p2m, "jenc_p2m", "jenc_p2m");
command_init (sh_jenc_y2j, "jenc_y2j", "jenc_y2j [in_filename] [out_filename]");
command_init (sh_jenc_y2v, "jenc_y2v", "jenc_y2v [-w width] [-h height] [-f fps] [-t total_frames] [-s max_fsize(M)] [in_filename] [out_filename]");
command_init (sh_jenc_c2v, "jenc_c2v", "jenc_c2v [-osd] [-w width] [-h height] [-f fps] [-s max_fsize(M)] [out_filename]");
command_init (sh_jdec_j2y, "jdec_j2y", "jdec_j2y [in_filename] [in_fsize] [out_filename]");
command_init (sh_jdec_v2y, "jdec_v2y", "jdec_v2y [-i] [-t output_frames] [in_filename] [in_fsize] [out_filename]");
command_init (sh_jdec_v2d, "jdec_v2d", "jdec_v2d [-i] [-m sync_mode] [-t output_frames] [in_filename] [in_fsize]");
command_init (sh_jdec_setup, "jdec_setup", "jdec_setup <pplay_lb_conf> <pplay_timing>");

extern int jenc_defpat2mem(void *sys_ctx, unsigned char *outaddr, int *outlen, unsigned char *inaddr, int img_w, int img_h, int no_huff_tbl);
extern int jenc_yuv2jpg (void *sys_ctx, char *out_fname, int *outlen, char *in_fname, int img_w, int img_h, int no_huff_tbl);
extern int jenc_yuv2avi (void *sys_ctx, char *out_fname, int *outlen, char *in_fname, int img_w, int img_h, int fps, int no_huff_tbl, int total_frms, int max_ofsize);
extern int jenc_cmos2avi (void *sys_ctx, char *out_fname, int *outlen, int img_w, int img_h, int fps, int no_huff_tb, int max_ofsizel, int preosd);
extern int jdec_jpg2yuv (void *sys_ctx, char *out_fname, int *outlen, char *in_fname, int in_fsize, int *img_w, int *img_h);
extern int jdec_avi2yuv (void *sys_ctx, char *out_fname, int *outlen, char *in_fname, int in_fsize, int *img_w, int *img_h, int output_frms, int interlace);
extern int jdec_avi2disp (void *sys_ctx, char *in_fname, int in_fsize, int *img_w, int *img_h, int output_frms, int interlace, int sync_mode);

static int sh_jenc_p2m (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    unsigned char *addr = (unsigned char *)0x8800a000;
    int len = 4096 * 6;
    int file_len;
    int width = 640, height = 480;

    if (argc >= 2) {
        if (strchr(argv[1], '+') || strchr(argv[1], '-')) {
            rc = get_addr_len(argv[1], &addr, &len);
        } else {
            rc = get_addr(argv[1], &addr);
        }

        if (rc < 0)
            goto EXIT;
    }
    
    rc = jenc_defpat2mem (sys_ctx, (unsigned char *)addr, &file_len, (unsigned char *)0x8800f000, width, height, 0);
    if (rc >= 0)
        printf("jenc_p2m: addr %p, length %d\n", addr, file_len);
    else
        printf("jenc_p2m: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jenc_p2m);
    return -1;
}

static int sh_jenc_y2j (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *in_name, *out_name;
    int file_len;
    int width = 640, height = 480;

    if (argc >= 3) {
        in_name = argv[1];
        out_name = argv[2];
        printf("jenc_y2j: in file %s, out file %s\n", in_name, out_name);
    } else {
        goto EXIT;
    }
    
    rc = jenc_yuv2jpg (sys_ctx, out_name, &file_len, in_name, width, height, 0);
    if (rc >= 0)
        printf("jenc_y2j: write file, length %d\n", file_len);
    else
        printf("jenc_y2j: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jenc_y2j);
    return -1;
}

static int sh_jenc_y2v (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *in_name, *out_name;
    int file_len;
    int width = 640, height = 480, fps= 30, total_frms = 60, max_fsize = 25;
    int i;

    if (argc >= 3) {
        for (i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                if (argv[i][1] == 'w') {
                    width = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 'h') {
                    height = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 'f') {
                    fps = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 't') {
                    total_frms = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 's') {
                    max_fsize = atoi(argv[i+1]);
                    i++;
                }
            } else {
                break;
            }
        }
        if (i + 1 >= argc) {
            goto EXIT;
        }
        in_name = argv[i];
        out_name = argv[i+1];
        printf("jenc_y2v: in file %s, out file %s\n", in_name, out_name);
    } else {
        goto EXIT;
    }
    
    rc = jenc_yuv2avi (sys_ctx, out_name, &file_len, in_name, width, height, fps, 0, total_frms, max_fsize);
    if (rc >= 0)
        printf("jenc_y2v: write file, length %d\n", file_len);
    else
        printf("jenc_y2v: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jenc_y2v);
    return -1;
}

static int sh_jenc_c2v (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *out_name;
    int file_len;
    int width = 640, height = 480, fps = 30, max_fsize = 25;
    int preosd = 0;
    int i;

    if (argc >= 2) {
        for (i = 1; i < argc; i++) {
            if (strcmp(argv[1], "-osd") == 0) {
                if (argc < 2) {
                    printf("ERROR: too few input arguments\n");
                    goto EXIT;
                }
                preosd = 1;
                i++;
            } else if (argv[i][0] == '-') {
                if (argv[i][1] == 'w') {
                    width = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 'h') {
                    height = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 'f') {
                    fps = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 's') {
                    max_fsize = atoi(argv[i+1]);
                    i++;
                }
            } else {
                break;
            }
        }
        if (i >= argc) {
            printf("ERROR: too few input arguments\n");
            goto EXIT;
        }
        out_name = argv[i];
        printf("jenc_y2v: out file %s, pre-osd %d\n", out_name, preosd);
    } else {
        goto EXIT;
    }
    
    rc = jenc_cmos2avi (sys_ctx, out_name, &file_len, width, height, fps, 0, max_fsize, preosd);
    if (rc >= 0)
        printf("jenc_c2v: write file, length %d\n", file_len);
    else
        printf("jenc_c2v: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jenc_c2v);
    return -1;
}

static int sh_jdec_j2y (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *in_name, *out_name;
    int file_len;
    int in_fsize;
    int width = 0, height = 0;

    if (argc >= 4) {
        in_name = argv[1];
        in_fsize = atoi(argv[2]);
        out_name = argv[3];
        printf("jdec_j2y: in file %s sz %d, out file %s\n", in_name, in_fsize, out_name);
    } else {
        goto EXIT;
    }
    
    rc = jdec_jpg2yuv (sys_ctx, out_name, &file_len, in_name, in_fsize, &width, &height);
    if (rc >= 0)
        printf("jdec_j2y: write file, length %d, img sz (%d x %d)\n", file_len, width, height);
    else
        printf("jdec_j2y: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jdec_j2y);
    return -1;
}

static int sh_jdec_v2y (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *in_name, *out_name;
    int file_len;
    int in_fsize;
    int width = 0, height = 0, output_frms = 30*60, interlace = 0;
    int i;

    if (argc >= 4) {
        for (i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                if (argv[i][1] == 'i') {
                    interlace = 1;
                } else if (argv[i][1] == 't') {
                    output_frms = atoi(argv[i+1]);
                    i++;
                }
            } else {
                break;
            }
        }
        if (i+2 >= argc) {
            printf("ERROR: too few input arguments\n");
            goto EXIT;
        }
        in_name = argv[i];
        in_fsize = atoi(argv[i+1]);
        out_name = argv[i+2];
        printf("jdec_v2y: in file %s sz %d, out file %s\n", in_name, in_fsize, out_name);
    } else {
        goto EXIT;
    }
    
    rc = jdec_avi2yuv (sys_ctx, out_name, &file_len, in_name, in_fsize, &width, &height, output_frms, interlace);
    if (rc >= 0)
        printf("jdec_v2y: write file, length %d, img sz (%d x %d)\n", file_len, width, height);
    else
        printf("jdec_v2y: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jdec_v2y);
    return -1;
}

static int sh_jdec_v2d (int argc, char **argv, void **result, void *sys_ctx)
{
    int rc;
    char *in_name;
    int in_fsize;
    int width = 0, height = 0, output_frms = 30*60, interlace = 0, sync_mode = 1;
    int i;

    if (argc >= 3) {
        for (i = 1; i < argc; i++) {
            if (argv[i][0] == '-') {
                if (argv[i][1] == 'i') {
                    interlace = 1;
                } else if (argv[i][1] == 'm') {
                    sync_mode = atoi(argv[i+1]);
                    i++;
                } else if (argv[i][1] == 't') {
                    output_frms = atoi(argv[i+1]);
                    i++;
                }
            } else {
                break;
            }
        }
        if (i+1 >= argc) {
            printf("ERROR: too few input arguments\n");
            goto EXIT;
        }
        in_name = argv[i];
        in_fsize = atoi(argv[i+1]);
        printf("jdec_v2d: in file %s sz %d\n", in_name, in_fsize);
    } else {
        goto EXIT;
    }
    
    rc = jdec_avi2disp (sys_ctx, in_name, in_fsize, &width, &height, output_frms, interlace, sync_mode);
    if (rc >= 0)
        printf("jdec_v2d: output img sz (%d x %d)\n", width, height);
    else
        printf("jdec_v2d: ERROR, rc(%d)\n", rc);

    return 0;

EXIT:
    print_usage (sh_jdec_v2d);
    return -1;
}

static int
sh_jdec_setup (int argc, char **argv, void **result, void *sys_ctx)
{
    extern unsigned int val_pplay_lb_conf;
    extern unsigned int val_pplay_timing;

    if (argc < 3)
       goto EXIT;

    val_pplay_lb_conf = strtol (argv[1], NULL, 16);
    val_pplay_timing = strtol (argv[2], NULL, 16);

    return 0;
EXIT:
    print_usage (sh_jdec_setup);
    return -1;
}

