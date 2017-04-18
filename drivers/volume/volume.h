/* 
 *  @file   volume.h
 *  @brief  volume driver APIs
 *
 *  $Id: volume.h,v 1.1 2014/02/27 07:53:12 kevin Exp $
 *  $Author: kevin $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *  @date   2014/02/27  Kevin    New file.
 *
 */

#ifndef __SCCB_H
#define __SCCB_H

#include <gpio.h>
#define PWM1_GCREG    0xB0400E80
#define PWM1_TCREG    0xB0400E90
#define PWM4_GCREG    0xB0400EA0
#define PWM4_TCREG    0xB0400EB0


/* GPIO operation functions */
/* T582 Use PWM1 to control AMP volume */
/* Must set right VAL_SYSMGM_ALTFUNC_SEL !!!*/
#define PWM_GCREG PWM1_GCREG
#define PWM_TCREG PWM1_TCREG
/***********************************************************/
#define VOL_LEVELS 16
extern int vol_up(void);
extern int vol_down(void);
extern int vol_set(int volume);
extern int vol_reset(void);
extern int vol_init(int volume);
extern int vol_close(void);

#endif /* __SCCB_H */
