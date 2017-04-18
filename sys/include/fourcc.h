/**
 *  @file   fat32.h
 *  @brief  fourcc definition
 *  $Id: fourcc.h,v 1.1 2014/01/16 05:24:51 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 * 
 *  @date   2014/01/16  hugo    New file.
 *
 */
#ifndef __FOURCC_H
#define __FOURCC_H

#define FOURCC(C0,C1,C2,C3) (unsigned int)(((C3) << 24) | ((C2) << 16) | ((C1) << 8) | ((C0) << 0))

#define ID_RIFF FOURCC('R', 'I', 'F', 'F')
#define ID_AVI  FOURCC('A', 'V', 'I', ' ')
#define ID_mjpg FOURCC('m', 'j', 'p', 'g')
#define ID_00dc FOURCC('0', '0', 'd', 'c')
#define ID_00wb FOURCC('0', '0', 'w', 'b')
#define ID_01wb FOURCC('0', '1', 'w', 'b')
#define ID_skip FOURCC('s', 'k', 'i', 'p')
#define ID_00dx FOURCC('0', '0', 'd', '\xd9')   // workaround for atoms being
#define ID_00wx FOURCC('0', '0', 'w', '\xd9')   // overwritten 0xffd9 in the
#define ID_01wx FOURCC('0', '1', 'w', '\xd9')   // beginning of the atom

#define ID_WAVE FOURCC('W', 'A', 'V', 'E')
#define ID_fmt_ FOURCC('f', 'm', 't', ' ')
#define ID_data FOURCC('d', 'a', 't', 'a')

#endif /* __FOURCC_H */
