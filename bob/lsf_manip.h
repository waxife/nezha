/**
 *  @file   lsf_manip.h
 *  @brief
 *  $Id: lsf_manip.h,v 1.1 2014/03/14 12:11:36 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/3/14  Ken 	New file.
 *
 */


#ifndef LSF_MANIP_H_
#define LSF_MANIP_H_

typedef struct {
    HLS *hls;
    int fs;
    int media;
    int pageno;
    int pos;
    char order;     /* 0 ascending, 1 descending */
} finder_t;

int prev_pos(finder_t *f);
int next_pos(finder_t *f);
int deletecurrent(finder_t *f);
void lsf_clear_page();
int open_list(finder_t *f);
int print_page(finder_t *f, int markpos);
int findfirstunread(finder_t *f);

#endif /* LSF_MANIP_H_ */
