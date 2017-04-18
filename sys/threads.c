/**
 *  @file   threads.c
 *  @brief  user-level thread
 *  $Id: threads.c,v 1.1 2014/08/06 07:07:36 jedy Exp $
 *  $Author: jedy $
 *  $Revision: 1.1 $
 *
 *  Copyright (c) 2007 Terawins Inc. All rights reserved.
 * 
 *  @date   2007/02/26  jedy    New file.
 *
 */
#include <config.h>
#include <stdio.h>
#include <stdlib.h>
#include <setjmp.h>
#include <sys_stack.h>
#include <spinlock.h>
#include <stdatomic.h>
#include <sys.h>

/* just in case this isn't compiled on UNIX system
 * we back off to use the standard setjmp and longjmp.
 * UNIX systems have _setjmp that just does control
 * transfers, and then setjmp (no underscore) that also
 * saves the signal masks.  Saving those makes sense
 * for exception handlers, but not for coroutines.
 */

#ifndef _setjmp
#define _setjmp setjmp
#endif

#ifndef _longjmp
#define _longjmp longjmp
#endif

void thread_startup_report()
/* reports on the results of the study down by above code */
{
    printf("Thread startup study results\n");
    printf("  Stack grows down\n");
    printf("  Local variable offset from AR base = %d\n", 0);
    printf("  Jump buffer fields subject to modification:\n");
    printf("  Jump buffer size: %ld\n", sizeof(jmp_buf));
}

/***
 * PART II: -- The Thread Manager
 * Base on the result reported by Part I
 */

/***
 * thread data types
 */

struct thread {
    struct thread *next;    /* used to link threads into queue */
    int size;               /* use size of the thread */
    void (*proc)(int);      /* procedure for body of thread */
    int param;              /* parameter to base procedure */
    jmp_buf state;          /* the state of the thread */

    /* size bytes of stack follow here */
};

#define thread_null ((struct thread *) 0)

struct thread_queue {
    struct thread *head;
    struct thread *tail;
};


/***
 * Fundamental scheduler data structures
 */

static struct thread_queue readylist;   /* the list of all ready threads */
static struct thread *current;          /* the current running thread */

static char *thread_error;              /* string giving package death cause */
static jmp_buf thread_death;            /* used on thread package death */         

/***
 * code for thread_queue data type
 */

static void thread_queue_init(struct thread_queue *q)
/* initialize q */
{
    q->head = thread_null;
    q->tail = thread_null;
}

static void thread_enqueue(struct thread *t, struct thread_queue *q)
/* enqueue t on q */
{
    spinlock();
    t->next = thread_null;
    if (q->head == thread_null) {
        q->head = t;
        q->tail = t;
    } else {
        q->tail->next = t;
        q->tail = t;
    }
    spinunlock();
}

static struct thread *thread_dequeue(struct thread_queue *q)
/* dequeue and return a thread form q */
{
    struct thread *t;

    spinlock();
    if (q->head == thread_null) {
        t = thread_null;
    } else {
        t = q->head;
        q->head = t->next;
    }

    spinunlock();

    return t;
}

static unsigned int stack_alloc(int size)
{
    static unsigned int __sp = 0;
    unsigned int rsp;

//printf("stack_alloc stack %08x, %08x\n", get_sp(), get_sp() + 32);
    
    if (__sp == 0)
        __sp = get_sp() - 176; /* idle thread reserve 176 bytes stack */

    rsp = __sp;
    __sp -= size;

    return rsp;
}

/**
 * user callable thread management routines 
 */
void thread_manager_init()
/* call this once before launching any threads */
{
    
    /* now build thread manager data structrues */
    thread_queue_init(&readylist);
    current = thread_null;
}

void thread_manager_start()
/* call this once after launching at least one thread */
/* this only returns when all threads are blocked! */
{

//printf("thread_manager_start stack %08x\n", get_sp());    
    current = thread_dequeue(&readylist);
    if (current == thread_null) {
        /* crisis */
        printf("Thread manager start failure, no threads!\n");
        abort();
    }

    if (_setjmp(thread_death)) {
        /* comes here when _longjmp (thread_death, 1) done */
        printf("Thread manager terminated, %s!\n", thread_error);
        while(1) { /* idle loop */
            current = thread_dequeue(&readylist);
            if (current != thread_null)
                _longjmp(current->state, 1);
            cpu_wait();
        }
    } else {
//printf("longjmp current %p sp %08lx ra %08lx\n", current, current->state[0], current->state[1]);        
//printf("size %d proc %p param %d\n", current->size, current->proc, current->param);
        _longjmp(current->state, 1);
    }
}

void thread_yield()
/* call this within a thread to allow scheduling of a new thread */
{
    if (_setjmp(current->state) == 0) {
        thread_enqueue(current, &readylist);
        current = thread_dequeue(&readylist);
        _longjmp(current->state, 1);
    }
}

void thread_exit()
/* call thsi within a thread to terminate that thread */
{
    /* now, get the next thread to run */
    current = thread_dequeue(&readylist);
    if (current == thread_null) {
        /* crisis */
        thread_error = "ready list empty";
        _longjmp(thread_death, 1);
    }

    _longjmp(current->state, 1);
}

void __thread_create(int size, void (*proc)(int), int param, long int usp)
/* call this to launch proc(param) as a thread */
/* may be called from main or from any thread */
{
    struct thread *t;

//printf("thread_create stack = %08x %08lx (%ld)\n", get_sp(), usp, usp - get_sp());    
    t = (struct thread *)stack_alloc(size + sizeof(struct thread));
    t = (struct thread *)(((char *)t) - sizeof(struct thread));
//printf("    t =  %08x (%d)\n", (int)t, get_sp() - (int)t); 
    t->size = size + sizeof(struct thread);
    t->proc = proc;
    t->param = param;
    
    if (_setjmp(t->state)) {
//printf("t = %p stack = %08x current = %p\n", t, get_sp(), current);        
//printf("size %d proc %p param %d\n", current->size, current->proc, current->param);
        /* come here only when new thread scheduled first time */
        (*current->proc)(current->param);
        thread_exit();
    } else {
        /* continue initialization */
        long int    *s;
        long int    local_base = get_sp();  /* address of local t*/
        long int    new_base = (long int)((char *)t - (usp - get_sp()));
        long int    *src, *dst;
        int i;
        src = (long int *)local_base;
        dst = (long int *)new_base;
        for (i = 0; i <= (usp - get_sp()); i+= sizeof(long int))
            *dst++ = *src++;

        /* this following code adjusts the references to the
         * activation record in the saved thread state so that
         * they point to the base of the newly allocated stack 
         */
        s = (long int *)(t->state);
//        printf("s[0] %08lx [1] %08lx local_base %08lx new_base %08lx\n", s[0], s[1], local_base, new_base);
        s[0] = new_base;
//        printf("s[0] %08lx\n", s[0]);
//        printf("t %p size %d proc %p param %d\n", t, t->size, t->proc, t->param);
        thread_enqueue(t, &readylist);
    }
}


void thread_safety_check(void)
/* call this to check for thread stack overflow */
{
    long int t = (long int) current + sizeof(struct thread);
    if (((long int) &t < t) || (long int)&t > (t+current->size)) {
        /* crisis */
        thread_error = "thread stack overflow";
        _longjmp(thread_death, 1);
    }
}

/***
 * thread manager extension for semaphores
 */

/* semaphore representation know only to semaphore methods */
struct thread_semaphore {
    int count;
    struct thread_queue queue;
};

/* meathods applying to semaphores */
void thread_semaphore_init(struct thread_semaphore *s, int v)
/* call this to initialize seamphore s to a count of v */
{
    s->count = v;
    thread_queue_init(&s->queue);
}

void thread_wait(struct thread_semaphore *s)
/* call this within a thread to block on a semaphore */
{
    if (s->count > 0) {
        atomic_fetch_sub_4((uint32_t *)&s->count, 1);
    } else {
        if (_setjmp(current->state) == 0) {
            thread_enqueue(current, &s->queue);
            current = thread_dequeue(&readylist);
            if (current == thread_null) {
                /* crisis */
                thread_error = "possible deadlock";
                _longjmp(thread_death, 1);
            }
            _longjmp(current->state, 1);
        }
    }
}

void thread_signal(struct thread_semaphore *s)
/* call this within a thread to signal a semaphore */
{
    struct thread *t = thread_dequeue(&s->queue);
    if (t == thread_null) {
        atomic_fetch_add_4((uint32_t *)&s->count,1);
    } else {
        thread_enqueue(t, &readylist);
    }
}



