/**
 *  @file   lsf_manip.c
 *  @brief
 *  $Id: lsf_manip.c,v 1.1 2014/03/14 12:11:36 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/3/14  Ken 	New file.
 *
 */

/**
 *  @file   ui_lsf.c
 *  @brief  terawins T582 file browser ui
 *  $Id: lsf_manip.c,v 1.1 2014/03/14 12:11:36 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2014 Terawins Inc. All rights reserved.
 *
 *  @date   2014/03/14  onejoe     New file.
 *
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <shell.h>
#include <ls.h>
#include <debug.h>
#include <fat32.h>
#include <norfs.h>
#include <time.h>
#include <serial.h>
#include <unistd.h>

#include "lsf_manip.h"

//#define LSF_UI
#ifdef LSF_UI
#include "ui_lsf.h"
#else
#define lsf_draw_line(x, y, z)
#define lsf_draw_mark(x, y)
#define lsf_clear_page(x)
#define lsf_clear_block(x, y, c, i)
#define lsf_clear_mark(x)
#endif

#define _MAX_CHAR 30
#define _LSF_X 8
#define _LSF_Y 3

#define PAGE_ITEMS      12

#define FP(pos)     ((pos)+5)

#define ATTR_PROTECT        (1 << 0)
#define ATTR_ISREAD         (1 << 1)


int open_list(finder_t *f)
{
    int rc, attr;
    char *pos;

    if (f->hls) {
        ERROR("invalid invoke sequence\n");
        return -1;
    }
    f->hls = openls(f->media, f->fs);
    if (f->hls == NULL) {
        ERROR("failed to openls\n");
        return -1;
    }

    if (f->order == 0) {
        pos2headls(f->hls);
        rc = getnextls(f->hls, &pos, &attr);
    } else {
        pos2taills(f->hls);
        rc = getprevls(f->hls, &pos, &attr);
    }

    f->pageno = 0;
    f->pos = 0;

//    print_finder(f);

    return 0;
}

static int print_page_ascending(finder_t *f, int markpos)
{
    int i = 0;
    int no = f->pageno;
    int rc;
    char *pos;
    int attr;
    char *status = NULL;
    //char attrstr[8];
    char str[_MAX_CHAR];
    int y = 0;

    markls(f->hls, 0);  /* mark page start */
    markls(f->hls, 1);

    //gotopage();

    rc = getcurrls(f->hls, &pos, &attr);
    if (rc < 0 && (eols(f->hls) || sols(f->hls))) {
        //printf(FILE_P"      [--- empty ---]");
        snprintf(str, _MAX_CHAR, "--");
        lsf_draw_line(_LSF_X, y++, str);
        markpos = -1;
        i = 1;
    } else if (rc < 0) {
        status = "getprevls error rc =";
        goto EXIT;
    } else {
        markls(f->hls, 1);
//      printf("%c%4d %-40s %s\n",
//          (markpos == 0 ? '>' : ' '),
//          no+1, pos, attrmark(attr, attrstr));

        snprintf(str, _MAX_CHAR, "%4d  %-20s", no+1, pos);
        lsf_draw_line(_LSF_X, y++, str);
        if (markpos == 0) {
            //draw sel
            lsf_draw_mark(markpos, f->media);
        }

        for (i = 1; i < PAGE_ITEMS ; i++) {
            rc = getnextls(f->hls, &pos, &attr);
            if (rc < 0 && !eols(f->hls)) {
                status = "getnextls error rc=";
                goto EXIT;
            }

            if (rc < 0 && eols(f->hls)) {
                if (markpos >= i) {
                    markpos = i - 1;
                    //print mark
                    //printf("\x1b[%d;0f>\n",FP(markpos));
                    lsf_draw_mark(markpos, f->media);
                }
                break;
            }

            no++;
//          printf("%c%4d %-40s %s\n",
//              (markpos == i ? '>' : ' '),
//              no+1, pos, attrmark(attr, attrstr));
            snprintf(str, _MAX_CHAR, "%4d  %-20s", no+1, pos);
            lsf_draw_line(_LSF_X, y++, str);
            if (markpos == i) {
                //draw sel
                lsf_draw_mark(markpos, f->media);
            }

            if (markpos >= i)
                markls(f->hls, 1);
        }
    }
    rc = 0;
EXIT:
    f->pos = markpos;
    seekmarkls(f->hls, 1);

//  for (; i < PAGE_ITEMS; i++) {
//      printf(BLANK_L);
//  }
    lsf_clear_block(_LSF_X, y+_LSF_Y, _MAX_CHAR, PAGE_ITEMS-i);

  if (status) {
        //printf(STATUS_P "Status: %s%d" BLANK40, status, rc);
      printf("Status: %s%d\n", status, rc);
  }

//    gotocmd();

    return rc;
}

static int print_page_descending(finder_t *f, int markpos)
{
    int i = 0;
    int no = f->pageno;
    int rc;
    char *pos;
    int attr;
    char *status = NULL;
    //char attrstr[8];
    char str[_MAX_CHAR];
    int y = 0;

    markls(f->hls, 0);  /* mark page start */
    markls(f->hls, 1);

    //gotopage();

    rc = getcurrls(f->hls, &pos, &attr);
    if (rc < 0 && (eols(f->hls) || sols(f->hls))) {
        //printf(FILE_P"      [--- empty ---]");
        snprintf(str, _MAX_CHAR, "--");
        markpos = -1;
        i = 1;
    } else if (rc < 0) {
        status = "getprevls error rc =";
        goto EXIT;
    } else {
        markls(f->hls, 1);
//      printf("%c%4d %-40s %s\n",
//          (markpos == 0 ? '>' : ' '),
//          no+1, pos, attrmark(attr, attrstr));
        snprintf(str, _MAX_CHAR, "%4d  %-20s", no+1, pos);
        lsf_draw_line(_LSF_X, y++, str);
        if (markpos == 0) {
            //draw sel
            lsf_draw_mark(markpos, f->media);
        }
        for (i = 1; i < PAGE_ITEMS ; i++) {
            rc = getprevls(f->hls, &pos, &attr);
            if (rc < 0 && !sols(f->hls)) {
                status = "getnextls error rc=";
                goto EXIT;
            }

            if (rc < 0 && sols(f->hls)) {
                if (markpos >= i) {
                    markpos = i - 1;
                    //printf("\x1b[%d;0f>\n",FP(markpos));
                    lsf_draw_mark(markpos, f->media);
                }
                break;
            }

            no++;
//          printf("%c%4d %-40s %s\n",
//              (markpos == i ? '>' : ' '),
//              no+1, pos, attrmark(attr, attrstr));
            snprintf(str, _MAX_CHAR, "%4d  %-20s", no+1, pos);
            lsf_draw_line(_LSF_X, y++, str);
            if (markpos == i) {
                //draw sel
                lsf_draw_mark(markpos, f->media);
            }

            if (markpos >= i)
                markls(f->hls, 1);

        }
    }
    rc = 0;
EXIT:
    f->pos = markpos;
    seekmarkls(f->hls, 1);

//  for (; i < PAGE_ITEMS; i++) {
//      printf(BLANK_L);
//  }
    lsf_clear_block (_LSF_X, y+_LSF_Y, _MAX_CHAR, PAGE_ITEMS-i);

    if (status) {
          //printf(STATUS_P "Status: %s%d" BLANK40, status, rc);
        printf("Status: %s%d\n", status, rc);
    }

    //gotocmd();

    return rc;
}

int print_page(finder_t *f, int markpos)
{
    if(f->order == 0)
        return print_page_ascending(f, markpos);
    else
        return print_page_descending(f, markpos);
}

static int print_firstpage(finder_t *f)
{
    int rc;
    char *pos;
    int attr;

    f->pageno = 0;
    f->pos = 0;

    if (f->order == 0) {
        pos2headls(f->hls);
        rc = getnextls(f->hls, &pos, &attr);
  } else {
      pos2taills(f->hls);
      rc = getprevls(f->hls, &pos, &attr);
  }

    print_page(f, 0);

    return 0;
}


static int print_markpage_ascending(finder_t *f)
{
    int rc;
    char *pos;
    int attr;
    int i;

    f->pageno = 0;
    f->pos = 0;
    pos2headls(f->hls);
    rc = getnextls(f->hls, &pos, &attr);
    if (rc < 0) {
        return print_firstpage(f);
    }

    while(1) {
        markls(f->hls, 0);
        for (i = 0; i < PAGE_ITEMS; i++) {
            if (matchmarkls(f->hls, 1)) {
                seekmarkls(f->hls, 0);
                print_page(f, i);
                return 0;
             }

             rc = getnextls(f->hls, &pos, &attr);
             if (rc < 0) {
                return print_firstpage(f);
             }

        }
        f->pageno += PAGE_ITEMS;
    }

    return 0;
}

static int print_markpage_descending(finder_t *f)
{
    int rc;
    char *pos;
    int attr;
    int i;

    f->pageno = 0;
    f->pos = 0;
    pos2taills(f->hls);
    rc = getprevls(f->hls, &pos, &attr);
    if (rc < 0) {
        return print_firstpage(f);
    }

    while(1) {
        markls(f->hls, 0);
        for (i = 0; i < PAGE_ITEMS; i++) {
            if (matchmarkls(f->hls, 1)) {
                seekmarkls(f->hls, 0);
                print_page(f, i);
                return 0;
             }

             rc = getprevls(f->hls, &pos, &attr);
             if (rc < 0) {
                return print_firstpage(f);
             }
        }
        f->pageno += PAGE_ITEMS;
    }

    return 0;
}

static int print_markpage(finder_t *f)
{
    if (f->order == 0)
        return print_markpage_ascending(f);
    else
        return print_markpage_descending(f);
}


static int next_pos_ascending(finder_t *f)
{
    char *pos;
    int attr;
    int rc;
    //char attrstr[8];


    markls(f->hls, 1);
    rc = getnextls(f->hls, &pos, &attr);
    if (rc < 0) {
        if (eols(f->hls)) {
            //printf(STATUS_P "Status: end of list" BLANK40);
            printf("Status: end of list\n");
            seekmarkls(f->hls, 1);
            return 0;
        } else {
            //printf(STATUS_P "Status: failed to getnextls %d" BLANK40, rc);
            printf("Status: failed to getnextls %d\n", rc);
            return rc;
        }
    }

    if (f->pos >= 0){
        //printf("\x1b[%d;0f ",FP(f->pos));
        //clean mark
        lsf_clear_mark(f->pos);
    }

    f->pos++;
    if(f->pos >= PAGE_ITEMS) {
        f->pageno += PAGE_ITEMS;
        print_page(f, 0);
    } else {
        lsf_draw_mark(f->pos, f->media);
//      printf("\x1b[%d;0f>%4d %-40s %s\n",FP(f->pos), f->pageno + f->pos + 1 ,
//          pos, attrmark(attr, attrstr));
    }

    return 0;
}

static int next_pos_descending(finder_t *f)
{
    char *pos;
    int attr;
    int rc;
    //char attrstr[8];


    markls(f->hls, 1);
    rc = getprevls(f->hls, &pos, &attr);
    if (rc < 0) {
        if (sols(f->hls)) {
            //printf(STATUS_P "Status: end of list" BLANK40);
            printf("Status: end of list\n");
            seekmarkls(f->hls, 1);
            return 0;
        } else {
            //printf(STATUS_P "Status: failed to getnextls %d" BLANK40, rc);
            printf("Status: failed to getnextls %d\n", rc);
            return rc;
        }
    }

    if (f->pos >= 0){
        //printf("\x1b[%d;0f ",FP(f->pos));
        lsf_clear_mark(f->pos);
    }

    f->pos++;
    if(f->pos >= PAGE_ITEMS) {
        f->pageno += PAGE_ITEMS;
        print_page(f, 0);
    } else {
       // printf("\x1b[%d;0f>%4d %-40s %s\n",FP(f->pos), f->pageno + f->pos + 1 ,
       //     pos, attrmark(attr, attrstr));
        lsf_draw_mark(f->pos, f->media);
    }

    return 0;
}

int next_pos(finder_t *f)
{
    if (f->order == 0)
        return next_pos_ascending(f);
    else
        return next_pos_descending(f);
}

static int seekback_page_ascending(finder_t *f)
{
    char *pos;
    int attr;
    int rc;
    int i = 0;

    if (f->pageno == 0)
        return 1;

    seekmarkls(f->hls, 0);

    for (i = 0; i < PAGE_ITEMS; i++) {
        f->pageno --;
        rc = getprevls(f->hls, &pos, &attr);
        if (rc < 0 && sols(f->hls))
            break;
        if (rc < 0) {
            //printf(STATUS_P "Status: getprevls failed %d" BLANK40, rc);
            printf("Status: getprevls failed %d\n", rc);
            return -1;
        }
    }

    return 0;
}

static int seekback_page_descending(finder_t *f)
{
    char *pos;
    int attr;
    int rc;
    int i = 0;

    if (f->pageno == 0)
        return 1;

    seekmarkls(f->hls, 0);

    for (i = 0; i < PAGE_ITEMS; i++) {
        f->pageno --;
        rc = getnextls(f->hls, &pos, &attr);
        if (rc < 0 && eols(f->hls))
            break;
        if (rc < 0) {
            //printf(STATUS_P "Status: getnextls failed %d" BLANK40, rc);
            printf("Status: getnextls failed %d\n", rc);
            return -1;
        }
    }

    return 0;
}

static int seekback_page(finder_t *f)
{
    if (f->order == 0)
        return seekback_page_ascending(f);
    else
        return seekback_page_descending(f);
}



//static int seekforward_page_ascending(finder_t *f)
//{
//    char *pos;
//    int attr;
//    int rc;
//    int i;
//    int pageno = f->pageno;
//
//    seekmarkls(f->hls, 0);
//
//    for (i = 0; i < PAGE_ITEMS; i++) {
//        pageno++;
//        rc = getnextls(f->hls, &pos, &attr);
//        if (rc < 0 && eols (f->hls))
//            return 1;
//        if (rc < 0) {
//            //printf(STATUS_P "Status: getnextls failed %d" BLANK40, rc);
//            printf("Status: getnextls failed %d\n", rc);
//            return -1;
//        }
//    }
//
//
//    f->pageno = pageno;
//
//    return 0;
//}
//
//static int seekforward_page_descending(finder_t *f)
//{
//    char *pos;
//    int attr;
//    int rc;
//    int i;
//    int pageno = f->pageno;
//
//    seekmarkls(f->hls, 0);
//
//    for (i = 0; i < PAGE_ITEMS; i++) {
//        pageno++;
//        rc = getprevls(f->hls, &pos, &attr);
//        if (rc < 0 && sols (f->hls))
//            return 1;
//        if (rc < 0) {
//            //printf(STATUS_P "Status: getprevls failed %d" BLANK40, rc);
//            printf("Status: getprevls failed %d\n", rc);
//            return -1;
//        }
//    }
//    f->pageno = pageno;
//
//    return 0;
//}

//static int seekforward_page(finder_t *f)
//{
//    if (f->order == 0)
//        return seekforward_page_ascending(f);
//    else
//        return seekforward_page_descending(f);
//}


static int prev_pos_ascending(finder_t *f)
{
    char *pos;
    int attr;
    int rc;
    //char attrstr[8];

    markls(f->hls, 1);
    rc = getprevls(f->hls, &pos, &attr);
    if (rc < 0) {
        if (sols(f->hls)) {
            //printf(STATUS_P "Status: start of list" BLANK40);
            printf("Status: start of list\n");
            seekmarkls(f->hls, 1);
            return 0;
        }
        //printf(STATUS_P "Status:failed to getnextls %d" BLANK40, rc);
        printf("Status:failed to getnextls %d\n", rc);
        return rc;
    }
    //printf("\x1b[%d;0f ", FP(f->pos));
    lsf_clear_mark(f->pos);
    f->pos--;
    if(f->pos < 0) {
        rc = seekback_page(f);
        if (rc < 0) {
            /* MUST DO SOMETHING HERE closesl and reopen ls */
            //print_page(f);
            //printf(STATUS_P "Status: seekback_page failed pageno %d" BLANK40, f->pageno);
            printf("Status: seekback_page failed pageno %d\n", f->pageno);
            return -1;
        }
        print_page(f, PAGE_ITEMS -1);
    } else {
//      printf("\x1b[%d;0f>%4d %-40s %s\n",FP(f->pos), f->pageno + f->pos + 1,
//          pos, attrmark(attr, attrstr));
        lsf_draw_mark(f->pos, f->media);
    }

    return 0;
}

static int prev_pos_descending(finder_t *f)
{
    char *pos;
    int attr;
    int rc;
    //char attrstr[8];

    markls(f->hls, 1);
    rc = getnextls(f->hls, &pos, &attr);
    if (rc < 0) {
        if (eols(f->hls)) {
            //printf(STATUS_P "Status: start of list" BLANK40);
            printf("Status: start of list\n");
            seekmarkls(f->hls, 1);
            return 0;
        }
        //printf(STATUS_P "Status:failed to getnextls %d" BLANK40, rc);
        printf("Status:failed to getnextls %d\n", rc);
        return rc;
    }
    //printf("\x1b[%d;0f ", FP(f->pos));
    lsf_clear_mark(f->pos);
    f->pos--;
    if(f->pos < 0) {
        rc = seekback_page(f);
        if (rc < 0) {
            /* MUST DO SOMETHING HERE closesl and reopen ls */
            //print_page(f);
            //printf(STATUS_P "Status: seekback_page failed pageno %d" BLANK40, f->pageno);
            printf("Status: seekback_page failed pageno %d\n", f->pageno);
            return -1;
        }
        print_page(f, PAGE_ITEMS -1);
    } else {
//      printf("\x1b[%d;0f>%4d %-40s %s\n",FP(f->pos), f->pageno + f->pos + 1,
//          pos, attrmark(attr, attrstr));
        lsf_draw_mark(f->pos, f->media);
    }

    return 0;
}

int prev_pos(finder_t *f)
{
    if (f->order == 0)
        return prev_pos_ascending(f);
    else
        return prev_pos_descending(f);
}

//static int next_page(finder_t *f)
//{
//    int rc;
//
//    markls(f->hls, 1);
//    rc = seekforward_page(f);
//    if (rc > 0) { /* no more page */
//        seekmarkls(f->hls, 1);
//        //printf(STATUS_P"Status: no more next page" BLANK40);
//        printf("Status: no more next page\n");
//        return 0;
//    }
//    if (rc < 0) {
//        return rc;
//    }
//
//    print_page(f, f->pos);
//
//    return 0;
//}
//
//static int prev_page(finder_t *f)
//{
//    int rc;
//
//
//    markls(f->hls, 1);
//    rc = seekback_page(f);
//    if (rc == 1) { /* no more page */
//        seekmarkls(f->hls, 1);
//        //printf(STATUS_P"Status: no more prev page" BLANK40);
//        printf("Status: no more prev page\n");
//        return 0;
//    }
//
//    if (rc < 0) {
//        return rc;
//    }
//
//    print_page(f, f->pos);
//
//    return 0;
//}

//static char *
//dirfname(finder_t *f, char *pos, char *fname)
//{
//    char *dir;
//    int nfiles, size;
//
//    media_get_config(f->fs, f->media, &dir, &nfiles, &size);
//
//    snprintf(fname, 48, "%s/%s", dir, pos);
//
//    return fname;
//}

int findfirstunread(finder_t *f)
{
    char *pos;
    int attr;
    int rc;


    if (f->order == 0)
        pos2headls(f->hls);
    else
        pos2taills(f->hls);

    while(1) {
        if (f->order == 0)
            rc = getnextls(f->hls, &pos, &attr);
        else
            rc = getprevls(f->hls, &pos, &attr);

        if (rc < 0 || eols(f->hls)) {
            //printf(STATUS_P"Status: not found unread item"BLANK40);
            printf("Status: not found unread item\n");
            print_page(f, 0);
            return 0;
        }

        if ((attr & ATTR_ISREAD) == 0) {
            markls(f->hls, 1);
            break;
        }
    }

    print_markpage(f);

    return 0;
}

//static int
//markallread(finder_t *f)
//{
//    char *pos;
//    int attr;
//    int rc;
//
//    pos2headls(f->hls);
//
//    while(1) {
//        rc = getnextls(f->hls, &pos, &attr);
//        if (eols(f->hls))
//            break;
//        if (rc < 0) {
//            //printf(STATUS_P"Status: getnextls failed rc = %d"BLANK40, rc);
//            printf("Status: getnextls failed rc = %d\n", rc);
//            return -1;
//        }
//        setattrls(f->hls, pos, ATTR_ISREAD);
//    }
//    seekmarkls(f->hls, 0);
//    print_page(f, f->pos);
//    //print_banner(f);
//
//    return 0;
//}
//
//
//static int
//protectcurrent(finder_t *f)
//{
//    int rc;
//    char *pos;
//    int attr;
//    //char attrstr[8];
//
//    rc = getcurrls(f->hls, &pos, &attr);
//    if (rc < 0) {
//        if (eols(f->hls)) {
//            //printf(STATUS_P"Status: point to end of list" BLANK40);
//            printf("Status: point to end of list\n");
//        } else if (sols(f->hls)) {
//            //printf(STATUS_P"Status: point to start of list" BLANK40);
//            printf("Status: point to start of list\n");
//        } else {
//            //printf(STATUS_P"Status: getcurrls failed rc = %d"BLANK40, rc);
//            printf("Status: getcurrls failed rc = %d\n", rc);
//            return -1;
//        }
//        return 0;
//    }
//
//    /* mark has read */
//    setattrls(f->hls, pos, ATTR_PROTECT);
//    getcurrls(f->hls, &pos, &attr);
//
//
////  printf("\x1b[%d;0f>%4d %-40s %s\n",FP(f->pos), f->pageno + f->pos + 1,
////          pos, attrmark(attr, attrstr));
//
//    //print_banner(f);
//    return 0;
//
//}
//
//static int
//opencurrent(finder_t *f)
//{
//    int rc;
//    char *pos;
//    int attr;
//    char fname[48];
//    struct fd32 fd[1];
//    struct stat_t st;
//    struct norfs_fd nfd[1];
//    struct norfs_stat nst;
//    char *open;
//    //char attrstr[8];
//    int size;
//
//
//    rc = getcurrls(f->hls, &pos, &attr);
//    if (rc < 0) {
//        if (eols(f->hls)) {
//            //printf(STATUS_P"Status: point to end of list" BLANK40);
//            printf("Status: point to end of list\n");
//        } else if (sols(f->hls)) {
//            //printf(STATUS_P"Status: point to start of list" BLANK40);
//            printf("Status: point to start of list\n");
//        } else {
//            //printf(STATUS_P"Status: getcurrls failed rc = %d"BLANK40, rc);
//            printf("Status: getcurrls failed rc = %d", rc);
//            return -1;
//        }
//        return 0;
//    }
//
//    dirfname(f, pos, fname);
//
//    if (f->fs == FATFS) {
//        rc = _open(fd, fname, 0, FD_RDONLY);
//        if (rc == 0)
//            open = "OK";
//        else
//            open = "Failed";
//
//        _close(fd);
//
//        rc = _stat(fname, &st);
//        if (rc == 0)
//            size = st.st_size;
//        else
//            size = -1;
//    } else {
//        rc = norfs_open(nfd, fname, 0, FD_RDONLY);
//        if (rc == 0)
//            open = "OK";
//        else
//            open = "Failed";
//
//        norfs_close(nfd);
//
//        rc = norfs_stat(fname, &nst);
//        if (rc == 0)
//            size = nst.st_size;
//        else
//            size = -1;
//    }
//
//    //printf(STATUS_P"Status: %s %x open %s size %d" BLANK40, pos, attr,
//    //            open, size);
//    printf("Status: %s %x open %s size %d\n\n", pos, attr,
//                open, size);
//
//    /* mark has read */
//    setattrls(f->hls, pos, ATTR_ISREAD);
//    getcurrls(f->hls, &pos, &attr);
//
//
////  printf("\x1b[%d;0f>%4d %-40s %s\n",FP(f->pos), f->pageno + f->pos + 1,
////          pos, attrmark(attr, attrstr));
//
//    return 0;
//}
//
//static int
//gettotal(finder_t *f)
//{
//    int rc;
//    int nls, npick;
//    rc = gettotalls(f->hls, &nls, &npick, 0, 0);
//    //printf(STATUS_P "Status: gettotalls rc = %d nls = %d npick = %d" BLANK40, rc, nls, npick);
//    printf("Status: gettotalls rc = %d nls = %d npick = %d\n", rc, nls, npick);
//    return 0;
//}




static int
deletecurrent_ascending(finder_t *f)
{
    int rc;
    char *pos;
    int attr;

    if (eols(f->hls)) {
        //printf(STATUS_P"Status: Can't delete end of list pointer" BLANK40);
        printf("Status: Can't delete end of list pointer\n");
        return 0;
    }

    if (sols(f->hls)) {
        //printf(STATUS_P"Status: Can't delete start of list pointer" BLANK40);
        printf("Status: Can't delete start of list pointer\n");
        return 0;
    }

    if (f->pos == 0) {
        markls(f->hls, 1);
        rc = getnextls(f->hls, &pos, &attr);
        if (rc < 0 && eols(f->hls)) {
            rc = seekback_page(f);
            if (rc < 0) {
                //printf(STATUS_P"Status: ls something wrong, failed to delete"BLANK40);
                printf("Status: ls something wrong, failed to delete\n");
                return -1;
            }
            if (rc > 0) {
                f->pos = -1;
            } else {
                markls(f->hls, 0);
                f->pos = PAGE_ITEMS - 1;
            }
        } else if (rc < 0) {
            //printf(STATUS_P"Status: ls something wrong, failed to delete"BLANK40);
            printf("Status: ls something wrong, failed to delete\n");
            return -1;
        }

        seekmarkls(f->hls, 1);
    }


    rc = deletels(f->hls);
    seekmarkls(f->hls, 0);
    print_page(f, f->pos);

    //print_banner(f);

    return 0;
}


static int
deletecurrent_descending(finder_t *f)
{
    int rc;
    char *pos;
    int attr;

    if (eols(f->hls)) {
        //printf(STATUS_P"Status: Can't delete end of list pointer" BLANK40);
        printf("Status: Can't delete end of list pointer\n");
        return 0;
    }

    if (sols(f->hls)) {
        //printf(STATUS_P"Status: Can't delete start of list pointer" BLANK40);
        printf("Status: Can't delete start of list pointer\n");
        return 0;
    }

    if (f->pos == 0) {
        markls(f->hls, 1);
        rc = getprevls(f->hls, &pos, &attr);
        if (rc < 0 && sols(f->hls)) {
            rc = seekback_page(f);
            if (rc < 0) {
                //printf(STATUS_P"Status: ls something wrong, failed to delete"BLANK40);
                printf("Status: ls something wrong, failed to delete\n");
                return -1;
            }
            if (rc > 0) {
                f->pos = -1;
            } else {
                markls(f->hls, 0);
                f->pos = PAGE_ITEMS - 1;
            }
        } else if (rc < 0) {
            //printf(STATUS_P"Status: ls something wrong, failed to delete"BLANK40);
            printf("Status: ls something wrong, failed to delete\n");
            return -1;
        }

        seekmarkls(f->hls, 1);
    }


    rc = deletels(f->hls);
    seekmarkls(f->hls, 0);
    print_page(f, f->pos);

    //print_banner(f);

    return 0;
}

int deletecurrent(finder_t *f)
{
    int rc;
    int attr;
    char *pos;
    //int ch;

    rc = getcurrls(f->hls, &pos, &attr);
//  if (rc == 0 && (attr & ATTR_PROTECT)) {
//      printf(STATUS_P "Status: Are you sure? [y/n]" BLANK40);
//      gotocmd();
//      ch = getkey();
//      if (ch != 'y' && ch != 'Y')
//          return 0;
//  }

    if (f->order == 0) {
        deletecurrent_ascending(f);
    } else {
        deletecurrent_descending(f);
    }

    return 0;
}

//static int
//deleteall(finder_t *f)
//{
//    int ndel = 0;
//    int rc;
//    char *pos;
//    int attr;
//
//    rc = pos2headls(f->hls);
//    if (rc < 0) {
//        //printf(STATUS_P "Status: failed to deleteall" BLANK40);
//        printf("Status: failed to deleteall\n");
//        return -1;
//    }
//
//    rc = getnextls(f->hls, &pos, &attr);
//    if (rc < 0 && eols(f->hls)) {
//       // printf(STATUS_P "Status: delete %d all done" BLANK40, ndel);
//        printf("Status: delete %d all done\n", ndel);
//        return 0;
//    }
//    if (rc < 0) {
//        //printf(STATUS_P "Status: failed delete all" BLANK40);
//        printf("Status: failed delete all\n");
//        return -1;
//    }
//
//    while(1) {
//        rc = deletels(f->hls);
//        if (eols(f->hls)) {
//            //printf(STATUS_P "Status: delete %d all done" BLANK40, ndel);
//            printf("Status: delete %d all done\n", ndel);
//            break;
//        }
//
//        if (rc < 0) {
//            //printf(STATUS_P "Status: partial delete %d, but others failed" BLANK40, ndel);
//            printf("Status: partial delete %d, but others failed\n", ndel);
//            return -1;
//        }
//
//        ndel++;
//        //printf(STATUS_P "Status: delete %d" BLANK40, ndel);
//        printf("Status: delete %d\n", ndel);
//    }
//
//    f->pos = -1;
//
//    return 0;
//
//}






