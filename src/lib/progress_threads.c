/*
 * Copyright (c) 2014-2015 Intel, Inc.  All rights reserved. 
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 */

#include "libevent2022.h"
#include "progress_threads.h"

/* create a tracking object for progress threads */
typedef struct {
    mqar_list_item_t super;
    char *name;
    mqar_event_base_t *ev_base;
    volatile bool ev_active;
    bool block_active;
    mqar_event_t block;
    bool engine_defined;
    mqar_thread_t engine;
    int pipe[2];
} mqar_progress_tracker_t;
static void trkcon(mqar_progress_tracker_t *p)
{
    p->name = NULL;
    p->ev_base = NULL;
    p->ev_active = true;
    p->block_active = false;
    p->engine_defined = false;
    p->pipe[0] = -1;
    p->pipe[1] = -1;
}
static void trkdes(mqar_progress_tracker_t *p)
{
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
    if (p->engine_defined) {
        OBJ_DESTRUCT(&p->engine);
    }
}
static OBJ_CLASS_INSTANCE(mqar_progress_tracker_t,
                          mqar_list_item_t,
                          trkcon, trkdes);

static mqar_list_t tracking;
static bool inited = false;
static void wakeup(int fd, short args, void *cbdata)
{
    mqar_progress_tracker_t *trk = (mqar_progress_tracker_t*)cbdata;

    /* if this event fired, then the blocker event will
     * be deleted from the event base by libevent, so flag
     * it so we don't try to delete it again */
    trk->block_active = false;
}
static void* progress_engine(mqar_object_t *obj)
{
    mqar_thread_t *t = (mqar_thread_t*)obj;
    mqar_progress_tracker_t *trk = (mqar_progress_tracker_t*)t->t_arg;

    while (trk->ev_active) {
        mqar_event_loop(trk->ev_base, MQAR_EVLOOP_ONCE);
    }
    return MQAR_THREAD_CANCELLED;
}

mqar_event_base_t *mqar_start_progress_thread(char *name,
                                              bool create_block)
{
    mqar_progress_tracker_t *trk;
    int rc;

    trk = OBJ_NEW(mqar_progress_tracker_t);
    trk->name = strdup(name);
    if (NULL == (trk->ev_base = mqar_event_base_create())) {
        MQAR_ERROR_LOG(MQAR_ERR_OUT_OF_RESOURCE);
        OBJ_RELEASE(trk);
        return NULL;
    }

    if (create_block) {
        /* add an event it can block on */
        if (0 > pipe(trk->pipe)) {
            MQAR_ERROR_LOG(MQAR_ERR_IN_ERRNO);
            OBJ_RELEASE(trk);
            return NULL;
        }
        /* Make sure the pipe FDs are set to close-on-exec so that
           they don't leak into children */
        if (mqar_fd_set_cloexec(trk->pipe[0]) != MQAR_SUCCESS ||
            mqar_fd_set_cloexec(trk->pipe[1]) != MQAR_SUCCESS) {
            MQAR_ERROR_LOG(MQAR_ERR_IN_ERRNO);
            OBJ_RELEASE(trk);
            return NULL;
        }
        mqar_event_set(trk->ev_base, &trk->block, trk->pipe[0], MQAR_EV_READ, wakeup, trk);
        mqar_event_add(&trk->block, 0);
        trk->block_active = true;
    }

    /* construct the thread object */
    OBJ_CONSTRUCT(&trk->engine, mqar_thread_t);
    trk->engine_defined = true;
    /* fork off a thread to progress it */
    trk->engine.t_run = progress_engine;
    trk->engine.t_arg = trk;
    if (MQAR_SUCCESS != (rc = mqar_thread_start(&trk->engine))) {
        MQAR_ERROR_LOG(rc);
        OBJ_RELEASE(trk);
        return NULL;
    }
    if (!inited) {
        OBJ_CONSTRUCT(&tracking, mqar_list_t);
        inited = true;
    }
    mqar_list_append(&tracking, &trk->super);
    return trk->ev_base;
}

void mqar_stop_progress_thread(char *name, bool cleanup)
{
    mqar_progress_tracker_t *trk;
    int i;

    if (!inited) {
        /* nothing we can do */
        return;
    }

    /* find the specified engine */
    OPAL_LIST_FOREACH(trk, &tracking, mqar_progress_tracker_t) {
        if (0 == strcmp(name, trk->name)) {
            /* if it is already inactive, then just cleanup if that
             * is the request */
            if (!trk->ev_active) {
                if (cleanup) {
                    mqar_list_remove_item(&tracking, &trk->super);
                    OBJ_RELEASE(trk);
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
                mqar_list_remove_item(&tracking, &trk->super);
                OBJ_RELEASE(trk);
            }
            return;
        }
    }
}

int mqar_restart_progress_thread(char *name)
{
    mqar_progress_tracker_t *trk;
    int rc;

    if (!inited) {
        /* nothing we can do */
        return MQAR_ERROR;
    }

    /* find the specified engine */
    OPAL_LIST_FOREACH(trk, &tracking, mqar_progress_tracker_t) {
        if (0 == strcmp(name, trk->name)) {
            if (!trk->engine_defined) {
                MQAR_ERROR_LOG(MQAR_ERR_NOT_SUPPORTED);
                return MQAR_ERR_NOT_SUPPORTED;
            }
            /* ensure the block is set, if requested */
            if (0 <= trk->pipe[0] && !trk->block_active) {
                mqar_event_add(&trk->block, 0);
                trk->block_active = true;
            }
            /* start the thread again */
            if (MQAR_SUCCESS != (rc = mqar_thread_start(&trk->engine))) {
                MQAR_ERROR_LOG(rc);
                return rc;
            }
        }
    }

    return MQAR_ERR_NOT_FOUND;
}
