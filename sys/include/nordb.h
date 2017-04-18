/**
 *  @file   nordb.h
 *  @brief  NOR record database
 *  $Id: nordb.h,v 1.1.1.1 2013/12/18 03:43:22 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2010 Terawins Inc. All rights reserved.
 *
 *  @date   2013/09/30  hugo    New file.
 *
 */
#ifndef __NORDB_H
#define __NORDB_H

int nordb_scan (struct norfs_dirent *dirents, int count);
int nordb_has_freeblk (void);
int nordb_add ();
int nordb_this ();
int nordb_get_free ();
int nordb_next ();
int nordb_prev ();
int nordb_set_idx ();
int nordb_delete ();

#endif /* __NORDB_H */
