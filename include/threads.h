/**
 *  @file   threads.h
 *  @brief  user-level thread
 *  $Id: threads.h,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */

#ifndef __THREADS_H
#define __THREADS_H

/***
 * Initialization routines
 */

void thread_manager_init();
/* call this once before launching any threads */

#define thread_create(s, p, pm)  __thread_create(s, p, pm, get_sp()) 


void __thread_create(int stack_size, void (*proc)(int), int param, long int sp);
/* call this to launch proc(param) as a thread */
/* may be called from main or from any thread */

void thread_manager_start();
/* call this once after launching at least one thread */
/* this only returns when all threads are blocked! */

/***
 * Routines to call from within a thread
 */
void thread_yield();
/* call this within a thread to allow scheduling of a new thread */

void thread_exit();
/* call this within a thread to terminate that thread */

void thread_free(void *ptr);
/* call this within a thread instead of calling free() */

/* note, thread_create may also be called within a thread */

/***
 * Utility and debugging routines
 */
void thread_startup_report();
/* reports on the results of the study done in the startup */

void thread_safety_check();
/* call this to caheck for thread stack overflow */

/***
 * Semaphore extension to thread package
 */
/* the declaration of the semphore type given here is a lie! */
struct thread_semaphore {
    void *unused_fields[3];
};

void thread_semaphore_init(struct thread_semaphore *s, int v);
/* call this to initialize a semaphore */

void thread_wait(struct thread_semaphore *s);
/* call this within a thread to block on a semaphore */

void thread_signal(struct thread_semaphore *s);
/* call this within a thread to signal a semaphore */


#endif /* __THREADS_H */
