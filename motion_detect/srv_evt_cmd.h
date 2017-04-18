/**
 *  @file   srv_evt_cmd.h
 *  @brief
 *  $Id: srv_evt_cmd.h,v 1.1 2015/07/15 09:51:40 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2013 Terawins Inc. All rights reserved.
 *
 *  @date   2013/11/22  Ken 	New file.
 *
 */

#ifndef SRV_EVT_CMD_H_
#define SRV_EVT_CMD_H_


/* user command of definition */
#define CMD_USE_BASE		0x1100
#define CMD_USER_COPY		(CMD_USE_BASE + 0x01)

extern int get_sys_cmd (int srv_id);
extern void ui_err_timeout_cnt (void);
extern void ui_err_clear_cnt (void);

#endif /* SRV_EVT_CMD_H_ */
