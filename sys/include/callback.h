/**
 *  @file   callback.h
 *  @brief  callback definition
 *  $Id: callback.h,v 1.2 2014/01/17 09:32:16 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.2 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2013/12/05  hugo    New file.
 *
 */
#ifndef __CALLBACK_H
#define __CALLBACK_H

/* command callback 
 *   - caller pass the point to the command argument of corresponding command
 *   - callee cast 'arg' to the correct type according to corresponding command
 *
 *   ps. caller allocate the memory of command argument
 */
typedef int (* cmd_callback_t)(void **arg);

#endif /* __CALLBACK_H */
