/*#----------------------------------------------------------------------
#  @file   i2cget.h
#  @brief  i2cget, copy file to memory through i2c path
#
#  $Id: i2cget.h,v 1.1.1.1 2013/12/18 03:43:37 jedy Exp $
#  $Author: jedy $
#  $Revision: 1.1.1.1 $
#
#  Copyright (c) 2010 Terawins Inc. All rights reserved.
#
#  @date   2010/12/24  yc_shih New file.
#----------------------------------------------------------------------*/

#ifndef __I2CFILE_H
#define __I2CFILE_H

int i2cget(char* filename);
/*
 *  Return value of i2c_file
 *
 *  0              :  Success
 *  -ENOBUFS (-132):  Data buf have no enough sapce
 *  -ENOSPACE(-70 ):  No space error
 *  			1. Open file fail
 *  			2. Write data into fial error
 *  -ECOMM   (-28) : Communication error
 *  			1. Recevice size != data size fram PC (data miss)
 *  			2. Crc error, data contents have error
 */

#endif

