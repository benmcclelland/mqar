/* Open-MPI
 * Copyright (c) 2010      Cisco Systems, Inc.  All rights reserved.
 * Copyright (c) 2010      Oracle and/or its affiliates.  All rights reserved.
 * Copyright (c) 2012-2013 Los Alamos National Security, LLC.
 *                         All rights reserved.
 * Copyright (c) 2015      Intel, Inc. All rights reserved.
 *
 * $COPYRIGHT$
 * 
 * Additional copyrights may follow
 * 
 * $HEADER$
 *
 * When this component is used, this file is included in the rest of
 * the OPAL/ORTE/OMPI code base via opal/mca/event/event.h.  As such,
 * this header represents the public interface to this static component.
 */

#ifndef MCA_OPAL_EVENT_LIBEVENT2022_H
#define MCA_OPAL_EVENT_LIBEVENT2022_H

#include <sys/types.h>
#include <sys/time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <signal.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#include "event.h"
#include "event2/thread.h"

typedef event_callback_fn mqar_event_cbfunc_t;

/* set the number of event priority levels */
#define MQAR_EVENT_NUM_PRI   8

#define MQAR_EV_ERROR_PRI         0
#define MQAR_EV_MSG_HI_PRI        1
#define MQAR_EV_SYS_HI_PRI        2
#define MQAR_EV_MSG_LO_PRI        3
#define MQAR_EV_SYS_LO_PRI        4
#define MQAR_EV_INFO_HI_PRI       5
#define MQAR_EV_INFO_LO_PRI       6
#define MQAR_EV_LOWEST_PRI        7

#define MQAR_EVENT_SIGNAL(ev)	mqar_event_get_signal(ev)

#define MQAR_TIMEOUT_DEFAULT	{1, 0}


typedef struct event_base mqar_event_base_t;
typedef struct event mqar_event_t;

extern mqar_event_base_t *mqar_event_base;

#define MQAR_EV_TIMEOUT EV_TIMEOUT
#define MQAR_EV_READ    EV_READ
#define MQAR_EV_WRITE   EV_WRITE
#define MQAR_EV_SIGNAL  EV_SIGNAL
/* Persistent event: won't get removed automatically when activated. */
#define MQAR_EV_PERSIST EV_PERSIST

#define MQAR_EVLOOP_ONCE     EVLOOP_ONCE        /**< Block at most once. */
#define MQAR_EVLOOP_NONBLOCK EVLOOP_NONBLOCK    /**< Do not block. */

/* Global function to create and release an event base */
mqar_event_base_t* mqar_event_base_create(void);

#define mqar_event_base_free(x) event_base_free(x)

int mqar_event_init(void);

#define mqar_event_reinit(b) event_reinit((b))

#define mqar_event_base_init_common_timeout (b, t) event_base_init_common_timeout((b), (t))

#define mqar_event_base_loopbreak(b) event_base_loopbreak(b)

#define mqar_event_base_loopexit(b) event_base_loopexit(b, NULL)

/* Event priority APIs */
#define mqar_event_base_priority_init(b, n) event_base_priority_init((b), (n))

#define mqar_event_set_priority(x, n) event_priority_set((x), (n))

/* thread support APIs */
#define mqar_event_use_threads() evthread_use_pthreads()

/* Basic event APIs */
#define mqar_event_enable_debug_mode() event_enable_debug_mode()

#define mqar_event_set(b, x, fd, fg, cb, arg) event_assign((x), (b), (fd), (fg), (event_callback_fn) (cb), (arg))

#define mqar_event_add(ev, tv) event_add((ev), (tv))

#define mqar_event_del(ev) event_del((ev))

#define mqar_event_active(x, y, z) event_active((x), (y), (z))

#define mqar_event_new(b, fd, fg, cb, arg) event_new((b), (fd), (fg), (event_callback_fn) (cb), (arg))

mqar_event_t* mqar_event_alloc(void);

#define mqar_event_free(x) event_free((x))

/* Timer APIs */
#define mqar_event_evtimer_new(b, cb, arg) mqar_event_new((b), -1, 0, (cb), (arg)) 

#define mqar_event_evtimer_add(x, tv) mqar_event_add((x), (tv))

#define mqar_event_evtimer_set(b, x, cb, arg) event_assign((x), (b), -1, 0, (event_callback_fn) (cb), (arg))

#define mqar_event_evtimer_del(x) mqar_event_del((x))

#define mqar_event_evtimer_pending(x, tv) event_pending((x), EV_TIMEOUT, (tv))

#define mqar_event_evtimer_initialized(x) event_initialized((x))

/* Signal APIs */
#define mqar_event_signal_add(x, tv) event_add((x), (tv))

#define mqar_event_signal_set(b, x, fd, cb, arg) event_assign((x), (b), (fd), EV_SIGNAL|EV_PERSIST, (event_callback_fn) (cb), (arg))

#define mqar_event_signal_del(x) event_del((x))

#define mqar_event_signal_pending(x, tv) event_pending((x), EV_SIGNAL, (tv))

#define mqar_event_signal_initalized(x) event_initialized((x))

#define mqar_event_get_signal(x) event_get_signal((x))

#define mqar_event_loop(b, fg) event_base_loop((b), (fg))

#endif /* MCA_OPAL_EVENT_LIBEVENT2022_H */
