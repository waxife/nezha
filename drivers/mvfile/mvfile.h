/**
 *  @file   touch.h
 *  @brief  touch panel driver header file 
 *  $Id: mvfile.h,v 1.1 2014/01/10 06:08:17 onejoe Exp $
 *  $Author: onejoe $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 *
 *  @date   2011/11/22  dos    New file.
 *
 */
#ifndef _MVFILE_H_
#define _MVFILE_H_

int copysd2nor(int media_type, char *sdfile, char *norfile);
int copynor2sd(int media_type, char *norfile, char *sdfile);

#endif
