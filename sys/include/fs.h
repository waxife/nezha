/**
 *  @file   fs.h
 *  @brief  file system main header
 *  $Id: fs.h,v 1.4 2014/02/10 01:51:13 hugo Exp $
 *  $Author: hugo $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2006 Terawins Inc. All rights reserved.
 * 
 *  @date   2006/12/19  New file.
 *
 */

#ifndef __FS_H
#define __FS_H

#define PATHLEN         63
#define NAMELEN         39

/* reserved user attribute */
#define UA_UNREAD       (1 << 7)
#define UA_UNPROTECTED  (1 << 6)
#define UA_RESERVED     (UA_UNREAD | UA_UNPROTECTED)

#endif /* __FS_H */
