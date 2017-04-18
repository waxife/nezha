/**
 *  @file   vr_ctrl.h
 *  @brief  Cheetah VR Read driver header
 *  $Id: vr_ctrl.h,v 1.1.1.1 2013/12/18 03:43:54 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/07  Kevin  New file.
 *
 */
#ifndef _VRCTRL_H_
#define _VRCTRL_H_

struct vr_data_t{
    unsigned char pre_value0;
    unsigned char pre_value1;
    unsigned char just_value0;
    unsigned char just_value1;    
};
int vr_open(unsigned char threshold);
int vr_read(struct vr_data_t *p_vr_data);
int vr_check(struct vr_data_t *p_vr_data);
int vr_close(void);

#endif /* _VRCTRL_H_ */
