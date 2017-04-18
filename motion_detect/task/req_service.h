/**
 *  @file   req_service.h
 *  @brief
 *  $Id: req_service.h,v 1.1 2015/07/15 09:55:34 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/19  Ken 	New file.
 *
 */


#ifndef REQ_SERVICE_H_
#define REQ_SERVICE_H_

extern int get_video_total (void);
extern int get_video_nread (void);
extern int get_play_srv_state (int *state);
extern int get_play_srv_std (void);

extern int get_photo_total (void);
extern int get_photo_nread (void);
extern int get_pview_srv_state (int *state);
extern int get_pview_srv_std (void);

extern int get_aumsg_total (void);
extern int get_aumsg_nread (void);
extern int get_aumsg_srv_state (int *state);

extern void req_playback_srv (void);
extern void req_record_srv (void);
extern void req_picview_srv (int fs);
extern void req_snapshot_srv (int fs);
extern void req_auplay_srv (int fs);
extern void req_aumsg_srv (int fs);
extern void req_slide_srv (void);
extern void req_md_srv (int x, int y, int w, int h, int fs);
extern void req_picture_lsf (int fs);
extern void req_aumsg_lsf (int fs);
extern void req_video_lsf (void);
extern void req_exit_sys (void);

#endif /* REQ_SERVICE_H_ */
