/**
 *  @file   sh_snap_pic.c
 *  @brief  picture codec demo
 *  $Id: sh_pic_codec.c,v 1.27 2015/08/31 10:34:52 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.27 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 *
 *  @date   2011/01/12  sherman    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <sys.h>
#include <io.h>
#include <mipsregs.h>
#include <serial.h>
#include <codec.h>
#include <event.h>
#include <cache.h>
#include <debug.h>
#include <ctype.h>
#include <cvbs.h>
#include <codec_eng/decpic.h>
#include <codec_eng/encpic.h>
#include <codec_eng/jreg.h>
#include <mconfig.h>
#include <ls.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys.h>
#include <time.h>
#include <heap.h>
#include "display.h"

command_init (sh_decpic_status, "decpic_status", "decpic_status");
command_init (sh_encpic_status, "encpic_status", "encpic_status");
command_init (sh_decpic, "decpic", "decpic [<fs>][<name>] [-t timeout]");
command_init (sh_decpic_ram, "decpic_ram", "decpic_ram [<name>] [-t timeout]");
command_init (sh_encpic, "encpic", "encpic [-w <width>] [-h <height>] [-q <quality>] [-f <field>] [<fs>] [<name>] [<user_info>]");
command_init (sh_picinfo, "picinfo", "picinfo [<fs>] [<name>]");

command_init (sh_decpic_c, "decpic_c", "decpic_c [<fs>][<name>] [-t timeout] [-c <count>]");
command_init (sh_encpic_c, "encpic_c", "encpic_c [-w <width>] [-h <height>] [-q <quality>] [-c <count>] [-f <field>] [<fs>] [<name>]");

command_init (sh_decpicls, "decpicls", "decpicls [<fs>]");
command_init (sh_encpicls, "encpicls", "encpic_c [-w <width>] [-h <height>] [-q <quality>] [-f <field>] [-d <fs>]");

command_init (sh_nor2sd, "nor2sd", "nor2sd [<src_name>] [<dst_name>]");

command_init (sh_encpic3, "encpic3", "encpic3 [-w <width>] [-h <height>] [-q <quality>] [-f <field>] [<fs>] [<name>] [<user_info>]");

int decpic_get_status(void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = decpic_status(&status);
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

int encpic_get_status(void)
{
    struct sysc_status_t status;
    int rc = 0;

    rc = encpic_status(&status);
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

static int snap_dec_cmd_callback (void **arg)
{
    int ch = 0;
    int cmd = 0;
	ch = getb2 ();
    if (ch > 0) {
        printf ("'%c'\n", ch);
        switch (toupper (ch)) {
            case 'Q':
                /* quit */
                printf ("snap_status: CMD_QUIT\n");
                cmd = CMD_QUIT;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
            case 'N':
                /* next */
                printf ("snap_status: CMD_NEXT\n");
                cmd = CMD_NEXT;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
            case 'P':
                /* next */
                printf ("snap_status: CMD_PREVIOUS\n");
                cmd = CMD_PREVIOUS;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
			case 'E':
                /* error */
                printf ("snap_status: CMD_ERROR\n");
                cmd = CMD_STOP_ERR;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                break;
        }
    }
    return cmd;
}

static int sh_decpic (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
	char *fs = NULL;
    int rc;
	
	struct dpic_opt_t opt;
	char *ch;
	char msgbuf[ERR_MSG_LEN];
	
	if(argc<3)
		goto EXIT;
		
	memset(&opt, 0, sizeof(struct dpic_opt_t));

	fs = str2upper(argv[1]);
	if(fs != NULL){
		if(strcmp("FATFS",fs) == 0)
			opt.dev = FATFS;
		else if(strcmp("NORFS",fs) == 0)
			opt.dev = NORFS;
		else
			goto EXIT;
	}
	
	fname = argv[2];
	
	ch = strchr(fname, '.');
	if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) {
		goto EXIT;
	}
	
	if(argc>4){
		if(argv[3][0] == '-' && argv[3][1] == 't'){
			opt.duration = atoi(argv[4]);
		}
	}
	
	printf("select = %d fname = %s\n",opt.dev,fname);

	rc = decpic(fname, &opt, snap_dec_cmd_callback);
	
	printf("decpic route command %d.\n", rc);
    if (rc == CMD_STOP_ERR) {
        printf("encpic return error command %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    }
    
	if (rc < 0)
        goto EXIT;
    
    return 0;

EXIT:
	print_usage(sh_decpic);
    return -1;
}
static int sh_decpic_ram (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
    int rc;
	struct fd32 fs2[1];
	struct dpic_opt_t opt;
	char *ch;
	char msgbuf[ERR_MSG_LEN];
	char *dbuf = heap_alloc(64*1024);
	if(argc<2)
		goto EXIT;
		
	memset(&opt, 0, sizeof(struct dpic_opt_t));
	
    opt.dev = FATFS;
    
	fname = argv[1];
	
	ch = strchr(fname, '.');
	if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) {
		goto EXIT;
	}
	
	if(argc>3){
		if(argv[2][0] == '-' && argv[2][1] == 't'){
			opt.duration = atoi(argv[3]);
		}
	}
	
	printf("select = %d fname = %s\n",opt.dev,fname);

    /* open file */
    rc = _open (fs2, (uint8_t *)fname, 0, FS2_RDONLY);
    if (rc < 0) {
        ERROR ("iopen(), rc=%d\n", rc);
        goto EXIT;
    }
    
    if(fs2->size > 64*1024) {
        printf("file size > 64k, not support \n");
        goto EXIT;
    }
    
    rc = _read (fs2, dbuf, fs2->size);
    if (rc < 0) {
        ERROR ("iread(), rc=%d\n", rc);
        goto EXIT;
    }
    
    opt.data_length = fs2->size;
    
	rc = decpic2_from_ram(dbuf, &opt, snap_dec_cmd_callback);
	
	printf("decpic route command %d.\n", rc);
    if (rc == CMD_STOP_ERR) {
        printf("encpic return error command %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    }
    
	if (rc < 0)
        goto EXIT;
    heap_release(dbuf);
    return 0;

EXIT:
    heap_release(dbuf);
	print_usage(sh_decpic_ram);
    return -1;
}
static int sh_encpic(int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
	char *fs = NULL;
    int rc = 0;
	struct epic_opt_t opt;
	char *ch;
	int i =0;
	unsigned long long t1;
	unsigned long long t2;
	char msgbuf[ERR_MSG_LEN];
	
	memset(&opt,0,sizeof(struct epic_opt_t));

	for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'w':
                    opt.width = atoi(argv[++i]);
                    break;
                case 'h':
                    opt.height = atoi(argv[++i]);
                    break;
                case 'q':
                    opt.quality = atoi(argv[++i]);
                    break;
				case 'f':
					if(atoi(argv[++i]) == 1)
						opt.field_type = ONE_FIELD;
					else
						opt.field_type = TWO_FIELD;
					break;
            }
        } else {
			fs = str2upper(argv[i]);
			printf("fs = %s\n",fs);
			if(fs != NULL){
				if(strcmp("FATFS",fs) == 0)
					opt.dev = FATFS;
				else if(strcmp("NORFS",fs) == 0)
					opt.dev = NORFS;
				else
					goto EXIT;
			}
            fname = argv[i+1];
			printf("fname = %s\n",fname);
			ch = strchr(fname, '.');
			if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) 
				goto EXIT;
			if(argc>=12){
				opt.user_info = argv[i+2];
				opt.user_info_len = strlen(argv[i+2]);
			}
			break;
        }
    }
	if(opt.user_info_len>0)
		printf("user_info = %s \n",opt.user_info);
	t1 = read_cpu_count();
	rc = encpic(fname, &opt, snap_dec_cmd_callback);
	t2 = read_cpu_count();
	//printf("before count = %lld (%ld sec  %ld msec)\n",t1, hz2sec(t1), hz2msec(t1));
	//printf("after count = %lld (%ld sec  %ld msec)\n",t2, hz2sec(t2), hz2msec(t2));
    printf("encpic route command %d.\n", rc);
    if (rc == CMD_STOP_ERR) {
        printf("encpic return error command %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    }
	
    if (rc < 0)
        goto EXIT;
    
    return 0;
EXIT:
    print_usage(sh_encpic);
    return -1;
}

static int sh_decpic_status(int argc, char **argv, void **result, void *sys_ctx)
{
    decpic_get_status();

    return 0;
}

static int sh_encpic_status(int argc, char **argv, void **result, void *sys_ctx)
{
    encpic_get_status();

    return 0;
}

static int sh_picinfo (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
	char *fs = NULL;
    int rc;
	int type = 0;
	struct pic_property property;
	
	
	char *ch;
	
	memset(&property,0,sizeof(struct pic_property));
	
	if(argc<3)
		goto EXIT;

	fs = str2upper(argv[1]);
	if(fs != NULL){
		if(strcmp("FATFS",fs) == 0)
			type = FATFS;
		else if(strcmp("NORFS",fs) == 0)
			type = NORFS;
		else
			goto EXIT;
	}
	
	fname = argv[2];
	
	ch = strchr(fname, '.');
	if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) {
		goto EXIT;
	}
	rc =pic_get_info(type,fname,&property);
	if(rc>=0){
		printf("width = %d\n",property.width);
		printf("height = %d\n",property.height);
		printf("time : %lu (%4d/%02d/%02d %02d:%02d:%02d)\n",
                property.time, property.year,  property.mon, property.mday,
                property.hour, property.min, property.sec);
		if(property.user_info_len>0)
			printf("user_len = %x user_info =%s\n",property.user_info_len,property.user_info);
	}
	if (rc < 0)
        goto EXIT;
    
    return 0;

EXIT:
	print_usage(sh_decpic);
    return -1;
}

static int sh_decpic_c (int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
	char *fs = NULL;
    int rc = 0;
	int count = 0;
	char fullname[32]={0};
	
	struct dpic_opt_t opt;
	char *ch;
	int i =0;
	
	if(argc<5)
		goto EXIT;
		
	memset(&opt, 0, sizeof(struct dpic_opt_t));

	fs = str2upper(argv[1]);
	if(fs != NULL){
		if(strcmp("FATFS",fs) == 0)
			opt.dev = FATFS;
		else if(strcmp("NORFS",fs) == 0)
			opt.dev = NORFS;
		else
			goto EXIT;
	}
	
	fname = argv[2];
	
	ch = strchr(fname, '.');
	if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) {
		goto EXIT;
	}
	
	if(argc>5){
		if(argv[3][0] == '-' && argv[3][1] == 't'){
			opt.duration = atoi(argv[4]);
		}
		if(argv[5][0] == '-' && argv[5][1] == 'c'){
			count = atoi(argv[6]);
		}
	}
	
	printf("select = %d fname = %s\n",opt.dev,fname);

	for(i = 0; i<count ;i++){
		sprintf(fullname,"%d_%s",i,fname);
		printf("%s\n",fullname);
		rc = decpic(fullname, &opt, snap_dec_cmd_callback);
    }
	if (rc < 0)
        goto EXIT;
    
    return 0;

EXIT:
	print_usage(sh_decpic);
    return -1;
}

static int sh_encpic_c(int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
	char *fs = NULL;
    int rc = 0;
	struct epic_opt_t opt;
	char *ch;
	int i =0;
	int count = 0;
	char fullname[32]={0};
	
	memset(&opt,0,sizeof(struct epic_opt_t));

	for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'w':
                    opt.width = atoi(argv[++i]);
                    break;
                case 'h':
                    opt.height = atoi(argv[++i]);
                    break;
                case 'q':
                    opt.quality = atoi(argv[++i]);
                    break;
				case 'c':
                    count = atoi(argv[++i]);
                    break;
				case 'f':
					if(atoi(argv[++i]) == 1)
						opt.field_type = ONE_FIELD;
					else
						opt.field_type = TWO_FIELD;
					break;
            }
        } else {
			fs = str2upper(argv[i]);
			printf("fs = %s\n",fs);
			if(fs != NULL){
				if(strcmp("FATFS",fs) == 0)
					opt.dev = FATFS;
				else if(strcmp("NORFS",fs) == 0)
					opt.dev = NORFS;
				else
					goto EXIT;
			}
            fname = argv[i+1];
			printf("fname = %s\n",fname);
			ch = strchr(fname, '.');
			if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) 
				goto EXIT;
			break;
        }
    }
	for(i =0;i<count;i++){
		sprintf(fullname,"%d_%s",i,fname);
		printf("%s\n",fullname);
		rc = encpic(fullname, &opt, snap_dec_cmd_callback);
	}
    if (rc < 0)
        goto EXIT;
    
    return 0;
EXIT:
    print_usage(sh_encpic);
    return -1;
}

static int sh_decpicls (int argc, char **argv, void **result, void *sys_ctx)
{
	HLS *hls = NULL;
	char *fs = NULL;
    int rc = 0;
	char* photo_fname = NULL;
	int fattrib = 0;
	int cmd = 0;
	
	struct dpic_opt_t dpic_opt;
	
	memset(&dpic_opt, 0, sizeof(struct dpic_opt_t));
	
	if(argc<2)
		goto EXIT;
		
	printf("N:next picture , P: previous picture Q: quit shell command\n");
		
	fs = str2upper(argv[1]);
	if(fs != NULL){
		if(strcmp("FATFS",fs) == 0)
			dpic_opt.dev = FATFS;
		else if(strcmp("NORFS",fs) == 0)
			dpic_opt.dev = NORFS;
		else
			goto EXIT;
	}
	
	hls = openls(PIC_LS, dpic_opt.dev);
    if (hls == NULL) {
    	goto EXIT;
    }

	rc = pos2headls (hls);
	if (rc < 0) {
		ERROR ("pos2headls(), rc=%d\n", rc);
		goto EXIT;
	}
	
	rc = getnextls(hls, &photo_fname, &fattrib);
	if (rc < 0) {
		ERROR ("pos2headls(), rc=%d\n", rc);
		goto EXIT;
	}
	
	cmd = decpic(photo_fname, &dpic_opt, snap_dec_cmd_callback);
	if(rc < 0){
		printf("decpic error\n");
		goto EXIT;
	}
	
	while(1){
		switch (cmd) {
				case CMD_NEXT:
					rc = getnextls(hls, &photo_fname, &fattrib);
					if(rc < 0){
						WARN("in tail now and to head!\n");
						rc = pos2headls (hls);
						if (rc < 0) {
							ERROR ("pos2headls(), rc=%d\n", rc);
							goto EXIT;
						}
						rc = getnextls(hls, &photo_fname, &fattrib);
						if (rc < 0) {
							ERROR ("getnextls(), rc=%d\n", rc);
							goto EXIT;
						}
					}
					printf("file_name = %s",photo_fname);
					cmd = decpic(photo_fname, &dpic_opt, snap_dec_cmd_callback);
					if(rc < 0){
						printf("decpic error\n");
							goto EXIT;
					}
				break;
				case CMD_PREVIOUS:
					rc = getprevls(hls, &photo_fname, &fattrib);
					if(rc < 0){
						WARN("in tail now and to head!\n");
						rc = pos2taills (hls);
						if (rc < 0) {
							ERROR ("pos2taills(), rc=%d\n", rc);
							goto EXIT;
						}
						rc = getprevls(hls, &photo_fname, &fattrib);
						if (rc < 0) {
							ERROR ("getprevls(), rc=%d\n", rc);
							goto EXIT;
						}
					}
					printf("file_name = %s",photo_fname);
					cmd = decpic(photo_fname, &dpic_opt, snap_dec_cmd_callback);
					if(cmd < 0){
						printf("decpic error\n");
							goto EXIT;
					}
				break;
				case CMD_QUIT:
					goto EXIT;
				break;
		}
	}

EXIT:
	if(hls)
		closels(hls);
    return -1;
}

char encpicls_quit = 0;
static int encpicls_cmd_callback (void **arg)
{
    int ch = 0;
    int cmd = 0;

	ch = getb2 ();
    if (ch > 0) {
        printf ("'%c'\n", ch);
        switch (toupper (ch)) {
            case 'Q':
                /* quit */
                printf ("snap_status: CMD_QUIT\n");
                cmd = CMD_QUIT;
                cvbs2_bluescreen_on(1,0,0x80,0x80);
                encpicls_quit = 1;
                break;
        }
    }
    return cmd;
}

static int sh_encpicls(int argc, char **argv, void **result, void *sys_ctx)
{
    HLS *hls = NULL;
    char fname[32] = { 0 }, *pfname = fname;
    int rc = 0;
	struct epic_opt_t opt;
	int i =0;
	char msgbuf[ERR_MSG_LEN];
    int file_cnt = -1;
	
	memset(&opt,0,sizeof(struct epic_opt_t));
    opt.width = 720;
    opt.height = 288;
    opt.quality = 80;
    opt.field_type = ONE_FIELD;
    opt.dev = NORFS;

	for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'w':
                    opt.width = atoi(argv[++i]);
                    break;
                case 'h':
                    opt.height = atoi(argv[++i]);
                    break;
                case 'q':
                    opt.quality = atoi(argv[++i]);
                    break;
				case 'f':
					if(atoi(argv[++i]) == 1)
						opt.field_type = ONE_FIELD;
					else
						opt.field_type = TWO_FIELD;
					break;
			    case 'd':
					if(atoi(argv[++i]) == 1)
						opt.dev = FATFS;
					else
					    opt.dev = NORFS;
					break;
			        
            }
        } else {			
			break;
        }
    }
    
	if(opt.user_info_len>0)
		printf("user_info = %s \n",opt.user_info);

    /* 0. set CVBS-in liveview */
    display_init();
    display_bklight_on();
    display_set_cvbs_full(VIDEO_STD_PAL, CH0);
    cvbs2_bluescreen_on(0,0,0,0);

    /* 1. open VIDEO list of file system */
    hls = openls(PIC_LS, NORFS);

    encpicls_quit = 0;
    while (encpicls_quit) {
        /* 2. file name is current time */
	    struct tm time;
	    rtc_get_time(&time);
        file_cnt = (file_cnt == 9999) ? 0 : (file_cnt + 1);
	    snprintf(fname, 24, "%04d%02d%02d_%02d%02d%02d_%04d.jpg",
                 time.tm_year + 1970, time.tm_mon + 1, time.tm_mday,
                 time.tm_hour, time.tm_min, time.tm_sec, file_cnt);
        printf("snapshot picture file name : [%s]\n", fname);

        /* 3. get free file in file system, replace oldest file if no free file */
        rc = getnewls(hls, &pfname, pfname, 1, 0, 0);
        if (rc < 0) {
            printf("Failed to generate record file name: %s.\n", fname);
            break;
        }

        rc = encpic(fname, &opt, encpicls_cmd_callback);
        printf("encpic route command %d.\n", rc);
        if (rc == CMD_STOP_ERR) {
            printf("encpic return error command %d.\n", rc);
            rc = get_err(msgbuf);
            if (rc) {
                printf("get error %d, message : %s\n", rc, msgbuf);
            }
        }

        if (rc < 0)
            goto EXIT;
    }
    
    return 0;

EXIT:
    print_usage(sh_encpic);
    return -1;
}

static int sh_nor2sd (int argc, char **argv, void **result, void *sys_ctx)
{
	int rc =0;
	if(argc != 3)
		goto EXIT;
	rc = pic_nor2sd(argv[1],argv[2]);
	if(rc<0)
		printf("error\n");
	return 0;
EXIT:
	 print_usage(sh_nor2sd);
	 return -1;
}

static int sh_encpic3(int argc, char **argv, void **result, void *sys_ctx)
{
    char *fname = NULL;
	char *fs = NULL;
    int rc = 0;
	struct epic_opt_t opt;
	char *ch;
	int i =0;
	unsigned long long t1;
	unsigned long long t2;
	char msgbuf[ERR_MSG_LEN];
	
	memset(&opt,0,sizeof(struct epic_opt_t));
	
	opt.width = 720;
	opt.height = 288;
	opt.quality = 80;

	for (i = 1; i < argc; i++) {
        if (argv[i][0] == '-') {
            switch (argv[i][1]) {
                case 'w':
                    opt.width = atoi(argv[++i]);
                    break;
                case 'h':
                    opt.height = atoi(argv[++i]);
                    break;
                case 'q':
                    opt.quality = atoi(argv[++i]);
                    break;
				case 'f':
					if(atoi(argv[++i]) == 1)
						opt.field_type = ONE_FIELD;
					else
						opt.field_type = TWO_FIELD;
					break;
            }
        } else {
			fs = str2upper(argv[i]);
			printf("fs = %s\n",fs);
			if(fs != NULL){
				if(strcmp("FATFS",fs) == 0)
					opt.dev = FATFS;
				else if(strcmp("NORFS",fs) == 0)
					opt.dev = NORFS;
				else
					goto EXIT;
			}
            fname = argv[i+1];
			printf("fname = %s\n",fname);
			ch = strchr(fname, '.');
			if (ch == NULL || (strncmp(ch +1, "jpg", 3) != 0)) 
				goto EXIT;
			if(argc>=12){
				opt.user_info = argv[i+2];
				opt.user_info_len = strlen(argv[i+2]);
			}
			break;
        }
    }
	if(opt.user_info_len>0)
		printf("user_info = %s \n",opt.user_info);
	t1 = read_cpu_count();
	rc = encpic3(fname, &opt, snap_dec_cmd_callback);
	t2 = read_cpu_count();
	//printf("before count = %lld (%ld sec  %ld msec)\n",t1, hz2sec(t1), hz2msec(t1));
	//printf("after count = %lld (%ld sec  %ld msec)\n",t2, hz2sec(t2), hz2msec(t2));
    printf("encpic route command %d.\n", rc);
    if (rc == CMD_STOP_ERR) {
        printf("encpic return error command %d.\n", rc);
        rc = get_err(msgbuf);
        if (rc) {
            printf("get error %d, message : %s\n", rc, msgbuf);
        }
    }
	
    if (rc < 0)
        goto EXIT;
    
    return 0;
EXIT:
    print_usage(sh_encpic);
    return -1;
}
