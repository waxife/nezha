/**
 *  @file   out_i80_320x240.h
 *  @brief  scaler setting functions, output i80 320x240
 *  $Id: out_i80_320x240.h,v 1.1.1.1 2013/12/18 03:43:54 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/04/28  Jonathan Kuo  New file.
 *
 */
#ifndef _OUT_I80_320X240_H_
#define _OUT_I80_320X240_H_

int scaler_set_in_cpu_out_i80_320x240 (struct i80c *pi80);
int scaler_set_in_cmos_out_i80_320x240 (struct i80c *pi80);
int scaler_set_in_t515_out_i80_320x240 (struct i80c *pi80);
int scaler_set_in_cvbs_out_i80_320x240 (struct i80c *pi80, int format);
int scaler_set_in_t515_ntsc_out_i80_320x240 (struct i80c *pi80);
int scaler_set_in_pb_cmos_out_i80_320x240 (struct i80c *pi80, int format);
int scaler_set_in_pb_cvbs_out_i80_320x240 (struct i80c *pi80, int format);

#endif /* _OUT_I80_320X240_H_ */
