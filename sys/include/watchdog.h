/**
 *  @file   watchdog.h
 *  @brief  Kirin watchdog header
 *  $Id: watchdog.h,v 1.4 2014/02/13 11:50:55 ken Exp $
 *  $Author: ken $
 *  $Revision: 1.4 $
 *
 *  Copyright (c) 2011 Terawins Inc. All rights reserved.
 * 
 *  @date   2011/05/16  C.N.Yu  New file.
 *
 */
#ifndef _WATCHDOG_H_
#define _WATCHDOG_H_

#define WATCHDOG_INIT_TIME	60000	// 60 sec
#define WATCHDOG_MIN_CNT	1000	// 1 sec

/**
 * @func    watchdog_enable
 * @brief   enable the watchdog
 * @param   none
 * @return  none
 */
void watchdog_enable (void);
/**
 * @func    watchdog_disable
 * @brief   disable the watchdog
 * @param   none
 * @return  none
 */
void watchdog_disable (void);
/**
 * @func    watchdog_keepalive
 * @brief   keep alive and setting timeout
 * @param   millisec	millisecond
 * @return  none
 */
void watchdog_keepalive (unsigned long millisec);
/**
 * @func    watchdog_init
 * @brief   watchdog initialization
 * @param   none
 * @return  none
 */
void watchdog_init (void);
/**
 * @func    watchdog_touch
 * @brief   keep alive and getting the timeout from CONFIG_WATCHDOG_TIMEOUT
 * @param   none
 * @return  none
 */
void watchdog_touch (void);

#endif /* _WATCHDOG_H_ */
