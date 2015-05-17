/* Open-MPI
 * Copyright (c) 2014-2015 Intel, Inc.  All rights reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "mqar.h"
#include "libevent2022.h"
#include "progress_threads.h"

struct mqar_thread_t;
typedef struct mqar_thread_t mqar_thread_t;
typedef void (*mqar_thread_fn_t) (mqar_thread_t *);

struct mqar_thread_t {
    mqar_thread_fn_t t_run;
    void* t_arg;
    pthread_t t_handle;
};

static void
mqar_thread_construct(mqar_thread_t *t)
{
    t->t_run = 0;
    t->t_handle = (pthread_t) -1;
}

/* create a tracking object for progress threads */
typedef struct {
    char *name;
    mqar_event_base_t *ev_base;
    volatile bool ev_active;
    bool block_active;
    mqar_event_t block;
    bool engine_defined;
    mqar_thread_t engine;
    int pipe[2];
} mqar_progress_tracker_t;

static void
mqar_progress_tracker_construct(mqar_progress_tracker_t *p)
{
    p->name = NULL;
    p->ev_base = NULL;
    p->ev_active = true;
    p->block_active = false;
    p->engine_defined = false;
    p->pipe[0] = -1;
    p->pipe[1] = -1;
}

static void
mqar_progress_tracker_destruct(void *data)
{
    mqar_progress_tracker_t *p = (mqar_progress_tracker_t *)data;
    
    if (NULL != p->name) {
        free(p->name);
    }
    if (p->block_active) {
        mqar_event_del(&p->block);
    }
    if (NULL != p->ev_base) {
        mqar_event_base_free(p->ev_base);
    }
    if (0 <= p->pipe[0]) {
        close(p->pipe[0]);
    }
    if (0 <= p->pipe[1]) {
        close(p->pipe[1]);
    }
    free(p);
}

static zhash_t *tracking;
static bool inited = false;

static void
wakeup(int fd, short args, void *cbdata)
{
    mqar_progress_tracker_t *trk = (mqar_progress_tracker_t*)cbdata;

    /* if this event fired, then the blocker event will
     * be deleted from the event base by libevent, so flag
     * it so we don't try to delete it again */
    trk->block_active = false;
}

static void
progress_engine(mqar_thread_t *t)
{
    mqar_progress_tracker_t *trk = (mqar_progress_tracker_t*)t->t_arg;

    while (trk->ev_active) {
        mqar_event_loop(trk->ev_base, MQAR_EVLOOP_ONCE);
    }
    return;
}

static int
mqar_thread_start(mqar_thread_t *t)
{
    int rc;
    
    if (NULL == t->t_run || t->t_handle != (pthread_t) -1) {
        zclock_log("E: thread bad parameter: ");
        return MQAR_ERROR;
    }
    
    rc = pthread_create(&t->t_handle, NULL, (void*(*)(void*)) t->t_run, t);
    
    return (rc == 0) ? MQAR_SUCCESS : MQAR_ERROR;
}

static int
mqar_thread_join(mqar_thread_t *t, void **thr_return)
{
    int rc = pthread_join(t->t_handle, thr_return);
    t->t_handle = (pthread_t) -1;
    return (rc == 0) ? MQAR_SUCCESS : MQAR_ERROR;
}

static int
mqar_fd_set_cloexec(int fd)
{
    int flags;
    
    flags = fcntl(fd, F_GETFD, 0);
    if (-1 == flags) {
        return MQAR_ERROR;
    }
    
    if (fcntl(fd, F_SETFD, FD_CLOEXEC | flags) == -1) {
        return MQAR_ERROR;
    }
    
    return MQAR_SUCCESS;
}

int
mqar_init_progress_threads()
{
    evthread_use_pthreads();

    return MQAR_SUCCESS;
}

mqar_event_base_t *
mqar_start_progress_thread(char *name, bool create_block)
{
    mqar_progress_tracker_t *trk;
    int rc;
    struct event_config *config=NULL;
    
    trk = (mqar_progress_tracker_t *)malloc(sizeof(mqar_progress_tracker_t));
    if (NULL == trk) {
        zclock_log("E: progress tracker unable to malloc: ");
        return NULL;
    }
    mqar_progress_tracker_construct(trk);
    trk->name = strdup(name);
    zclock_log("I: creating progress thread: %s", name);
    
    /* event setup */
    config = event_config_new();
    event_config_avoid_method(config, "select");

    if (NULL == (trk->ev_base = event_base_new_with_config(config))) {
        zclock_log("E: progress tracker unable to create event: ");
        event_config_free(config);
        free(trk);
        return NULL;
    }
    
    event_config_free(config);

    if (create_block) {
        /* add an event it can block on */
        if (0 > pipe(trk->pipe)) {
            zclock_log("E: tracker pipe failed: ");
            free(trk);
            return NULL;
        }
        /* Make sure the pipe FDs are set to close-on-exec so that
           they don't leak into children */
        if (mqar_fd_set_cloexec(trk->pipe[0]) != MQAR_SUCCESS ||
            mqar_fd_set_cloexec(trk->pipe[1]) != MQAR_SUCCESS) {
            zclock_log("E: progress tracker unable to set cloexec: ");
            free(trk);
            return NULL;
        }
        mqar_event_set(trk->ev_base, &trk->block, trk->pipe[0],
                       MQAR_EV_READ, wakeup, trk);
        mqar_event_add(&trk->block, 0);
        trk->block_active = true;
    }

    /* construct the thread object */
    mqar_thread_construct(&trk->engine);
    trk->engine_defined = true;
    /* fork off a thread to progress it */
    trk->engine.t_run = progress_engine;
    trk->engine.t_arg = trk;
    if (MQAR_SUCCESS != (rc = mqar_thread_start(&trk->engine))) {
        zclock_log("E: progress tracker unable to start thread: ");
        free(trk);
        return NULL;
    }
    if (!inited) {
        tracking = zhash_new();
        inited = true;
    }
    zhash_insert(tracking, name, (void *)trk);
    zhash_freefn(tracking, name, mqar_progress_tracker_destruct);
    return trk->ev_base;
}

void
mqar_stop_progress_thread(char *name, bool cleanup)
{
    mqar_progress_tracker_t *trk;
    int i;

    if (!inited) {
        /* nothing we can do */
        return;
    }

    zclock_log("I: stopping progress thread: %s", name);
    /* find the specified engine */
    trk = (mqar_progress_tracker_t *)zhash_lookup(tracking, name);

    if (NULL != trk) {
        /* if it is already inactive, then just cleanup if that
         * is the request */
        if (!trk->ev_active) {
            if (cleanup) {
                zhash_delete(tracking, name);
            }
            return;
        }
        /* mark it as inactive */
        trk->ev_active = false;
        /* break the event loop - this will cause the loop to exit
         * upon completion of any current event */
        mqar_event_base_loopbreak(trk->ev_base);
        /* if present, use the block to break it loose just in
         * case the thread is blocked in a call to select for
         * a long time */
        if (trk->block_active) {
            i=1;
            write(trk->pipe[1], &i, sizeof(int));
        }
        /* wait for thread to exit */
        mqar_thread_join(&trk->engine, NULL);
        /* cleanup, if they indicated they are done with this event base */
        if (cleanup) {
            zhash_delete(tracking, name);
        }
        return;
    }
}

/* TODO: Cleanup all progress threads */
