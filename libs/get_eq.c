/**
 *  @file   get_eq.c
 *  @brief  get event queue
 *  $Id: get_eq.c,v 1.1.1.1 2013/12/18 03:43:20 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1.1.1 $
 *
 *  Copyright (c) 2012 Terawins Inc. All rights reserved.
 * 
 *  @date   2012/11/22  yc 	New file.
 *
 */

#include "tw_widget_sys.h"

#define DBG_LEVEL   0
#include "debug.h"

/* Event Queue */
#define EQ_SIZE	16
TW_EVENT event_queue[EQ_SIZE];
unsigned char event_queue_idx = 0;
unsigned char event_queue_cnt = 0;

/**
 * @func    put_event
 * @brief   put event to event queue.
 * @param   TW_EVENT* event		event (type for tw widget system) of point.
 * @return  none
 */
void put_event (TW_EVENT* event)
{
	unsigned char idx;

	if (event_queue_cnt >= EQ_SIZE) {
		WARN("event queue full; data lost!\n");
		return;
	}

	idx = (event_queue_idx + event_queue_cnt) % EQ_SIZE;
	event_queue[idx] = *event;
	event_queue_cnt++;
	dbg(2, "event:%d, EQ<< cnt=%d\n", event->type, (char)event_queue_cnt);
}

/**
 * @func    get_event
 * @brief   get event from event queue.
 * @param   none
 * @return  none type if not event, or return at event queue of point if has event
 */
TW_EVENT* get_event (void)
{
	unsigned char id;

	if (event_queue_cnt == 0) {
		/* queue empty; no available data */
		event_queue[0].type = TW_EVENT_TYPE_NONE;
		return &event_queue[0];
	}

	id = event_queue_idx;
	event_queue_idx = (event_queue_idx + 1) % EQ_SIZE;
	event_queue_cnt--;
	dbg(2, "event: %d, EQ>> cnt=%d\n", event_queue[id].type, (char)event_queue_cnt);

	return &event_queue[id];
}

/**
 * @func    clear_event
 * @brief   clear all events at event queue.
 * @param   none
 * @return  none
 */
void clear_event(void)
{
	event_queue_cnt = 0;
}

#if 0	// maybe someday need this
/* Service Event Queue */
#define SRV_EQ_SIZE		4
TW_EVENT srv_evt_queue[SRV_EQ_SIZE];
unsigned char srv_evt_queue_idx = 0;
unsigned char srv_evt_queue_cnt = 0;

/**
 * @func    srv_put_event
 * @brief   put event to event queue.
 * @param   TW_EVENT* event		event (type for tw widget system) of point.
 * @return  none
 */
void srv_put_event (TW_EVENT* event)
{
	unsigned char idx;

	if (srv_evt_queue_cnt >= SRV_EQ_SIZE) {
		WARN("event queue full; data lost!\n");
		return;
	}

	idx = (srv_evt_queue_idx + srv_evt_queue_cnt) % SRV_EQ_SIZE;
	srv_evt_queue[idx] = *event;
	srv_evt_queue_cnt++;
	dbg(2, "event:%d, EQ<< cnt=%d\n", event->type, (char)srv_evt_queue_cnt);
}

/**
 * @func    srv_get_event
 * @brief   get event from event queue.
 * @param   none
 * @return  none type if not event, or return at event queue of point if has event
 */
TW_EVENT* srv_get_event (void)
{
	unsigned char id;

	if (srv_evt_queue_cnt == 0) {
		/* queue empty; no available data */
		srv_evt_queue[0].type = TW_EVENT_TYPE_NONE;
		return &srv_evt_queue[0];
	}

	id = srv_evt_queue_idx;
	srv_evt_queue_idx = (srv_evt_queue_idx + 1) % SRV_EQ_SIZE;
	srv_evt_queue_cnt--;
	dbg(2, "event: %d, EQ>> cnt=%d\n", srv_evt_queue[id].type, (char)srv_evt_queue_cnt);

	return &srv_evt_queue[id];
}

/**
 * @func    srv_clear_event
 * @brief   clear all events at event queue.
 * @param   none
 * @return  none
 */
void srv_clear_event(void)
{
	srv_evt_queue_cnt = 0;
}
#endif

